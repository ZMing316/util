#pragma once


// using Height = StrongTag<double, struct HeightTag>;

namespace zm
{
   template <typename T, typename TagName>
   class StrongTag{
       public:
            // 编译时常量
            explicit constexpr StrongTag(T&& value):value_(std::move(value)){}
            explicit constexpr StrongTag(const T& value):value_(value){}
            explicit constexpr operator T(){return value_;}
            explicit constexpr operator T const &() const {return value_;}
        private:
            T value_;
   };
} // namee zm
