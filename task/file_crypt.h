//
// Created by Homin Su on 2021/10/2.
//

#ifndef XFILECRYPT_TASK_FILE_CRYPT_X_FILE_CRYPT_H_
#define XFILECRYPT_TASK_FILE_CRYPT_X_FILE_CRYPT_H_

#include <string>
#include <memory>
#include <memory_resource>

class ReadTask;
class WriteTask;
class CryptTask;

/**
 * @brief 文件加解密
 */
class FileCrypt {
 public:
  std::string in_file_;
  std::string out_file_;
  size_t read_bytes_{};
  size_t crypt_bytes_{};
  size_t write_bytes_{};

 private:
  std::shared_ptr<ReadTask> read_task_;    ///< 读取任务
  std::shared_ptr<WriteTask> write_task_;  ///< 写出任务
  std::shared_ptr<CryptTask> crypt_task_;  ///< 加密任务

 public:
  /**
   * @brief 开始文件加解密
   * @param _in_file 输入文件
   * @param _out_file 输出文件
   * @param _password 密码
   * @param _is_encrypt 加密还是解密
   * @return 是否加密成功
   */
  bool Start(const std::string &_in_file,
             const std::string &_out_file,
             const std::string &_password,
             bool _is_encrypt,
             std::shared_ptr<std::pmr::synchronized_pool_resource> &_memory_resource);

  /**
   * @brief 等待加解密完成
   */
  void Wait();
};

#endif //XFILECRYPT_TASK_FILE_CRYPT_X_FILE_CRYPT_H_
