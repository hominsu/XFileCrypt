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
  std::bitset<32> RoundFunc(std::bitset<32> _r, std::bitset<48> _k);

 private:
  /**
   * @brief 4 位 2 进制转 10 进制
   * @tparam Tb bitset 对象指针
   * @param _d 第四位
   * @param _c 第三位
   * @param _b 第二位
   * @param _a 第一位
   * @return 十进制
   */
  template<typename Tb>
  inline unsigned char Bin4ToDec(Tb &&_d, Tb &&_c, Tb &&_b, Tb &&_a) {
    return _d * 8 + _c * 4 + _b * 2 + _a * 1;
  };

  /**
   * @brief 2 位 2 进制转 10 进制
   * @tparam Tb bitset 对象指针
   * @param _b 第二位
   * @param _a 第一位
   * @return 十进制
   */
  template<typename Tb>
  inline unsigned char Bin2ToDec(Tb &&_b, Tb &&_a) {
    return _b * 2 + _a * 1;
  };
};

#endif //XFILECRYPT_CRYPT_MY_DES_H_
