#ifndef _CAMERA_H
#define _CAMERA_H

#include <grrlib.h>

class Camera {
public:
    Camera(guVector pos, guVector up, guVector lookAt)
        : m_position(pos)
        , m_up(up)
        , m_lookAt(lookAt)
        , m_minDrawDist(0.1f)
        , m_maxDrawDist(1000)
        , m_fov(45)
        , m_useTexCoords(false)
        , m_useNormalCoords(true)
    {
        applyCamera();
    }

    ~Camera() = default;

    void applyCamera()
    {
        GRRLIB_Camera3dSettings(m_position.x, m_position.y, m_position.z, m_up.x, m_up.y, m_up.z, m_lookAt.x,
                                m_lookAt.y, m_lookAt.z);
        GRRLIB_3dMode(m_minDrawDist, m_maxDrawDist, m_fov, m_useTexCoords, m_useNormalCoords);
    }

    const guVector& position() const { return m_position; }
    guVector& position() { return m_position; }

    const guVector& up() const { return m_up; }
    guVector& up() { return m_up; }

    const guVector& lookAt() const { return m_lookAt; }
    guVector& lookAt() { return m_lookAt; }

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