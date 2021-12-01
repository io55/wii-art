#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "math/vector.h"

enum class ProgramState : u8 {
    FadeInText,
    FadeOutText,

    // clang-format off
    MainMenu,
        Options,
        Extras,
            ChangeLog,
            Controls,
            GX_S55_S1,
            GX_S55_S2,
    // clang-format on

    MainGame
};

enum class ObjectSpawnMode : u8 {
    Cube   = 1 << 0,
    Torus  = 1 << 1,
    Sphere = 1 << 2,

    All = Cube | Torus | Sphere,
};

// Simple implementation for bitflags
static inline u8 operator&(ObjectSpawnMode a, ObjectSpawnMode b) { return static_cast<u8>(a) & static_cast<u8>(b); }

struct Options {
    // Vector2 instances are used like: [CURRENT VALUE, MAX VALUE]
    math::Vector2<u8> m_sceneObjCount = { 25, 50 };
    math::Vector2<u8> m_wfObjCount    = { 25, 50 };
    math::Vector2<u8> m_lightCount    = { 1, 3 };

    ObjectSpawnMode m_spawnMode = ObjectSpawnMode::All;
    bool m_showUI               = true;

    ProgramState m_state = ProgramState::FadeInText;

    void toggleOptionSpawnMode(const bool fwd)
    {
        if (fwd) {
            switch (m_spawnMode) {
            case ObjectSpawnMode::Cube:
                m_spawnMode = ObjectSpawnMode::Torus;
                break;
            case ObjectSpawnMode::Torus:
                m_spawnMode = ObjectSpawnMode::Sphere;
                break;
            case ObjectSpawnMode::Sphere:
                m_spawnMode = ObjectSpawnMode::All;
                break;
            case ObjectSpawnMode::All:
                m_spawnMode = ObjectSpawnMode::Cube;
                break;
            }
        } else {
            switch (m_spawnMode) {
            case ObjectSpawnMode::Cube:
                m_spawnMode = ObjectSpawnMode::All;
                break;
            case ObjectSpawnMode::Torus:
                m_spawnMode = ObjectSpawnMode::Cube;
                break;
            case ObjectSpawnMode::Sphere:
                m_spawnMode = ObjectSpawnMode::Torus;
                break;
            case ObjectSpawnMode::All:
                m_spawnMode = ObjectSpawnMode::Sphere;
                break;
            }
        }
    }

    void toggleOptionLightCount(const bool fwd) { moveSetting(m_lightCount, fwd); }
    void toggleOptionObjectCount(const bool fwd) { moveSetting(m_sceneObjCount, fwd); }
    void toggleOptionWfObjCount(const bool fwd) { moveSetting(m_wfObjCount, fwd); }

private:
    template <typename T> inline void moveSetting(math::Vector2<T>& toMove, const bool fwd)
    {
        if (fwd) {
            toMove.m_x++;
            if (toMove.m_x > toMove.m_y) {
                toMove.m_x = 0;
            }
        } else {
            toMove.m_x--;
            if ((s32)toMove.m_x == (s32)-1) {
                toMove.m_x += toMove.m_y;
            }
        }
    }
} gOptions;

#endif