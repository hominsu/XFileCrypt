//
// Created by Homin Su on 2021/9/20.
//

#ifndef XTHREADPOOL_XTHREADPOOL_TEST_TASK_H_
#define XTHREADPOOL_XTHREADPOOL_TEST_TASK_H_

#include <iostream>
#include <sstream>

/**
 * @brief 测试任务
 */
class TestTask : public XTask<std::string> {
 public:
  std::string name_;

 public:
  TestTask() = default;
  explicit TestTask(std::string _name) : name_(std::move(_name)) {};

  void Main() override {
    for (int i = 0; i < 10; ++i) {
      if (!is_running()) {
        break;
      }
      std::stringstream str_run;
      str_run << name_ << ": " << i << std::endl;
      std::cout << str_run.str();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    set_return(std::forward<std::string>(name_));
  }
};

#endif //XTHREADPOOL_XTHREADPOOL_TEST_TASK_H_
