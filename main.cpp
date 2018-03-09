#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
class ThreadPool {
  bool closed;
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex mtx;
  std::condition_variable cond;
  void workRoutine() {
    while (true) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(mtx);
        while (!closed && tasks.empty()) {
          cond.wait(lock);
        }
        if (closed && tasks.empty()) {
          return;
        }
        task = std::move(tasks.front());
        tasks.pop();
      }
      task();
    }
  }
 public:
  ThreadPool(int numberOfWorkers) :
      closed(false) {
    for (int i = 0; i < numberOfWorkers; i++) {
      workers.emplace_back(&ThreadPool::workRoutine, this);
    }
  }

  ~ThreadPool() {
    {
      std::lock_guard<std::mutex> lock_guard(mtx);
      closed = true;
    }
    cond.notify_all();
    for (auto &worker : workers) {
      worker.join();
    }
  }

  template<typename F, typename ... Args>
  auto addNewTask(F &&f, Args &&... args)
  -> std::future<typename std::result_of<F(Args...)>::type> {
    using Ret = typename std::result_of<F(Args...)>::type;
    auto function = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto taskPtr = std::make_shared<std::packaged_task<Ret()>>(std::move(function));
    {
      std::lock_guard<std::mutex> lock_guard(mtx);
      if (closed) {
        throw std::runtime_error("add new task to closed thread pool");
      }
      tasks.emplace([taskPtr]() { (*taskPtr)(); });
    }
    cond.notify_one();
    return taskPtr->get_future();
  };
};

int main() {
  std::cout << "Hello, World!" << std::endl;
  // create thread pool with 4 worker threads
  ThreadPool pool(4);

  // enqueue and store future
  auto result = pool.addNewTask([](int answer) { return answer; }, 42);

  // get result from future
  std::cout << result.get() << std::endl;
  return 0;
}