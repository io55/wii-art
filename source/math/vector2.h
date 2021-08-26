#ifndef _VECTOR2_H
#define _VECTOR2_H

template <typename T>
struct Vector2
{
  Vector2() = default;
  ~Vector2() = default;

  T m_x,
      m_y;
};

#endif