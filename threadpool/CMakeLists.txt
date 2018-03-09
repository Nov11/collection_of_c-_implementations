find_package (Threads)
add_executable(threadpool threadpool.cpp)
target_link_libraries (threadpool ${CMAKE_THREAD_LIBS_INIT})