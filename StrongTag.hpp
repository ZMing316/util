#pragma once


// using Height = StrongTag<double, struct HeightTag>;

namespace zm
{
   template <typename T, typename TagName>
   class StrongTag{
       public:
            explicit StrongTag(T&& value):value_(std::move(value)){}
            explicit StrongTag(const T& value):value_(value){}
            explicit operator T(){return value_;}
            explicit operator T const &() const {return value_;}
        private:
            T value_;
   };
} // namee zm
