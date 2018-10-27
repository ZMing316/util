#pragma once

#include <functional>
#include <memory>

namespace zm
{
template <typename CLASS, typename ... ARGS>
class WeakCallback
{
public:
  WeakCallback(std::weak_ptr<CLASS> object, std::function<void(CLASS*, ARGS ...)> func)
    : object_(object)
    , func_(std::move(func))
  { }

  void
    operator()(ARGS&&... args)
  {
    if (auto object = object_.lock())
    {
      func_(object.get(), std::forward<ARGS>(args)...);
    }
  }

private:
  std::weak_ptr<CLASS> object_;
  std::function<void(CLASS*, ARGS ...)> func_;
};


template <typename CLASS, typename ... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(std::shared_ptr<CLASS> object, void(CLASS::*funcation)(ARGS...))
{
 return WeakCallback<CLASS, ARGS...>(object, funcation); 
}

template <typename CLASS, typename ... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(std::shared_ptr<CLASS> object, void(CLASS::*funcation)(ARGS...) const)
{
 return WeakCallback<CLASS, ARGS...>(object, funcation); 
}
}
