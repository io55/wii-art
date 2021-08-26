#ifndef _SETTINGS_H
#define _SETTINGS_H

enum class ProgramState : u8 {
    FadeInText,
    FadeOutText,

    Menu,
    Options,

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

struct Settings {
    // Vector2 instances are used like: [CURRENT VALUE, MAX VALUE]
    Vector2<u32> m_sceneObjCount = { 25, 50 };
    Vector2<u32> m_wfObjCount    = { 25, 50 };
    Vector2<u32> m_lightCount    = { 1, 3 };

    ObjectSpawnMode m_spawnMode = ObjectSpawnMode::All;
    bool m_showUI               = true;

    ProgramState m_state = ProgramState::FadeInText;

    inline void moveSpawnMode(const bool fwd)
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
    inline void moveLightCount(const bool fwd)
    {
        if (fwd) {
            m_lightCount.m_x++;
            if (m_lightCount.m_x > m_lightCount.m_y) {
                m_lightCount.m_x -= m_lightCount.m_y;
            }
        } else {
            m_lightCount.m_x--;
            if (m_lightCount.m_x == 0) {
                m_lightCount.m_x += m_lightCount.m_y;
            }
        }
    }
    inline void moveObjectCount(const bool fwd)
    {
        if (fwd) {
            m_sceneObjCount.m_x++;
            if (m_sceneObjCount.m_x > m_sceneObjCount.m_y) {
                m_sceneObjCount.m_x = 0;
            }
        } else {
            m_sceneObjCount.m_x--;
            if (m_sceneObjCount.m_x == (s32)-1) {
                m_sceneObjCount.m_x += m_sceneObjCount.m_y;
            }
        }
    }
    inline void moveWfObjCount(const bool fwd)
    {
        if (fwd) {
            m_wfObjCount.m_x++;
            if (m_wfObjCount.m_x > m_wfObjCount.m_y) {
                m_wfObjCount.m_x = 0;
            }
        } else {
            m_wfObjCount.m_x--;
            if ((s32)m_wfObjCount.m_x == (s32)-1) {
                m_wfObjCount.m_x += m_wfObjCount.m_y;
            }
        }
    }
} gSettings;

#endif