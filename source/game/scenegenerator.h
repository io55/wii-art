#ifndef _SCENEGENERATOR_H
#define _SCENEGENERATOR_H

#include <vector>

#include <grrlib.h>

#include "light.cpp"
#include "object.h"

struct SceneGenerator {
    SceneGenerator()  = default;
    ~SceneGenerator() = default;

    inline void setup()
    {
        randomiseObjects();
        randomiseLights();
    }

    inline void render()
    {
        for (auto& light : m_lights) {
            GRRLIB_SetLightDiff(light.m_index, light.m_position, 10, 5, light.m_colour);
        }

        for (auto& obj : m_objects) {
            obj.render();
        }

        for (auto& obj : m_wfObjects) {
            obj.render();
        }
    }

    void randomiseObjects();
    void randomiseLights();

    std::vector<Object> m_objects;
    std::vector<Object> m_wfObjects;
    std::vector<Light> m_lights;
} gSceneGenerator;

#endif