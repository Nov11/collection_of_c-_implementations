//
// Created by c6s on 18-3-9.
//

#ifndef COLLECTIONS_RWLOCK_H
#define COLLECTIONS_RWLOCK_H

#include <mutex>
#include <condition_variable>
#include <climits>
#include <cassert>
class RWlock {
  bool write_entered;
  size_t reader_count;
  std::mutex mtx;
  std::condition_variable reader;
  std::condition_variable writer;
 public:
  RWlock() : write_entered(false), reader_count(0) {}
  RWlock(const RWlock &) = delete;
  RWlock &operator=(const RWlock &) = delete;

  void readLock() {
    std::unique_lock<std::mutex> lock(mtx);
    while (write_entered) {
      reader.wait(lock);
    }
    if (reader_count == INT_MAX) {
      throw std::runtime_error("possibly request too many read locks");
    }
    reader_count++;
  }
  void readUnlock() {
    std::lock_guard<std::mutex> guard(mtx);
    if (reader_count == 0) {
      throw std::runtime_error("try release read lock without any reader holding the lock");
    }
    reader_count--;
    if (write_entered && reader_count == 0) {
      writer.notify_one();
    }
  }
  void writeLock() {
    std::unique_lock<std::mutex> lock(mtx);
    while (write_entered) {
      reader.wait(lock);
    }
    write_entered = true;
    while (reader_count) {
      writer.wait(lock);
    }
  }
  void writeUnlock() {
    std::lock_guard<std::mutex> lock(mtx);
    write_entered = false;
    reader.notify_all();
  }
};

class rwlockwithreaderlimit {
  bool writer_entered;
  size_t reader_count;
  size_t reader_limit;
  std::mutex mtx;
  std::condition_variable reader;
  std::condition_variable writer;
 public:
  rwlockwithreaderlimit() : writer_entered(false), reader_count(0), reader_limit(INT_MAX) {}
  rwlockwithreaderlimit(const rwlockwithreaderlimit &) = delete;
  rwlockwithreaderlimit &operator=(const rwlockwithreaderlimit &)= delete;
  void RLock() {
    std::unique_lock<std::mutex> lock(mtx);
    while (writer_entered || reader_count == reader_limit) {
      reader.wait(lock);
    }
    reader_count++;
  }
  void RUnlock() {
    std::lock_guard<std::mutex> guard(mtx);
    assert(reader_count > 0);
    reader_count--;
    if (writer_entered) {
      if (reader_count == 0) {
        writer.notify_one();
      }
    } else {
      if (reader_count + 1 == reader_limit) {
        reader.notify_one();
      }
    }
  }
  void WLock() {
    std::unique_lock<std::mutex> lock(mtx);
    while (writer_entered) {
      reader.wait(lock);
    }
    writer_entered = true;
    while (reader_count != 0) {
      writer.wait(lock);
    }
  }
  void WUnlock() {
    std::lock_guard<std::mutex> guard(mtx);
    writer_entered = false;
    reader.notify_all();
  }
};

#endif //COLLECTIONS_RWLOCK_H
