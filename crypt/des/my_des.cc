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

  key_ = Bytes2Bit(key);

  return false;
}

/**
 * @brief 将 8 个字节转换为一个 64 位 bitset
 * @param c 无符号字符数组
 * @return std::shared_ptr<std::bitset<8>>
 */
inline std::bitset<64> MyDes::Bytes2Bit(const unsigned char *c) {
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
inline std::bitset<28> MyDes::KeyLeftShift(const std::bitset<28> &_k, unsigned char &_shift_num) {
  auto tmp = std::bitset<28>(_k);

  for (unsigned char index = 0; index < 28 - _shift_num; ++index) {
    tmp[index + _shift_num] = _k[index];
  }

  for (unsigned char index = 0; index < _shift_num; ++index) {
    tmp[index] = _k[28 - index];
  }

  return tmp;
}
