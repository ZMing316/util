#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>

#include "NonCopyable.h"

namespace zm
{
class CountDownLatch : NonCopyAble
{
public:
  explicit
    CountDownLatch(int count)
  {
    assert(count > 0);
    count_ = count;
  }

  void
    countDown()
  {
    // Even if the shared variable is atomic, it must be modified under the mutex in order to correctly publish the modification to the waiting thread.
    std::lock_guard<std::mutex> lock(lock_);
    if (--count_ == 0)
    {
      condition_.notify_all();
    }
  }

  void
    wait()
  {
    std::unique_lock<std::mutex> lock(lock_);
    condition_.wait(lock,
                    [this]()
                    {
                      return this->getCount() == 0;
                    });
    assert(count_ == 0);
  }

  int
    getCount() const
  {
    return count_;
  }

private:

  std::mutex lock_;
  int count_{};
  std::condition_variable condition_;
};
}
