//
// Created by Homin Su on 2021/10/2.
//

#include <iostream>
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
  std::cout << "XCryptTask::Main() Start" << std::endl;
#endif

  size_t write_bytes = 0;

  if (!OpenFile()) {
    set_return(0);
    return;
  }

  while (is_running) {
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
  std::cout << "WriteTask::Main() End" << std::endl;
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
