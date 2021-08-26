#ifndef _CAMERA_H
#define _CAMERA_H

#include <grrlib.h>

class Camera
{
public:
  Camera(guVector pos, guVector up, guVector lookAt);
  ~Camera() = default;

  inline void applyCamera()
  {
    GRRLIB_Camera3dSettings(m_position.x, m_position.y, m_position.z, m_up.x, m_up.y, m_up.z, m_lookAt.x, m_lookAt.y, m_lookAt.z);
    GRRLIB_3dMode(m_minDrawDist, m_maxDrawDist, m_fov, m_useTexCoords, m_useNormalCoords);
  }

  inline const guVector &position() const { return m_position; }
  inline guVector &position() { return m_position; }

  inline const guVector &up() const { return m_up; }
  inline guVector &up() { return m_up; }

  inline const guVector &lookAt() const { return m_lookAt; }
  inline guVector &lookAt() { return m_lookAt; }

private:
  guVector m_position;
  guVector m_up;
  guVector m_lookAt;

  f32 m_minDrawDist;
  f32 m_maxDrawDist;
  f32 m_fov;
  bool m_useTexCoords;
  bool m_useNormalCoords;
};

#endif