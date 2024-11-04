#include "TaskManager.hpp"

TaskManager::TaskManager() {
  for (auto&& thread : threads_) {
    std::thread cur(&TaskManager::Run, this);
    thread = std::move(cur);
  }
}

TaskManager::~TaskManager() {
  {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    end_ = true;
  }
  cond_var_.notify_all();
  for (auto&& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void TaskManager::AddTask(std::function<void()> task, std::time_t timestamp) {
  tasks_mutex_.lock();
  tasks_.emplace(task, timestamp);
  tasks_mutex_.unlock();
  cond_var_.notify_one();
}

void TaskManager::Run() {
  for (;;) {
    std::unique_lock<std::mutex> lock(tasks_mutex_);
    cond_var_.wait(lock);
    while (!tasks_.empty()) {
      Task task = tasks_.top();
      tasks_.pop();
      lock.unlock();
      auto now = std::chrono::system_clock::now();
      if (task.timestamp < std::chrono::system_clock::to_time_t(now)) {
        std::this_thread::sleep_until(
            std::chrono::system_clock::from_time_t(task.timestamp));
      }
      try {
          task.task();
      } catch (...) {
          lock.lock();
          throw;
      }
      lock.lock();
    }
    if (end_) {
      return;
    }
  }
}