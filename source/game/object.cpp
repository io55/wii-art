#include "object.h"
#include "../util/random.h"

void Object::randomisePosition()
{
    m_position.x = util::getRandom<f32>(-7.5f, 7.5f);
    m_position.y = util::getRandom<f32>(0.5f, 5.0f);
    m_position.z = util::getRandom<f32>(-7.5f, 7.5f);
}

void Object::randomiseScaling()
{
    m_size = util::getRandom<f32>(0.5f, 1.5f);
}

void Object::randomiseColour()
{
    m_colour = util::GetColour(util::getRandom<u32>(0x00, 0xFF),
        util::getRandom<u32>(0x00, 0xFF), util::getRandom<u32>(0x00, 0xFF),
        util::getRandom<u32>(0x33, 0xFF));
}

void Object::render()
{
    GRRLIB_ObjectView(m_position.x, m_position.y, m_position.z, 0, 0, 0, m_size, m_size, m_size);
    switch (m_type) {
    case ObjectType::Cube:
        GRRLIB_DrawCube(m_size, true, m_colour);
        break;
    case ObjectType::Torus:
        GRRLIB_DrawTorus(m_size / 2, m_size, 10, 10, true, m_colour);
        break;
    case ObjectType::Sphere:
        GRRLIB_DrawSphere(m_size, 10, 10, true, m_colour);
        break;
    }
}