//
// Created by Homin Su on 2021/10/20.
//

#include <memory>
#include <cstring>

#include "defs.h"

#include "my_des.h"

/**
 * @brief 初始化密钥
 * @details DES 加密算法，密钥 8 位，多余丢弃，不足补 0
 * @param _password 8 位密钥
 * @return bool
 */
bool MyDes::Init(const std::string &_password) {
  unsigned char key[8]{0};
  memcpy(key, _password.c_str(), _password.size()); // 密码拷贝到 char 数组中，少的为 0

  key_ = Bytes2Bits(key);

  return false;
}

/**
 * @brief 将 8 个字节转换为一个 64 位 bitset
 * @param c 无符号字符数组
 * @return std::shared_ptr<std::bitset<8>>
 */
inline std::bitset<64> MyDes::Bytes2Bits(const unsigned char *c) {
  auto bit_set = std::bitset<64>(0x0);
  for (int i = 0; i < 8; ++i) {
    for (int index = 0, offset = 7; index < 8; ++index, --offset) {
      (bit_set)[8 * i + index] = ((c[i] >> offset) & 0x1);
    }
  }
  return bit_set;
}

/**
 * @brief 轮函数
 * @param _r 上一轮右 32 位
 * @param _k 48 位子密钥
 * @return 加密后的 32 位数据
 */
std::bitset<32> MyDes::RoundFunc(const std::bitset<32> &_r, const std::bitset<48> &_k) {
  // 存储经过 e 表扩展的数据
  auto expend_e = std::bitset<48>(0x0);

  // 明文经过扩展置换到 48 位
  for (unsigned char index = 0; index < 48; ++index) {
    expend_e[index] = _r[DesDefine::kE[index] - 1];
  }

  // 扩展的明文与密钥进行异或
  expend_e = expend_e ^ _k;

  // 暂存 S Box 的输出
  auto tmp = std::bitset<32>(0x0);

  // S Box
  for (unsigned char index = 0, loc = 0; index < 48; index += 6, loc += 4) {
    // 将经过 e 表扩展的数据通过 S Box 进行压缩置换
    auto result = std::bitset<4>(
        DesDefine::kSBox[index / 6][ExpendBin2Dec(expend_e, index + 0, index + 1)][ExpendBin2Dec(expend_e,
                                                                                                 index + 2,
                                                                                                 index + 3,
                                                                                                 index + 4,
                                                                                                 index + 5)]);

    tmp[loc] = result[0];
    tmp[loc + 1] = result[0];
    tmp[loc + 2] = result[0];
    tmp[loc + 3] = result[0];
  }

  // 输出
  auto output = std::bitset<32>(0x0);

  // P 置换
  for (unsigned char index = 0; index < 32; ++index) {
    output[index] = tmp[DesDefine::kP[index] - 1];
  }

  return output;
}

/**
 * @brief 将一个 28 位的子密钥左移
 * @param _k 子密钥
 * @param _shift_num 左移位数
 * @return 左移后的密钥
 */
inline std::bitset<28> MyDes::KeyLeftShift(const std::bitset<28> &_k, const unsigned char &_shift_num) {
  auto tmp = std::bitset<28>(_k);

  for (unsigned char index = 0; index < 28 - _shift_num; ++index) {
    tmp[index + _shift_num] = _k[index];
  }

  for (unsigned char index = 0; index < _shift_num; ++index) {
    tmp[index] = _k[28 - index];
  }

  return tmp;
}

/**
 * @brief 生成 16 个 48 位的子密钥
 */
void MyDes::GenSubKey() {
  auto key_56 = std::bitset<56>(0x0);
  auto left_key = std::bitset<28>(0x0);
  auto right_key = std::bitset<28>(0x0);
  auto key_48 = std::bitset<48>(0x0);

  // 压缩置换 1, 64 位压缩到 56 位
  for (unsigned char index = 0; index < 56; ++index) {
    key_56[index] = key_[DesDefine::kPC_1[index] - 1];
  }

  // 56 位密钥分为左右两边两个 28 位的密钥
  for (unsigned char index = 0; index < 28; ++index) {
    left_key[index] = key_56[index];
    right_key[index] = key_56[index + 28];
  }

  // 16 轮
  for (unsigned char kShiftBit: DesDefine::kShiftBits) {
    // 循环左移
    left_key = KeyLeftShift(left_key, kShiftBit);
    right_key = KeyLeftShift(right_key, kShiftBit);

    // 拼接
    for (unsigned char index = 0; index < 28; ++index) {
      key_56[index] = left_key[index];
      key_56[index + 28] = right_key[index];
    }

    // 压缩置换 2, 56 位压缩到 48 位
    for (unsigned char index = 0; index < 48; ++index) {
      key_48[index] = key_56[DesDefine::kPC_2[index - 1]];
    }

    sub_key_.push_back(key_48);
  }
}

/**
 * @brief 加密
 * @param _text 明文, 64 位 bitset
 * @return 密文, 64 位 bitset
 */
std::bitset<64> MyDes::Crypt(const std::bitset<64> &_text, bool _is_encrypt = DES_ENCRYPT) {
  auto text = std::bitset<64>(0x0);
  auto result = std::bitset<64>(0x0);
  auto left = std::bitset<32>(0x0);
  auto right = std::bitset<32>(0x0);

  // 初始 IP 置换
  for (unsigned char index = 0; index < 64; ++index) {
    text[index] = _text[DesDefine::kIP[index] - 1];
  }

  // 分为左半部分和右半部分
  for (unsigned char index = 0; index < 32; ++index) {
    left[index] = text[index];
    right[index] = text[index + 32];
  }

  // 16 次轮函数
  if (_is_encrypt) {
    // 加密
    for (unsigned char index = 0; index < 16; ++index) {
      auto tmp = right;
      right = left ^ RoundFunc(right, sub_key_[index]);
      left = tmp;
    }
  } else {
    // 解密, 密钥逆用
    for (unsigned char index = 0; index < 16; ++index) {
      auto tmp = right;
      right = left ^ RoundFunc(right, sub_key_[15 - index]);
      left = tmp;
    }
  }

  // 合并左半部分和右半部分
  for (unsigned char index = 0; index < 32; ++index) {
    text[index] = left[index];
    text[index + 32] = right[index];
  }

  // IP 逆置换
  for (unsigned char index = 0; index < 64; ++index) {
    result[index] = text[DesDefine::kIP_1[index] - 1];
  }

  return result;
}

/**
 * @brief 加密, 单次加密 8 个字节
 * @param _in 输入数据
 * @param _out 输出数据
 */
void MyDes::Encrypt(const void *_in, void *_out) {
  unsigned char src[8]{0};
  memcpy(static_cast<void *>(src), _in, 8);

  auto plain_text = Bytes2Bits(src);
  auto result = Crypt(plain_text, DES_ENCRYPT);

  memcpy(static_cast<void *>(&result), _out, 8);
}

/**
 * @brief 解密, 单次解密 8 个字节
 * @param _in 输入数据
 * @param _out 输出数据
 */
void MyDes::Decrypt(const void *_in, void *_out) {
  unsigned char src[8]{0};
  memcpy(static_cast<void *>(src), _in, 8);

  auto plain_text = Bytes2Bits(src);
  auto result = Crypt(plain_text, DES_DECRYPT);

  memcpy(static_cast<void *>(&result), _out, 8);
}

