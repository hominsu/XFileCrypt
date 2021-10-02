//
// Created by Homin Su on 2021/10/2.
//

#ifndef XFILECRYPT_TASK_READ_READ_TASK_H_
#define XFILECRYPT_TASK_READ_READ_TASK_H_

#include <string>
#include <fstream>

#include "task_base.h"

/**
 * @brief 读取任务
 */
class ReadTask : public TaskBase {
 private:
  std::ifstream ifs_; ///< 读取文件

 public:
  /**
   * @brief 初始化读取任务，获取文件大小
   * @param _file_name 文件名
   * @return 是否初始化成功
   */
  bool Init(const std::string &_file_name);

 private:
  /**
   * 线程入口函数
   */
  void Main() override;
};

#endif //XFILECRYPT_TASK_READ_READ_TASK_H_
