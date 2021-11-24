//
// Created by Homin Su on 2021/10/2.
//

#include <iostream>
#include <mutex>

#include "../x_thread_pool/x_thread_pool.h"
#include "file_crypt.h"
#include "crypt_task.h"
#include "write_task.h"
#include "read_task.h"

std::once_flag flag;  ///< 函数单次执行标识

/**
 * @brief 开始文件加解密
 * @param _in_file 输入文件
 * @param _out_file 输出文件
 * @param _password 密码
 * @param _is_encrypt 加密还是解密
 * @return 是否加密成功
 */
bool FileCrypt::Start(const std::string &_in_file,
                      const std::string &_out_file,
                      const std::string &_password,
                      bool _is_encrypt,
                      std::shared_ptr<std::pmr::synchronized_pool_resource> &_memory_resource) {

  // 只初始化一次
  std::call_once(flag, [] {
    auto thread_num = XThreadPool::Get()->Init();
    std::cout << "Thread num = " << thread_num << std::endl;
  });

  in_file_ = _in_file;
  out_file_ = _out_file;

  // 读取任务
  read_task_ = std::make_shared<ReadTask>();
  if (!read_task_->Init(_in_file)) {
    return false;
  }
  read_task_->set_memory_resource(_memory_resource);

  // 写出任务
  write_task_ = std::make_shared<WriteTask>();
  if (!write_task_->Init(_out_file)) {
    return false;
  }
  write_task_->set_memory_resource(_memory_resource);

  // 加解密任务
  crypt_task_ = std::make_shared<CryptTask>();
  crypt_task_->set_memory_resource(_memory_resource);
  crypt_task_->set_is_encrypt(_is_encrypt);
  crypt_task_->Init(_password);

  // 设置责任链: read_task -> crypt_task -> write_task
  read_task_->set_next(crypt_task_);
  crypt_task_->set_next(write_task_);

  // 设置责任链: read_task <- crypt_task <- write_task
  write_task_->set_prev(crypt_task_);
  crypt_task_->set_prev(read_task_);

  // 任务加入线程池
  XThreadPool::Get()->AddTask(read_task_);
  XThreadPool::Get()->AddTask(crypt_task_);
  XThreadPool::Get()->AddTask(write_task_);

  return true;
}

/**
 * @brief 等待加解密完成
 */
void FileCrypt::Wait() {
  if (nullptr != read_task_) {
    read_bytes_ = read_task_->get_return();
  }
  if (nullptr != crypt_task_) {
    crypt_bytes_ = crypt_task_->get_return();
  }
  if (nullptr != write_task_) {
    write_bytes_ = write_task_->get_return();
  }
}
