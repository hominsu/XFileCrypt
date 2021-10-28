//
// Created by Homin Su on 2021/10/20.
//

#ifndef XFILECRYPT_CRYPT_MY_DES_H_
#define XFILECRYPT_CRYPT_MY_DES_H_

#include <bitset>

#include "defs.h"

class MyDes {
 private:
  std::bitset<64> key_{}; ///< 存储密钥

 public:
  /**
   * @brief 初始化密钥
   * @details DES 加密算法，密钥 8 位，多余丢弃，不足补 0
   * @param _password 8 位密钥
   * @return bool
   */
  bool Init(const std::string &_password);

  /**
   * @brief 将一个字节转换为一个 8 位 bitset
   * @param c 无符号字符
   * @return std::shared_ptr<std::bitset<8>>
   */
  static std::bitset<64> Bytes2Bit(const unsigned char *c);

 private:
  /**
   * @brief 轮函数
   * @param _r 上一轮右 32 位
   * @param _k 48 位子密钥
   * @return 加密后的 32 位数据
   */
  std::bitset<32> RoundFunc(const std::bitset<32> &_r, const std::bitset<48> &_k);

  /**
   * @brief 将一个 28 位的子密钥左移
   * @param _k 子密钥
   * @param _shift_num 左移位数
   * @return 左移后的密钥
   */
  static std::bitset<28> KeyLeftShift(const std::bitset<28> &_k, unsigned char &_shift_num);

 private:
  /**
   * @brief 强制转换为无符号字符型
   * @tparam Tn 类型
   * @param _n 数据 0 ～ 255
   * @return 强制转换后的结果
   */
  template<typename Tn>
  inline unsigned char ToUnsignedChar(Tn &&_n) {
    return static_cast<unsigned char>(std::forward<Tn>(_n));
  }

  /**
   * @brief 2 位 2 进制转 10 进制
   * @tparam Te 经过 e 表扩展的数据
   * @tparam Tb bitset 对象指针
   * @param _e 经过 e 表扩展的数据
   * @param _b 第二位
  * @param _a 第一位
  * @return 十进制
   */
  template<typename Te, typename Tb>
  inline unsigned char ExpendBin2Dec(Te &&_e, Tb &&_b, Tb &&_a) {
    return _e[_b] * 2 + _e[_a] * 1;
  };

  /**
   * @brief 4 位 2 进制转 10 进制
   * @tparam Te 经过 e 表扩展的数据
   * @tparam Tb bitset 对象指针
   * @param _e 经过 e 表扩展的数据
   * @param _d 第四位
   * @param _c 第三位
   * @param _b 第二位
   * @param _a 第一位
   * @return 十进制
   */
  template<typename Te, typename Tb>
  inline unsigned char ExpendBin2Dec(Te &&_e, Tb &&_d, Tb &&_c, Tb &&_b, Tb &&_a) {
    return _e[_d] * 8 + _e[_c] * 4 + _e[_b] * 2 + _e[_a] * 1;
  };
};

#endif //XFILECRYPT_CRYPT_MY_DES_H_
