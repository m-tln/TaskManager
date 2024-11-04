#include <iostream>
#include "TaskManager.hpp"

using namespace std::chrono_literals;

std::mutex mutex;

void Print1() {
  mutex.lock();
  for (size_t i = 0; i < 10; ++i) {
    std::cout << "A";
    std::this_thread::sleep_for(200ms);
    std::cout.flush();
  }
  mutex.unlock();
}

void Print2() {
  mutex.lock();
  for (size_t i = 0; i < 10; ++i) {
    std::cout << "B";
    std::this_thread::sleep_for(200ms);
    std::cout.flush();
  }
  mutex.unlock();
}

void Print3() {
  mutex.lock();
  for (size_t i = 0; i < 10; ++i) {
    std::cout << "C";
    std::this_thread::sleep_for(200ms);
    std::cout.flush();
  }
  mutex.unlock();
}

void Print4() {
  for (size_t i = 0; i < 10; ++i) {
    std::cout << "D";
//    std::this_thread::sleep_for(200ms);
//    std::cout.flush();
  }
}


int main() {
  TaskManager task_manager;
  const auto now = std::chrono::system_clock::now();
  task_manager.AddTask(&Print1, std::chrono::system_clock::to_time_t(now) + 100);
  task_manager.AddTask(&Print2, std::chrono::system_clock::to_time_t(now));
  task_manager.AddTask(&Print3, std::chrono::system_clock::to_time_t(now) + 10000);
  std::cout.flush();

  std::this_thread::sleep_for(2000ms);
//  task_manager.AddTask(&Print4, std::chrono::system_clock::to_time_t(now));
//  std::thread thread1(Print1);
//  std::thread thread2(Print2);
//  std::thread thread3(Print3);
//
//  thread1.join();
//  thread2.join();
//  thread3.join();
}
