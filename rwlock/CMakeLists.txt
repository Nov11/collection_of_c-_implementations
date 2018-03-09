find_package (Threads)
add_executable(testdrive test.cpp rwlock.h)
target_link_libraries(testdrive ${CMAKE_THREAD_LIBS_INIT})