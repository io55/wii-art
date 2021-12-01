#ifndef _VECTOR_H
#define _VECTOR_H

#include "../types.hpp"

namespace math {
template <typename T> struct Vector2 {
    Vector2() = default;
    Vector2(const T& x, const T& y)
        : m_x(x)
        , m_y(y)
    {
    }
    ~Vector2() = default;

    T m_x, m_y;
};

using Vector2u = Vector2<u32>;
using Vector2i = Vector2<s32>;
using Vector2f = Vector2<f32>;
using Vector2d = Vector2<f64>;
} // namespace math

#endif