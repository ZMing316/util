#pragma once


#include <atomic>
#include <deque>
#include <mutex>
#include <condition_variable>

#include "NonCopyable.h"

namespace zm
{
template <typename T, class Allocator = std::allocator<T>>
class ThreadSafeQueue : public NonCopyable
{
public:
    using container_type = std::deque<T, Allocator>;
    typedef typename container_type::value_type      value_type;
    typedef typename container_type::reference       reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::size_type       size_type;


    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    value_type waitDequeue() noexcept(false){
       std::unique_lock lock(mutex_);
       con_.wait_for(lock, [&](){return !container_.empty() || hasWeakUp_;});
       if (hasWeakUp_){
           throw std::runtime_error("wait has be wakeup");
       }
       value_type value = container_.front();
       container_.pop_front();
       return value; // ROV
    }

    size_type size() const{
        return container_.size();
    }

    bool empty() const{
        return container_.empty();
    }

    void enqueue(T&& value){
        std::lock_guard<std::mutex> lock(mutex_);
        bool is_empty = empty();
        container_.push_back(std::forward<T>(value));
        if (is_empty)
            con_.notify_one();
    }

    void enqueuUrgent(T&& value){
        std::lock_guard<std::mutex> lock(mutex_);
        bool is_empty = empty();
        container_.push_front(std::forward<T>(value));
        if (is_empty)
            con_.notify_one();
    }

    void weakUp(){
        hasWeakUp_ = true;
    }
private:
    container_type container_;
    std::mutex mutex_;
    std::condition_variable con_;
    volatile bool hasWeakUp_{false};
};
}