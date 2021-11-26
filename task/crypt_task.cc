//
// Created by Homin Su on 2021/10/2.
//

#include "crypt_task.h"

#include <iostream>
#include <thread>

#include "../memory/data.h"
#include "../crypt/aes_crypt.h"

/**
 * @brief 初始化加密任务
 * @param _password 密码
 */
void CryptTask::Init(const std::string &_password) {
  crypt_ = std::make_shared<AesCrypt>();
  crypt_->Init(_password, is_encrypt_);
}

/**
 * @brief 线程主函数
 */
void CryptTask::Main() {
#ifdef Debug
  std::cout << "CryptTask::Main() Start, thread id: " << std::this_thread::get_id() << std::endl;
#endif
  size_t crypt_bytes = 0;

  // 设置上游节点的下游状态
  prev_->next_status_ = true;
  // 设置下游节点的上游状态
  next_->prev_status_ = true;

  while (is_running) {
    // 当下游节点的数据块总和超过 10MB 时阻塞
    if (next_status_) {
      std::unique_lock<std::shared_mutex> lock(mutex_);
      cv_.wait(mutex_, [this]() -> bool { return next_->DataListSize() <= 1024 * 10; });
    }

    // 当数据块总和小于 1MB 时通知上游，解除阻塞，继续读文件
    if (prev_status_ && DataListSize() <= 1024) {
      prev_->cv_.notify_one();
    }

    auto data = PopFront();
    if (nullptr == data) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    auto out = Data::Make(memory_resource_);
    size_t out_size = data->size() + AesCrypt::GetMaxPaddingSize(data->size());
    out->New(out_size);

    size_t crypt_data_size;
    if (is_encrypt_) {
      crypt_data_size =
          crypt_->Encrypt(static_cast<char *>(data->data()),
                          data->size(),
                          static_cast<char *>(out->data()),
                          data->end());
    } else {
      crypt_data_size =
          crypt_->Decrypt(static_cast<char *>(data->data()),
                          data->size(),
                          static_cast<char *>(out->data()),
                          data->end());
    }

    crypt_bytes += crypt_data_size;
    out->set_size(crypt_data_size);
    out->set_end(data->end());  // 设置状态

#ifdef Debug
//    std::cout << "<" << out->size() << ">" << std::flush;
#endif

    // 如果有下一个责任链，就 push 数据进去
    if (nullptr != next_) {
      next_->PushBack(out);
    }

    // 到结尾了就退出
    if (data->end()) {
      break;
    }
  }

#ifdef Debug
  std::cout << std::endl << "XCryptTask::Main() End, thread id: " << std::this_thread::get_id() << std::endl;
#endif

  // 设置下游节点的上游状态
  next_->prev_status_ = false;

  set_return(crypt_bytes);
}

/**
 * @brief 设置加密或解密
 * @param _is_encrypt 加密或解密
 */
void CryptTask::set_is_encrypt(bool _is_encrypt) {
  is_encrypt_ = _is_encrypt;
}
