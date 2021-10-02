//
// Created by Homin Su on 2021/10/2.
//

#ifdef Debug
#include <iostream>
#elif Release
#include <stdexcept>
#endif
#include <cstring>

#include "des_crypt.h"

bool DesCrypt::Init(const std::string &_password) {
  const_DES_cblock key = {0}; // 少的补零

  size_t key_size = _password.size();
  if (key_size > sizeof(key)) {
    key_size = sizeof(key); // 多出的丢弃
  }

  memcpy(key, _password.c_str(), key_size);
  DES_set_key(&key, &key_schedule_);  // 设置密钥

  return true;
}

size_t DesCrypt::Encrypt(const char *_in_data, size_t _in_size, char *_out_data, bool _is_end) {
  if (nullptr == _in_data || nullptr == _out_data || _in_size <= 0) {
    return 0;
  }
  size_t write_size = 0;

  const_DES_cblock in = {0};  // 输入
  DES_cblock out = {0}; // 输出

  const size_t block_size = sizeof(const_DES_cblock); // 加密数据块大小
  int padding_num = static_cast<int>(block_size - _in_size % block_size); // 填充数量，同时也是填充的内容，如果是 8 就填充 8
  int padding_offset = static_cast<int>(_in_size % block_size); // 填充位置

  size_t data_size;

  for (size_t i = 0; i < _in_size; i += block_size) {

    // 处理结尾处字节数小于 8 的情况
    if (_in_size - i < block_size) {
      data_size = _in_size - i;
    } else {
      data_size = block_size;
    }

    // 输入数据拷贝到 in 中
    memcpy(in, _in_data + write_size, data_size);

    // PKCS7 Padding
    // 缺多少个字节就填充相对应的大小，如果没缺，也要填充 8 个 8
    if (_is_end && i + block_size >= _in_size) {  // 处理最后一块数据
      // ????????88888888
      if (padding_num == block_size) {
        // 加密原来的数据
        DES_ecb_encrypt(&in, &out, &key_schedule_, DES_ENCRYPT);
        memcpy(_out_data + write_size, &out, block_size);
        write_size += block_size;

        memset(in, padding_num, sizeof(in));  // 填充 8
      } else {  // ??22    ?????55555
        memset(in + padding_offset, padding_num, padding_num);
#ifdef Debug
        std::cout << "padding_offset = " << padding_offset << ", padding_num = " << padding_num << std::endl;
#endif
      }
    }

    // 加密
    DES_ecb_encrypt(&in, &out, &key_schedule_, DES_ENCRYPT);

    // 加密好的数据拷贝到 _out_data 中
    memcpy(_out_data + write_size, &out, block_size);

    write_size += block_size;
  }

  return write_size;
}

size_t DesCrypt::Decrypt(const char *_in_data, size_t _in_size, char *_out_data, bool _is_end) {
  if (nullptr == _out_data || nullptr == _in_data || _in_size <= 0) {
    return 0;
  }
  size_t write_size = 0;

  const_DES_cblock in{0};  // 输入
  DES_cblock out{0}; // 输出
  const size_t block_size = sizeof(const_DES_cblock);  // 加密数据块大小
  size_t data_size;

  for (size_t i = 0; i < _in_size; i += block_size) {
    // 输入数据拷贝到 in 中
    memcpy(in, _in_data + write_size, block_size);

    // 解密
    DES_ecb_encrypt(&in, &out, &key_schedule_, DES_DECRYPT);
    data_size = block_size;

    // 处理结尾填充: ??22    ?????55555
    if (_is_end && _in_size - i <= block_size) {
      data_size = block_size - out[7];

      // ????????88888888
      if (0 == data_size) { break; }
      else if (data_size < 0) {
#ifdef Debug
        std::cerr << "Decrypt failed! padding size error" << std::endl;
        break;
#elif Release
        throw std::runtime_error("Decrypt failed! padding size error");
#endif
      }
    }

    memcpy(_out_data + write_size, &out, data_size);
    write_size += data_size;
  }
  return write_size;
}

size_t DesCrypt::GetMaxPaddingSize(size_t _data_size) {
  const size_t block_size = sizeof(const_DES_cblock); // 加密数据块大小
  size_t padding_num = block_size - _data_size % block_size;  // 填充数量，同时也是填充的内容，如果是 8 就填充 8
  if (0 == padding_num) {
    padding_num = sizeof(const_DES_cblock);
  }
  return padding_num;
}
