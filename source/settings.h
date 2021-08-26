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
    Cube = 1 << 0,
    Torus = 1 << 1,
    Sphere = 1 << 2,

    All = Cube | Torus | Sphere,
};

// Simple implementation for bitflags
static inline u8 operator&(ObjectSpawnMode a, ObjectSpawnMode b)
{
    return static_cast<u8>(a) & static_cast<u8>(b);
}

struct Settings {
    // Vector2 instances are used like: [CURRENT VALUE, MAX VALUE]
    Vector2<u32> m_sceneObjCount = { 25, 50 };
    Vector2<u32> m_lightCount = { 1, 3 };

    ObjectSpawnMode m_spawnMode = ObjectSpawnMode::All;
    bool m_showUI = true;

    ProgramState m_state = ProgramState::FadeInText;
} gSettings;

#endif