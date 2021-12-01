#ifndef _RANDOM_H
#define _RANDOM_H

#include "../math/vector.h"
#include <gctypes.h>
#include <type_traits>

namespace util {
/* Should only be of numeric type */
template <typename T> static inline auto getRandom(T minimum, T maximum)
{
    if constexpr (std::is_same<T, u32>::value) {
        return (rand() % (maximum - minimum + 1)) + minimum;
    } else if (std::is_same<T, f32>::value) {
        return minimum + (rand() / (float)RAND_MAX) * (maximum - minimum);
    }
}

/* Should only be of Vector2<T> type */
template <typename T> static inline auto getRandom(T extents) { return getRandom<T>(extents.m_x, extents.m_y); }

} // namespace util

#endif