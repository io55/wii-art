#ifndef _OBJECT_H
#define _OBJECT_H

#include "../util/colour.h"
#include <grrlib.h>

enum class ObjectType : u8 {
    Cube,
    Torus,
    Sphere
};

struct Object {
    guVector m_position = { 0, 0, 0 };
    f32 m_size = 1;
    u32 m_colour = util::white;
    bool m_wireFrame = false;

    ObjectType m_type;

    Object() = default;
    ~Object() = default;

    inline void rngPosition()
    {
        float extents = 10;
        m_position.x = util::getRandom<f32>(-extents, extents);
        m_position.y = util::getRandom<f32>(0.5f, 5.0f);
        m_position.z = util::getRandom<f32>(-extents, extents);
    }

    inline void rngScaling()
    {
        m_size = util::getRandom<f32>(0.5f, 1.5f);
    }

    inline void rngColour()
    {
        m_colour = util::GetColour(util::getRandom<u32>(0x00, 0xFF),
            util::getRandom<u32>(0x00, 0xFF), util::getRandom<u32>(0x00, 0xFF),
            util::getRandom<u32>(0x33, 0xFF));
    }

    inline void randomise()
    {
        rngPosition();
        rngScaling();
        rngColour();
    }

    inline void render()
    {
        GRRLIB_ObjectView(m_position.x, m_position.y, m_position.z, 0, 0, 0, m_size, m_size, m_size);
        switch (m_type) {
        case ObjectType::Cube:
            GRRLIB_DrawCube(m_size, !m_wireFrame, m_colour);
            break;
        case ObjectType::Torus:
            GRRLIB_DrawTorus(m_size / 2, m_size, 10, 10, !m_wireFrame, m_colour);
            break;
        case ObjectType::Sphere:
            GRRLIB_DrawSphere(m_size, 10, 10, !m_wireFrame, m_colour);
            break;
        }
    }
};

#endif