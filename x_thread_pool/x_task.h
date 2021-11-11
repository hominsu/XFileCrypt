//
// Created by Homin Su on 2021/10/2.
//

#ifndef XFILECRYPT_X_THREAD_POOL_X_TASK_H_
#define XFILECRYPT_X_THREAD_POOL_X_TASK_H_

#include <functional>
#include <future>

/**
 * @brief 线程池任务基类
 */
class XTaskBase {
 public:
  std::function<bool()> is_running = nullptr; ///< 线程池运行状态函数指针

 public:
  virtual void Main() = 0;
};

/**
 * @brief 线程池任务模版类，ret_type 设定值类型
 * @tparam ret_type 值类型，不允许为 std::thread
 */
template<class ret_type,
    class = typename std::enable_if
        <!std::is_same<ret_type, std::thread>::value>::type
>
class XTask : public XTaskBase {
 private:
  std::promise<ret_type> p_; ///< 接收返回值

 public:
  /**
   * @brief 设置 future 的值
   * @param ret_type int value
   */
  void set_return(ret_type &&_value) {
    p_.set_value(std::forward<ret_type>(_value));
  }

  void set_return(const ret_type &_value) {
    p_.set_value(_value);
  }

  /**
   * @brief 阻塞等待 set_value
   * @return decltype(auto)
   */
  decltype(auto) get_return() {
    return p_.get_future().get();
  }
};

#endif //XFILECRYPT_X_THREAD_POOL_X_TASK_H_
