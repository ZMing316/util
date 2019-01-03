#pragma once
#include <map>

template <typename T, typename U, typename... Args>
struct MultiLevelMap
{
  using type = std::map<T, typename MultiLevelMap<U, Args...>::type>;  
};

template <typename T, typename U>
struct MultiLevelMap<T, U>
{
  using type = std::map<T, U>; 
};


template <typename ...Args>
using MultiLevelMap_t = typename MultiLevelMap<Args...>::type; 
