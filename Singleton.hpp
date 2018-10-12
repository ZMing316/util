#pragma once

#include <memory>
#include <mutex>
#include <cassert>

#include "NonCopyable.h"

namespace zm
{
template <typename T>
class Singleton : NonCopyAble
{
public:

  template <typename ...Args>
  static T&
    instance(Args&& ... args)
  {
    std::call_once(once_,
                   [&]()
                   {
                     init(std::forward<Args>(args)...);
                   });
    assert(value_ != nullptr);
    return *value_;
  }

protected:
  Singleton() = default;
  ~Singleton() = default;

private:
  template <typename ...Args>
  static void
    init(Args&& ... args)
  {
    value_.reset(new T(std::forward<Args>(args)...));
  }

private:
  static std::unique_ptr<T> value_;
  static std::once_flag once_;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::value_{nullptr};

template <typename T>
std::once_flag Singleton<T>::once_;
} // zm
