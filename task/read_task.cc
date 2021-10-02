//
// Created by Homin Su on 2021/10/2.
//

#include <string>
#include <iostream>

#include "read_task.h"
#include "../memory/data.h"

/**
 * @brief 初始化读取任务，获取文件大小
 * @param _file_name 文件名
 * @return 是否初始化成功
 */
bool ReadTask::Init(const std::string &_file_name) {
  if (_file_name.empty()) {
    return false;
  }
  ifs_.open(_file_name, std::ios::binary);
  if (!ifs_) {
    std::cerr << "open file: " << _file_name << " failed" << std::endl;
    return false;
  }
#ifdef Debug
  std::cout << "open file: " << _file_name << " success" << std::endl;
#endif

  // 计算文件长度
  ifs_.seekg(0, std::ios::end);
  data_bytes_ = ifs_.tellg();
  ifs_.seekg(0, std::ios::beg);

#ifdef Debug
  std::cout << "file size = " << data_bytes_ << std::endl;
#endif

  return true;
}

/**
 * 线程入口函数
 */
void ReadTask::Main() {
#ifdef Debug
  std::cout << "XReadTask::Main() Start" << std::endl;
#endif
  while (is_running()) {
    if (ifs_.eof()) {
      break;
    }

    // 创建内存池空间管理对象
    auto data = Data::Make(memory_resource_);
    int data_size = 1024;

    // 申请空间
    auto buf = data->New(data_size);

    // 读取文件
    ifs_.read(static_cast<char *>(buf), data_size);
    if (ifs_.gcount() <= 0) {
      break;
    }

    data->set_size(ifs_.gcount());

    if (ifs_.eof()) {
      data->set_end(true);
    }
#ifdef Debug
    std::cout << "[" << ifs_.gcount() << "]" << std::flush;
#endif

    if (nullptr != next_) {
      next_->PushBack(data);
    }
  }
  ifs_.close();
#ifdef Debug
  std::cout << std::endl << "XReadTask::Main() End" << std::endl;
#endif

  set_return(0);
}
