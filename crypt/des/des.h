//
// Created by Homin Su on 2021/10/20.
//

#ifndef XFILECRYPT_CRYPT_DES_H_
#define XFILECRYPT_CRYPT_DES_H_

#include <bitset>

namespace des {
constexpr bool kEncrypt = true;
constexpr bool kDecrypt = false;

namespace common {
/**
 * @brief 将一个字节转换为一个 8 位 bitset
 * @param c 无符号字符
 * @return std::shared_ptr<std::bitset<8>>
 */
std::bitset<64> Bytes2Bits(const char *c);

/**
 * @brief 将一个 28 位的子密钥左移
 * @param _k 子密钥
 * @param _shift_num 左移位数
 * @return 左移后的密钥
 */
std::bitset<28> KeyLeftShift(const std::bitset<28> &_k, const unsigned char &_shift_num);

/**
 * @brief 二进制转十进制
 * @tparam Te 经过 e 表扩展的数据数组
 * @tparam Args 要转化的数据索引
 * @param _e 经过 e 表扩展的数据
 * @param args 要转化的数据索引, 由高到地
 * @return 十进制
 */
template<typename Te, typename ... Args>
inline unsigned char ExpendBin2Dec(Te &&_e, Args ... args) {
  unsigned char BCD_8421 = 0;
  return (0 + ... + (_e[args] * (BCD_8421 > 1 ? BCD_8421 *= 2 : ++BCD_8421)));
}

/**
 * @brief 轮函数
 * @param _r 上一轮右 32 位
 * @param _k 48 位子密钥
 * @return 加密后的 32 位数据
 */
std::bitset<32> RoundFunc(const std::bitset<32> &_r, const std::bitset<48> &_k);
} // namespace common

/**
 * @brief DES 加密算法
 */
class Des {
 private:
  std::bitset<64> key_{}; ///< 密钥
  std::bitset<48> sub_keys_[16];  ///< 16 轮加解密的子密钥

 public:
  /**
   * @brief 初始化密钥
   * @details 密钥 8 位，多余丢弃，不足补 0
   * @param _password 8 位密钥
   */
  void Init(const std::string &_password);

  /**
   * @brief 加密, 单次加密 8 个字节
   * @param _in 输入数据
   * @param _out 输出数据
   */
  void Encrypt(const void *_in, void *_out);

  /**
   * @brief 解密, 单次解密 8 个字节
   * @param _in 输入数据
   * @param _out 输出数据
   */
  void Decrypt(const void *_in, void *_out);

 protected:
  /**
    * @brief 加解密
    * @param _text 明文或密文, 64 位 bitset
    * @param _is_encrypt 加密还是解密
    * @return 密文或明文
    */
  std::bitset<64> Crypt(const std::bitset<64> &_text, bool _is_encrypt);

 private:
  /**
   * @brief 生成 16 个 48 位的子密钥
   */
  void GenSubKey();
};
} // namespace des

#endif //XFILECRYPT_CRYPT_DES_H_
