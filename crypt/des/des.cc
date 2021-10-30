//
// Created by Homin Su on 2021/10/20.
//

#include <cstring>

#include "des.h"

// 匿名命名空间, 保存各种 DES 要用到的常量
namespace {
///< 初始置换 IP
constexpr unsigned char kIP[] = {58, 50, 42, 34, 26, 18, 10, 2,
                                 60, 52, 44, 36, 28, 20, 12, 4,
                                 62, 54, 46, 38, 30, 22, 14, 6,
                                 64, 56, 48, 40, 32, 24, 16, 8,
                                 57, 49, 41, 33, 25, 17, 9, 1,
                                 59, 51, 43, 35, 27, 19, 11, 3,
                                 61, 53, 45, 37, 29, 21, 13, 5,
                                 63, 55, 47, 39, 31, 23, 15, 7};

///< IP 逆置换表
constexpr unsigned char kIP_1[] = {40, 8, 48, 16, 56, 24, 64, 32,
                                   39, 7, 47, 15, 55, 23, 63, 31,
                                   38, 6, 46, 14, 54, 22, 62, 30,
                                   37, 5, 45, 13, 53, 21, 61, 29,
                                   36, 4, 44, 12, 52, 20, 60, 28,
                                   35, 3, 43, 11, 51, 19, 59, 27,
                                   34, 2, 42, 10, 50, 18, 58, 26,
                                   33, 1, 41, 9, 49, 17, 57, 25};

///< 压缩置换 1, 将 64 位密钥压缩为 56 位
constexpr unsigned char kPC_1[] = {57, 49, 41, 33, 25, 17, 9,
                                   1, 58, 50, 42, 34, 26, 18,
                                   10, 2, 59, 51, 43, 35, 27,
                                   19, 11, 3, 60, 52, 44, 36,
                                   63, 55, 47, 39, 31, 23, 15,
                                   7, 62, 54, 46, 38, 30, 22,
                                   14, 6, 61, 53, 45, 37, 29,
                                   21, 13, 5, 28, 20, 12, 4};

///< 压缩置换 2, 将 56 位密钥压缩为 48 位
constexpr unsigned char kPC_2[] = {14, 17, 11, 24, 1, 5,
                                   3, 28, 15, 6, 21, 10,
                                   23, 19, 12, 4, 26, 8,
                                   16, 7, 27, 20, 13, 2,
                                   41, 52, 31, 37, 47, 55,
                                   30, 40, 51, 45, 33, 48,
                                   44, 49, 39, 56, 34, 53,
                                   46, 42, 50, 36, 29, 32};

///< 密钥每轮左移的位数
constexpr unsigned char kShiftBits[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// 轮函数相关设定

///< 扩展置换表，将右 32位数据扩展至 48位
constexpr unsigned char kE[] = {32, 1, 2, 3, 4, 5,
                                4, 5, 6, 7, 8, 9,
                                8, 9, 10, 11, 12, 13,
                                12, 13, 14, 15, 16, 17,
                                16, 17, 18, 19, 20, 21,
                                20, 21, 22, 23, 24, 25,
                                24, 25, 26, 27, 28, 29,
                                28, 29, 30, 31, 32, 1};

///< SBox 8 张 6 到 4 位的压缩置换表
constexpr unsigned char kSBox[8][4][16] = {
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};

///< P置换，32 位到32 位的置换表
constexpr unsigned char kP[] = {16, 7, 20, 21,
                                29, 12, 28, 17,
                                1, 15, 23, 26,
                                5, 18, 31, 10,
                                2, 8, 24, 14,
                                32, 27, 3, 9,
                                19, 13, 30, 6,
                                22, 11, 4, 25};
} // namespace

namespace des {
namespace common {
/**
 * @brief 将 8 个字节转换为一个 64 位 bitset
 * @param c 字符数组指针
 * @return
 */
std::bitset<64> Bytes2Bits(const char *c) {
  auto bit_set = std::bitset<64>(0x0);
  for (unsigned char i = 0; i < 8; ++i) {
    for (unsigned char j = 0; j < 8; ++j) {
      (bit_set)[8 * i + j] = ((c[i] >> j) & 0x1);
    }
  }
  return bit_set;
}

/**
 * @brief 将一个 28 位的子密钥左移
 * @param _k 子密钥
 * @param _shift_num 左移位数
 * @return 左移后的密钥
 */
std::bitset<28> KeyLeftShift(const std::bitset<28> &_k, const unsigned char &_shift_num) {
  auto tmp = std::bitset<28>(_k);

  for (unsigned char i = 0; i < 28 - _shift_num; ++i) {
    tmp[i + _shift_num] = _k[i];
  }

  for (unsigned char i = 0, offset = 28 - _shift_num; i < _shift_num; ++i, ++offset) {
    tmp[i] = _k[offset];
  }

  return tmp;
}

/**
 * @brief 轮函数
 * @param _r 上一轮右 32 位
 * @param _k 48 位子密钥
 * @return 加密后的 32 位数据
 */
std::bitset<32> RoundFunc(const std::bitset<32> &_r, const std::bitset<48> &_k) {
  auto expend_e = std::bitset<48>(0x0); // 存储经过 e 表扩展的数据

  // 明文经过扩展置换到 48 位
  for (unsigned char i = 0; i < 48; ++i) {
    expend_e[i] = _r[kE[i] - 1];
  }

  // 扩展的明文与密钥进行异或
  expend_e = expend_e ^ _k;

  auto tmp = std::bitset<32>(0x0);  // 暂存 S Box 的输出

  // S Box
  for (unsigned char i = 0, loc = 0; i < 48; i += 6, loc += 4) {
    // 将经过 e 表扩展的数据通过 S Box 进行压缩置换
    auto result = std::bitset<4>(
        kSBox[i / 6][common::ExpendBin2Dec(expend_e, i + 1, i + 0)][common::ExpendBin2Dec(expend_e,
                                                                                          i + 5,
                                                                                          i + 4,
                                                                                          i + 3,
                                                                                          i + 2)]);
    tmp[loc] = result[0];
    tmp[loc + 1] = result[1];
    tmp[loc + 2] = result[2];
    tmp[loc + 3] = result[3];
  }

  auto output = std::bitset<32>(0x0);

  // P 置换
  for (unsigned char i = 0; i < 32; ++i) {
    output[i] = tmp[kP[i] - 1];
  }

  return output;
}
} // namespace common

/**
 * @brief 初始化密钥
 * @details DES 加密算法，密钥 8 位，多余丢弃，不足补 0
 * @param _password 8 位密钥
 * @return bool
 */
void Des::Init(const std::string &_password) {
  char key[8]{0};
  memcpy(key, _password.c_str(), _password.size()); // 密码拷贝到 char 数组中，少的为 0

  key_ = common::Bytes2Bits(key);

  GenSubKey();
}

/**
 * @brief 生成 16 个 48 位的子密钥
 */
void Des::GenSubKey() {
  auto key_56 = std::bitset<56>(0x0);

  // 压缩置换 1, 64 位压缩到 56 位
  for (unsigned char i = 0; i < 56; ++i) {
    key_56[i] = key_[kPC_1[i] - 1];
  }

  auto left_key = std::bitset<28>(0x0);
  auto right_key = std::bitset<28>(0x0);

  // 56 位密钥分为左右两边两个 28 位的密钥
  for (unsigned char i = 0; i < 28; ++i) {
    left_key[i] = key_56[i + 28];
    right_key[i] = key_56[i];
  }

  auto key_48 = std::bitset<48>(0x0);

  // 16 轮
  for (unsigned char i = 0; i < 16; ++i) {

    // 循环左移
    left_key = common::KeyLeftShift(left_key, kShiftBits[i]);
    right_key = common::KeyLeftShift(right_key, kShiftBits[i]);

    // 拼接
    for (unsigned char j = 0; j < 28; ++j) {
      key_56[j + 28] = left_key[j];
      key_56[j] = right_key[j];
    }

    // 压缩置换 2, 56 位压缩到 48 位
    for (unsigned char j = 0; j < 48; ++j) {
      key_48[j] = key_56[kPC_2[j] - 1];
    }

    sub_keys_[i] = key_48;
  }
}

/**
 * @brief 加密
 * @param _text 明文, 64 位 bitset
 * @return 密文, 64 位 bitset
 */
std::bitset<64> Des::Crypt(const std::bitset<64> &_text, bool _is_encrypt = kEncrypt) {
  auto temp = std::bitset<64>(0x0);

  // 初始 IP 置换
  for (unsigned char i = 0; i < 64; ++i) {
    temp[i] = _text[kIP[i] - 1];
  }

  auto left = std::bitset<32>(0x0);
  auto right = std::bitset<32>(0x0);

  // 分为左半部分和右半部分
  for (unsigned char i = 0; i < 32; ++i) {
    left[i] = temp[i + 32];
    right[i] = temp[i];
  }

  // 16 次轮函数
  if (_is_encrypt) {
    // 加密
    for (auto &sub_key: sub_keys_) {
      auto tmp = right;
      right = left ^ common::RoundFunc(right, sub_key);
      left = tmp;
    }
  } else {
    // 解密, 密钥逆用
    for (unsigned char index = 0; index < 16; ++index) {
      auto tmp = right;
      right = left ^ common::RoundFunc(right, sub_keys_[15 - index]);
      left = tmp;
    }
  }

  // 交换左半部分和右半部分, 并合并
  for (unsigned char i = 0; i < 32; ++i) {
    temp[i] = left[i];
    temp[i + 32] = right[i];
  }

  auto result = std::bitset<64>(0x0);

  // IP 逆置换
  for (unsigned char i = 0; i < 64; ++i) {
    result[i] = temp[kIP_1[i] - 1];
  }

  return result;
}

/**
 * @brief 加密, 单次加密 8 个字节
 * @param _in 输入数据
 * @param _out 输出数据
 */
void Des::Encrypt(const void *_in, void *_out) {
  char src[8]{0};
  memcpy(static_cast<void *>(src), _in, 8);

  auto plain_text = common::Bytes2Bits(src);
  auto result = Crypt(plain_text, kEncrypt);

  memcpy(_out, static_cast<void *>(&result), 8);
}

/**
 * @brief 解密, 单次解密 8 个字节
 * @param _in 输入数据
 * @param _out 输出数据
 */
void Des::Decrypt(const void *_in, void *_out) {
  char src[8]{0};
  memcpy(static_cast<void *>(src), _in, 8);

  auto plain_text = common::Bytes2Bits(src);
  auto result = Crypt(plain_text, kDecrypt);

  memcpy(_out, static_cast<void *>(&result), 8);
}
} // namespace des