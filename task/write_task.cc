//
// Created by Homin Su on 2021/10/2.
//

#include <iostream>
#include <thread>
#include <string>

#include "write_task.h"
#include "../memory/data.h"

/**
 * @brief 初始化写出任务
 * @param _file_name 文件名
 * @return 初始化状态
 */
bool WriteTask::Init(const std::string &_file_name) {
  if (_file_name.empty()) {
    return false;
  }
  file_name_ = _file_name;
  return true;
}

/**
 * @brief 线程主函数
 */
void WriteTask::Main() {
#ifdef Debug
  std::cout << "XCryptTask::Main() Start, thread id: " << std::this_thread::get_id() << std::endl;
#endif

  size_t write_bytes = 0;

  constexpr size_t data_size = KB(8);
  constexpr size_t up_data_limit_size = MB(5);

  // 编译期异常检测
  static_assert(data_size > KB(1) && data_size < MB(1), "data_size must between 1KB and 1MB");
  static_assert(up_data_limit_size > data_size, "up_data_limit_size must greater than data_size");

  // 设置上游节点的下游状态
  prev_->next_status_ = true;

  if (!OpenFile()) {
    set_return(0);
    return;
  }

  while (is_running) {
    // 当数据块总和小于 5MB 时通知上游，解除阻塞，继续读文件
    if (prev_status_ && DataListNum() <= LimitNum(up_data_limit_size, data_size)) {
      prev_->cv_.notify_one();
    }

    auto data = PopFront(); // 弹出一个数据块指针
    if (nullptr == data) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    // 数据块写入到文件中
    ofs_.write(static_cast<char *>(data->data()), static_cast<long>(data->size()));
    write_bytes += data->size();
    if (data->end()) {
      break;
    }
  }
  // 关闭文件流
  ofs_.close();

#ifdef Debug
  std::cout << "WriteTask::Main() End, thread id: " << std::this_thread::get_id() << std::endl;
#endif

  set_return(write_bytes);
}

/**
 * @brief 打开文件
 * @return 是否打开
 */
bool WriteTask::OpenFile() {
  ofs_.open(file_name_, std::ios::binary);
  if (!ofs_) {
    std::cerr << "open file: " << file_name_ << " failed" << std::endl;
    return false;
  }
#ifdef Debug
  std::cout << "open file: " << file_name_ << " success" << std::endl;
#endif
  return true;
}
