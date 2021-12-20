//
// Created by Homin Su on 2021/11/17.
//

#ifndef XFILECRYPT_CRYPT_AES_CBC_CRYPT_H_
#define XFILECRYPT_CRYPT_AES_CBC_CRYPT_H_

#include <string>

#include <openssl/aes.h>

/**
 * @brief Aes 加密
 */
class AesCBCCrypt {
 private:
  AES_KEY key_schedule_; ///< 存储密钥

 public:
  /**
   * @brief 初始化密钥
   * @details DES 加密算法，密钥 8 位，多余丢弃，不足补 0
   * @param _password 8 位密钥
   * @param _is_encrypt 加密还是解密
   * @return bool
   */
  bool Init(const std::string &_password, bool _is_encrypt);

  /**
   * @brief 加密数据，加密到结尾填充数据
   * @detail 加密到结尾会使用 PKCS7 Padding 数据填充算法进行填充
   * @param _in_data 输入数据
   * @param _in_size 输入数据大小
   * @param _out_data 输出数据
   * @param _is_end 是否加密到结尾
   * @return 返回加密后的数据，有可能大于输入（结尾处）
   */
  size_t Encrypt(const char *_in_data, size_t _in_size, char *_out_data, bool _is_end = false);

  /**
   * @brief 解密数据，解密到结尾会删除填充数据
   * @param _in_data 输入数据
   * @param _in_size 输入数据大小
   * @param _out_data 输出数据
   * @param _is_end 是否加密到结尾
   * @return 返回解密后的数据，有可能小雨输入（结尾处）
   */
  size_t Decrypt(const char *_in_data, size_t _in_size, char *_out_data, bool _is_end = false);

  /**
   * @brief 获取需要填充的字节数
   * @param _data_size
   * @return 需要填充的字节数
   */
  static size_t GetMaxPaddingSize(size_t _data_size);
};

#endif //XFILECRYPT_CRYPT_AES_CBC_CRYPT_H_
