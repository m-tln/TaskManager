#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <array>

#ifndef TASK_MANAGER__TASKMANAGER_HPP_
#define TASK_MANAGER__TASKMANAGER_HPP_

namespace Constants {
constexpr uint kMaxThreads = 12;
}

class TaskManager {
 public:
  void AddTask(std::function<void()> task, std::time_t timestamp);

  TaskManager();
  ~TaskManager();
 private:
  struct Task {
    std::function<void()> task;
    std::time_t timestamp;

    Task(std::function<void()> task, std::time_t timestamp)
        : task(task), timestamp(timestamp) {}

    bool operator<(Task other) const { return timestamp >= other.timestamp; }
  };

  bool end_ = false;
  std::priority_queue<Task> tasks_;
  std::mutex tasks_mutex_;
  std::condition_variable cond_var_;
  std::array<std::thread, Constants::kMaxThreads> threads_;

  void Run();
};

#endif  //TASK_MANAGER__TASKMANAGER_HPP_
