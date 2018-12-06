#include <memory>

namespace zm
{
template <typename T, typename Allocator = std::allocator<T>>
class Builder
{
  public:
    using value_type = T;
    using pointer = T*;

    template <typename... ARGS>
    static pointer build(ARGS &&... args) noexcept
    {
        auto p = allocator_.allocate(1);
        if (p)
        {
            allocator_.construct(p, std::forward<ARGS>(args)...);
        }
        return p;
    }

    static void release(pointer p) noexcept
    {
        if (p)
        {
            allocator_.destroy(p);
            allocator_.deallocate(p, 1);
        }
    }

  private:
    static thread_local Allocator allocator_;
};

template <typename T, typename Allocator>
thread_local Allocator Builder<T, Allocator>::allocator_;

template <typename CLASS, typename... ARGS>
class IBuilder
{
  protected:
    virtual ~IBuilder() = default;

  public:
    using value_type = CLASS;
    using pointer = CLASS*;

    virtual pointer build(ARGS&&...) noexcept = 0;
    virtual void release(pointer) noexcept = 0;
};

} // namespace zm