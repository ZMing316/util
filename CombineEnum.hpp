#pragma once

#include <cstdint>

namespace zm
{

template <typename Enum>
constexpr size_t enumSize()
{
    return static_cast<size_t>(Enum::Last);
}

template <typename Enum>
constexpr size_t enumValue(Enum e)
{
    return static_cast<size_t>(e);
}

template <typename Enum1, typename Enum2>
struct EnumCombine
{
    constexpr size_t enums(Enum1 enum1, Enum2 enum2)
    {
        return enumValue(enum1) * enumSize<Enum2>() + enumValue(enum2);
    }
}

} // namespace zm
