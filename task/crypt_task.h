//
// Created by Homin Su on 2021/10/2.
//

#ifndef XFILECRYPT_TASK_CRYPT_CRYPT_TASK_H_
#define XFILECRYPT_TASK_CRYPT_CRYPT_TASK_H_

#include "task_base.h"

class AesCBCCrypt;

/**
 * @brief 加解密任务
 */
class CryptTask : public TaskBase {
 private:
  std::shared_ptr<AesCBCCrypt> crypt_; ///< 加解密对象指针
  bool is_encrypt_ = true;  ///< 加密解密标识

 public:
  /**
   * @brief 初始化加密任务
   * @param _password 密码
   */
  void Init(const std::string &_password);

  /**
   * @brief 设置加密或解密
   * @param _is_encrypt 加密或解密
   */
  void set_is_encrypt(bool _is_encrypt);

 private:
  /**
   * @brief 线程主函数
   */
  void Main() override;
};

#endif //XFILECRYPT_TASK_CRYPT_CRYPT_TASK_H_
