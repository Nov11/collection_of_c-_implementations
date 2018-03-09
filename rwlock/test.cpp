//
// Created by c6s on 18-3-9.
//

#include <thread>
#include <vector>
#include <iostream>
#include <sstream>
#include "rwlock.h"
using namespace std;
RWlock rWlock;
int value = 0;
void reader() {
  std::this_thread::sleep_for(2s);
  rWlock.readLock();
  stringstream ss;
  ss << std::this_thread::get_id() << " read:" << value << endl;
  cout << ss.str();
  rWlock.readUnlock();
}
void writer() {
  std::this_thread::sleep_for(2s);
  rWlock.writeLock();
  value++;
  stringstream ss;
  ss << std::this_thread::get_id() << " write:" << value << endl;
  cout << ss.str();
  rWlock.writeUnlock();
}
int main() {
  vector<std::thread> v;
  for (int i = 0; i < 50; i++) {
    v.emplace_back(reader);
    if (i % 10 == 0) {
      v.emplace_back(writer);
    }
  }
  for (auto &t : v) {
    t.join();
  }
}