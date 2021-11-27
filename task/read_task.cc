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
  file_name_ = _file_name;
  return true;
}

/**
 * 线程入口函数
 */
void ReadTask::Main() {
#ifdef Debug
  std::cout << "ReadTask::Main() Start, thread id: " << std::this_thread::get_id() << std::endl;
#endif

  size_t read_bytes;
  size_t total_read_bytes = 0;

  if (!OpenFile()) {
    set_return(0);
    return;
  }

  constexpr size_t data_size = KB(512);
  constexpr size_t down_data_limit_size = MB(20);

  // 编译期异常检测
  static_assert(data_size > KB(1) && data_size < MB(1), "data_size must between 1KB and 1MB");
  static_assert(down_data_limit_size > data_size, "down_data_limit_size must greater than data_size");

  // 设置下游节点的上游状态
  next_->prev_status_ = true;

  while (is_running()) {
    if (ifs_.eof()) {
      break;
    }

    // 当下游节点的数据块总和超过 20MB 时阻塞
    if (next_status_) {
      std::unique_lock<std::shared_mutex> lock(cv_mutex_);
      cv_.wait(cv_mutex_, [&]() -> bool {
        return next_->DataListNum() <= LimitNum(down_data_limit_size, data_size);
      });
    }

    // 创建内存池空间管理对象
    auto data = Data::Make(memory_resource_);

    // 申请空间
    auto buf = data->New(data_size);

    // 读取文件
    ifs_.read(static_cast<char *>(buf), data_size);
    read_bytes = ifs_.gcount();

    if (read_bytes <= 0) {
      break;
    }

    data->set_size(read_bytes);
    total_read_bytes += read_bytes;

    if (total_read_bytes == data_bytes_) {
      data->set_end(true);
    }
#ifdef Debug
    //    std::cout << "[" << ifs_.gcount() << "]" << std::flush;
#endif

    if (nullptr != next_) {
      next_->PushBack(data);
    }
  }
  ifs_.close();

#ifdef Debug
  std::cout << std::endl << "ReadTask::Main() End, thread id: " << std::this_thread::get_id() << std::endl;
#endif

  // 设置下游节点的上游状态
  next_->prev_status_ = false;

  set_return(data_bytes_);
}

/**
 * @brief 打开文件
 * @return 是否打开
 */
bool ReadTask::OpenFile() {
  ifs_.open(file_name_, std::ios::binary);
  if (!ifs_) {
    std::cerr << "open file: " << file_name_ << " failed" << std::endl;
    return false;
  }
#ifdef Debug
  std::cout << "open file: " << file_name_ << " success" << std::endl;
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
