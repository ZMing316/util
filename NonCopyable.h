#pragma once

namespace zm
{
class NonCopyAble
{
protected:
  constexpr NonCopyAble() = default;
  ~NonCopyAble() = default;

  NonCopyAble(const NonCopyAble&) = delete;
  NonCopyAble& operator=(const NonCopyAble&) = delete;
  NonCopyAble(NonCopyAble&&) = delete;
  NonCopyAble& operator=(NonCopyAble&&) = delete;
};
}
