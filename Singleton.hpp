#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <mutex>

#include "NonCopyable.h"

namespace zm
{

// 动态构建
template <typename T>
struct CreatePolicyNew
{
  template <typename... Args>
  static T *create(Args &&... args)
  {
    return new T(std::forward<Args>(args)...);
  }

  static void destroy(T *t)
  {
    assert(t != nullptr);
    delete t;
  }
};

// 静态构建
template <typename T>
struct CreatePolicyStatic
{
  template <typename... Args>
  static T *create(Args &&... args)
  {
    static char buffer_[sizeof(T)];
    return new (buffer_) T(std::forward<Args>(args)...);
  }

  static void destroy(T* t)
  {
    t->~T();
  }
};

template <typename T, template <typename> typename CreatePolicy = CreatePolicyNew>
class Singleton
{
public:
  template <typename... Args>
  static T &
  instance(Args&&... args)
  {
    std::call_once(once_,
                   [&]() {
                     init(std::forward<Args>(args)...);
                   });
    return *value_;
  }

protected:
  Singleton() = default;
  ~Singleton() = default;

private:
  template <typename... Args>
  static void
  init(Args&&... args)
  {
    value_ = typename CreatePolicy<T>::create(std::forward<Args>(args)...);
    assert(value_ != nullptr);
    std::atexit(destory);
  }

  static void destroy()
  {
    typename CreatePolicy<T>::destroy(value_);
  }

private:
  static T* value_;
  static std::once_flag once_;
};

template <typename T, template <typename> typename CreatePolicy>
T* Singleton<T, CreatePolicy>::value_ = nullptr;

template <typename T, template <typename> typename CreatePolicy>
std::once_flag Singleton<T, CreatePolicy>::once_;
} // namespace zm