//
// Created by Homin Su on 2021/10/2.
//

#ifndef XFILECRYPT_DATA_DATA_H_
#define XFILECRYPT_DATA_DATA_H_

#include <memory>
#include <memory_resource>

/**
 * @brief 内存池数据块
 */
class Data {
 private:
  void *data_ = nullptr;
  bool end_ = false;        ///< 是否是文件结尾
  size_t size_ = 0;         ///< 数据字节数
  size_t memory_size_ = 0;  ///< 申请内存空间字节数
  std::shared_ptr<std::pmr::memory_resource> memory_resource_;  ///< 内存池

 private:
  Data();

 public:
  ~Data();

  /**
   * @brief 创建 Data 对象
   * @param _memory_resource
   * @return Data 的智能指针对象
   */
  static std::shared_ptr<Data> Make(std::shared_ptr<std::pmr::memory_resource> _memory_resource);

  /**
   * @brief 创建内存空间
   * @param _memory_size 占用内存字节数
   * @return 创建的内存空间的指针，创建失败为空 nullptr
   */
  void *New(size_t _memory_size);

  /**
   * @brief 获取数据块的指针
   * @return 数据块的指针
   */
  [[nodiscard]] void *data() const;

  /**
   * @brief 获取实际数据的字节数
   * @return 实际数据的字节数
   */
  [[nodiscard]] size_t size() const;

  /**
   * @brief 设置实际数据字节数
   * @param size 实际数据字节数
   */
  void set_size(size_t _size);

  /**
   * @brief 获取分配的内存大小
   * @return size_t 分配的内存大小
   */
  [[nodiscard]] size_t memory_size() const;

  /**
   * @brief 是否是文件结尾
   * @return
   */
  [[nodiscard]] bool end() const;

  /**
   * @brief 设置为文件结尾
   * @param _end true or false
   */
  void set_end(bool _end);
};

/**
 * @brief 定义了 Byte、KB、MB、GB 的大小
 */
enum class Unit : size_t {
  Byte = 1, KB = 1024 * Byte, MB = 1024 * KB, GB = 1024 * MB
};

/**
 * @brief 将字节的大小换算成对应单位的大小
 * @tparam size_type 输入类型，只能为算术类型
 * @param _size 字节大小
 * @param _unit 转换的单位
 * @return 转换后的数值
 */
template<typename size_type,
    class = typename std::enable_if<
        std::is_arithmetic<size_type>::value>::type
>
double UnitConvert(size_type _size, Unit _unit) {
  return _size / static_cast<double>(_unit);
}

constexpr size_t KB(size_t _size) {
  return static_cast<size_t>(Unit::KB) * _size;
}

constexpr size_t MB(size_t _size) {
  return static_cast<size_t>(Unit::MB) * _size;
}

constexpr size_t GB(size_t _size) {
  return static_cast<size_t>(Unit::GB) * _size;
}

constexpr size_t LimitNum(size_t _a, size_t _b) {
  return _a / _b;
}

#endif //XFILECRYPT_DATA_DATA_H_
