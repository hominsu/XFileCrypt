//
// Created by Homin Su on 2021/10/2.
//

#ifndef XFILECRYPT_TASK_WRITE_WRITE_TASK_H_
#define XFILECRYPT_TASK_WRITE_WRITE_TASK_H_

#include <string>
#include <fstream>

#include "task_base.h"

/**
 * @brief 写出任务
 */
class WriteTask : public TaskBase {
 private:
  std::ofstream ofs_;

 public:
  /**
   * @brief 初始化写出任务
   * @param _file_name 文件名
   * @return 初始化状态
   */
  bool Init(const std::string &_file_name);

 private:
  /**
   * @brief 线程主函数
   */
  void Main() override;
};

#endif //XFILECRYPT_TASK_WRITE_WRITE_TASK_H_
