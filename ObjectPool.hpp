#pragma once

#include <atomic>
#include <algorithm>
#include <unordered_map>
#include <vector>

#include "Singleton.hpp"

namespace zm
{
template <typename T>
class ObjectPool : public Singleton<ObjectPool<T>>
{
  using HolderPtr = std::unique_ptr<T, std::function<void(T *)>>;

public:
  struct ObjectHolder
  {
    ObjectHolder(HolderPtr &&ptr);
    bool isNull() const
    {
      return object_ == nullptr;
    }
    T &operator*() const;

  private:
    HolderPtr object_;
  };

  template <typename... Args>
  ObjectHolder acquire(Args &&... args);

  explicit ObjectPool(int init_size, int increase_chunk_size, size_t MAX_MEMORY_LIMIT);
  ~ObjectPool();

  size_t useCount() const;
  size_t totalCount() const;

private:
  size_t memory() const { return totalCOunt() * size_of(T); }
  using BookingType = typename std::unordered_map<T *, bool>::value_type;

  template <typename... Args>
  T *acquireImpl(Args &&... args);
  void release(T *object_ptr);
  void allocate(int size);
  void tryAllocate(int size);
  T *getFree();

  size_t count_{};
  std::mutex mutex_;
  std::vector<void *> objectChunk_;
  std::unordered_map<T *, bool> object_;
  int increaseChunkSize_;
  const size_t K_MAX_MEMROY_LIMIT;
};

template <typename T>
template <typename... Args>
T *ObjectPool<T>::acquireImpl(Args &&... args)
{
  std::lock_guard<std::mutex> lock(mutex_);
  tryAllocate(increaseChunkSize_);
  auto object_ptr = getFree();
  if (object_ptr)
  {
    new (object_ptr) T(std::forward<Args>(args)...);
    ++count_;
    object_[object_ptr] = true;
  }

  return object_ptr;
}

template <typename T>
ObjectPool<T>::ObjectHolder::ObjectHolder(HolderPtr &&ptr)
{
  object_.swap(ptr);
}

template <typename T>
T &
    ObjectPool<T>::ObjectHolder::operator*() const
{
  assert(object_ != nullptr);
  return *object_;
}

template <typename T>
template <typename... Args>
typename ObjectPool<T>::ObjectHolder
ObjectPool<T>::acquire(Args &&... args)
{
  using Deleter = std : function<void(T *) noexcept>;
  T *ptr = nullptr;
  Deleter deleter;

  if (memory() > K_MAX_MEMROY_LIMIT)
  {
    ptr = new T(std::forward<Args>(args)...);
    deleter = [this](T *ptr) {
      if (ptr)
      {
        delete ptr;
      }
    }
  }
  else
  {
    ptr = acquireImpl(std::forward<Args>(args)...);
    deleter = [this](T *ptr) {
      if (ptr)
      {
        this->release(ptr);
      }
    }
  }

  return ObjectHolder(std::unique_ptr<T, std::function<void(T *)>>(ptr, deleter));
}

template <typename T>
void ObjectPool<T>::release(T *object_ptr)
{
  if (object_ptr)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iterator = object_.find(object_ptr);
    if (iterator != object_.cend())
    {
      if (std::is_destructible<T>::value && std::is_class<T>::value)
      {
        iterator->first->~T();
      }
      iterator->second = false;
      --count_;
    }
  }
}

template <typename T>
ObjectPool<T>::ObjectPool(int init_size, int increase_chunk_size /* recommend 4k % sizeof(T) */, size_t MAX_MEMORY_LIMIT)
    : increaseChunkSize_(increase_chunk_size), K_MAX_MEMORY_LIMIT(MAX_MEMORY_LIMIT - sizeof(T))
{
  allocate(init_size);
}

template <typename T>
ObjectPool<T>::~ObjectPool()
{
  assert(count_ == 0);
  std::for_each(objectChunk_.begin(),
                objectChunk_.end(),
                [](void *raw) {
                  operator delete[](raw);
                });
}

template <typename T>
size_t
ObjectPool<T>::useCount() const
{
  return count_;
}

template <typename T>
size_t
ObjectPool<T>::totalCount() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return object_.size();
}

template <typename T>
void ObjectPool<T>::allocate(int size)
{
  auto raw = operator new[](sizeof(T) * size, std::nothrow_t());
  if (raw)
  {
    objectChunk_.push_back(raw);
    auto ptr = static_cast<T *>(raw);
    for (auto offset = 0; offset < size; ++offset)
    {
      object_.emplace(std::make_pair(ptr + offset, false));
    }
  }
}

template <typename T>
void ObjectPool<T>::tryAllocate(int size)
{
  auto const pred = [](const BookingType &ele) {
    return !ele.second;
  };

  auto unused = std::find_if(object_.cbegin(), object_.cend(), pred);
  if (unused == object_.cend())
  {
    allocate(size);
  }
}

template <typename T>
T *ObjectPool<T>::getFree()
{
  auto const pred = [](const BookingType &ele) {
    return !ele.second;
  };

  auto unused = std::find_if(object_.cbegin(), object_.cend(), pred);
  if (unused == object_.cend())
    return nullptr;
  return unused->first;
}
} // namespace zm
