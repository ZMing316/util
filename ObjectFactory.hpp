#include <unordered_map>
#include <mutex>

#include "Builder.hpp"

namespace zm
{
template <typename ID, typename BASE, typename... ARGS>
class ObjectFactory
{
  public:
    std::shared_ptr<BASE> create(const ID &id, ARGS &&... args)
    {
        BASE *object = nullptr;
        std::lock_guard<std::mutex> lock(mutex_);
        const auto builder_itor = builders_.find(id);
        if (builder_itor != builders_.cend())
        {
            object = builder_itor->second->build(std::forward<Args>(args)...);
            return std::shared_ptr<BASE>(object, [builder = builder_itor->second](BASE *p) {
                builder->release(p);
            }
        }
        else
        {
            throw std::logic_error("Unknown id, pelease registe builder first");
        }
    }

    template <typename Builder>
    auto
    registerBuilder(const ID &id, std::shared_ptr<Builder> &&builder) -> typename std::enable_if<std::is_base_of<IBuilder<BASE, ARGS...>, Builder>::value, void>::type
    {
        assert(builder.get() != nullptr);
        if (builders_.find(id) != builders_.cend())
        {
            throw std::logic_error("ID has already be registed, if you want to continue please remove first.");
        }
        std::lock_guard<std::mutex> lock(mutex_);
        std::swap(builders_[id], builder);
    }

  private:
    std::unordered_map < ID, std::shared_ptr<IBuilder<BASE, ARGS...>> builders_;
    std::mutex mutex_;
}
} // namespace zm