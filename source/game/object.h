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
    guVector m_position;
    f32 m_size;
    u32 m_colour = util::white;

    ObjectType m_type;

    Object() = default;
    ~Object() = default;

    void randomisePosition();
    void randomiseScaling();
    void randomiseColour();
    void render();
};

#endif