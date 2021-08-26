#ifndef _LIGHT_H
#define _LIGHT_H

#include "../util/colour.h"
#include "../util/random.h"
#include <grrlib.h>

struct Light {
    guVector m_position = { 0, 0, 0 };
    u32 m_colour        = util::white;
    f32 m_distattn      = 5;
    f32 m_brightness    = 5;

    const u32 m_index = 0;
    Light(u32 index);

    inline void randomise()
    {
        rngPosition();
        rngColour();
        rngDistAttn();
        rngBrightness();
    }

    inline void rngPosition()
    {
        m_position.x = util::getRandom<f32>(-7.5f, 7.5f);
        m_position.y = util::getRandom<f32>(2.5f, 10.0f);
        m_position.z = util::getRandom<f32>(-7.5f, 7.5f);
    }

    inline void rngColour()
    {
        m_colour = util::GetColour(util::getRandom<u32>(0x00, 0xFF), util::getRandom<u32>(0x00, 0xFF),
                                   util::getRandom<u32>(0x00, 0xFF));
    }

    inline void rngDistAttn() { m_distattn = util::getRandom<u32>(1, 10); }

    inline void rngBrightness() { m_brightness = util::getRandom<u32>(1, 10); }
};

#endif