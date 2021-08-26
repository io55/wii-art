#ifndef _RANDOM_H
#define _RANDOM_H

#include "../math/vector2.h"
#include <gctypes.h>
#include <type_traits>

namespace util {
// X is min, Y is max
static inline u32 getRandom(Vector2<u32> extents)
{
    return (rand() % (extents.m_y - extents.m_x + 1)) + extents.m_x;
}

// X is min, Y is max
static inline f32 getRandom(Vector2<f32> extents)
{
    return extents.m_x + (rand() / (float)RAND_MAX) * (extents.m_y - extents.m_x);
}

// Should only be of numeric type
template <typename T>
static inline auto getRandom(T minimum, T maximum)
{
    if constexpr (std::is_same<T, u32>::value) {
        return (rand() % (maximum - minimum + 1)) + minimum;
    } else if (std::is_same<T, f32>::value) {
        return minimum + (rand() / (float)RAND_MAX) * (maximum - minimum);
    }
}

// Should only be of Vector2<T> type
template <typename T>
static inline auto getRandom(T extents)
{
    return getRandom<T>(extents.m_x, extents.m_y);
}

}

#endif