#include "camera.h"

Camera::Camera(guVector pos, guVector up, guVector lookAt)
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