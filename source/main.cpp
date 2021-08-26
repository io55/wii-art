#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "icon_jpg.h"
#include "terminus_ttf.h"

#include "globals.h"
#include "math/camera.cpp"
#include "menu.h"
#include "util/colour.h"

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

struct Settings {
    // VECTOR 2 INSTANCES WORK AS: [CURRENT VALUE, MAX VALUE]
    Vector2<u32> m_sceneObjCount = { 15, 30 };
    Vector2<u32> m_lightCount = { 1, 5 };

    ObjectSpawnMode m_spawnMode = ObjectSpawnMode::All;
    bool m_showUI = true;
} gSettings;

enum class ObjectType : u8 {
    Cube,
    Torus,
    Sphere
};

// X is min, Y is max
static inline u32 getRandom(Vector2<u32> extents)
{
    return (rand() % (extents.m_y - extents.m_x + 1)) + extents.m_x;
}

// X is min, Y is max
static inline f32 getRandom(Vector2<f32> extents)
{
    return extents.m_x + (rand() / (float)RAND_MAX) * (extents.m_y - extents.m_x);
}

struct Object {
    guVector m_position;
    f32 m_size;
    u32 m_colour = cl::white;

    ObjectType m_type;

    void randomisePosition()
    {
        m_position.x = getRandom(Vector2<f32>({ -7.5f, 7.5f }));
        m_position.y = getRandom(Vector2<f32>({ 0.5f, 5.0f }));
        m_position.z = getRandom(Vector2<f32>({ -7.5f, 7.5f }));
    }

    void randomiseScaling()
    {
        m_size = getRandom(Vector2<f32>({ 0.5f, 1.5f }));
    }

    void randomiseColour()
    {
        Vector2<u32> rng = { 0x00, 0xFF };
        Vector2<u32> rngAlpha = { 0x33, 0xAA };
        m_colour = cl::GetColour(getRandom(rng), getRandom(rng), getRandom(rng), getRandom(rngAlpha));
    }

    void render()
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
};

struct Light {
    guVector m_position;
    u32 m_colour;
    u32 m_index = 0;
    f32 m_distattn;
    f32 m_brightness;
};

struct RandomGenerator {
    std::vector<Object> m_objects;
    std::vector<Light> m_lights;

    void setup()
    {
        m_objects.clear();
        for (u32 i = 0; i < gSettings.m_sceneObjCount.m_x; i++) {
            while (true) {
                Vector2<u32> rngGen = { 0, 6 };
                u32 rng = getRandom(rngGen);
                if (rng < 2) {
                    if ((u8)gSettings.m_spawnMode & (u8)ObjectSpawnMode::Cube) {
                        Object obj;
                        obj.m_type = ObjectType::Cube;
                        obj.randomiseColour();
                        obj.randomisePosition();
                        obj.randomiseScaling();
                        m_objects.push_back(obj);
                        break;
                    }
                } else if (rng >= 2 && rng < 4) {
                    if ((u8)gSettings.m_spawnMode & (u8)ObjectSpawnMode::Torus) {
                        Object obj;
                        obj.m_type = ObjectType::Torus;
                        obj.randomiseColour();
                        obj.randomisePosition();
                        obj.randomiseScaling();
                        m_objects.push_back(obj);
                        break;
                    }
                } else if (rng >= 4) {
                    if ((u8)gSettings.m_spawnMode & (u8)ObjectSpawnMode::Sphere) {
                        Object obj;
                        obj.m_type = ObjectType::Sphere;
                        obj.randomiseColour();
                        obj.randomisePosition();
                        obj.randomiseScaling();
                        m_objects.push_back(obj);
                        break;
                    }
                }
            }
        }

        randomiseLights();
    }

    void randomiseLights()
    {
        m_lights.clear();
        for (u32 i = 0; i < gSettings.m_lightCount.m_x; i++) {
            Light light;
            Vector2<u32> rng = { 0x00, 0xFF };
            Vector2<f32> rngF = { 1, 10 };
            light.m_brightness = getRandom(rngF);
            light.m_distattn = getRandom(rngF);
            light.m_colour = cl::GetColour(getRandom(rng), getRandom(rng), getRandom(rng));
            light.m_index = i;
            light.m_position.x = getRandom(Vector2<f32>({ -7.5f, 7.5f }));
            light.m_position.y = getRandom(Vector2<f32>({ 2.5f, 10.0f }));
            light.m_position.z = getRandom(Vector2<f32>({ -7.5f, 7.5f }));
            m_lights.push_back(light);
        }
    }

    void render()
    {
        for (auto& light : m_lights) {
            GRRLIB_SetLightDiff(light.m_index, light.m_position, 10, 5, light.m_colour);
        }
        for (auto& obj : m_objects) {
            obj.render();
        }
    }
};

static void IncrementSpawnMode(const bool fwd)
{
    ObjectSpawnMode& mode = gSettings.m_spawnMode;
    if (fwd) {
        switch (mode) {
        case ObjectSpawnMode::Cube:
            mode = ObjectSpawnMode::Torus;
            break;
        case ObjectSpawnMode::Torus:
            mode = ObjectSpawnMode::Sphere;
            break;
        case ObjectSpawnMode::Sphere:
            mode = ObjectSpawnMode::All;
            break;
        case ObjectSpawnMode::All:
            mode = ObjectSpawnMode::Cube;
            break;
        }
    } else {
        switch (mode) {
        case ObjectSpawnMode::Cube:
            mode = ObjectSpawnMode::All;
            break;
        case ObjectSpawnMode::Torus:
            mode = ObjectSpawnMode::Cube;
            break;
        case ObjectSpawnMode::Sphere:
            mode = ObjectSpawnMode::Torus;
            break;
        case ObjectSpawnMode::All:
            mode = ObjectSpawnMode::Sphere;
            break;
        }
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    GRRLIB_Init();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS);

    // Initialise globals
    gMainCamera = new Camera({ 0, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0 });
    gFont.init(terminus_ttf, terminus_ttf_size);
    GRRLIB_texImg* icon = GRRLIB_LoadTextureJPG(icon_jpg);

    ProgramState state = ProgramState::FadeInText;
    float fadeTimer = 0;

    Menu mainMenu;
    std::vector<MenuItem>& mainMenuItems = mainMenu.getItems();
    mainMenuItems.push_back({ 0, { 64, 241 }, "START", 46, cl::white, cl::red });
    mainMenuItems.push_back({ 1, { 64, 301 }, "OPTIONS", 46, cl::white, cl::red });
    mainMenuItems.push_back({ 2, { 64, 361 }, "EXIT", 46, cl::white, cl::red });
    mainMenu.reset(0);

    Menu gameMenu;
    std::vector<MenuItem>& gameMenuItems = gameMenu.getItems();
    gameMenuItems.push_back({ 0, { 32, 32 }, "RANDOMISE SCENE", 26, cl::white, cl::red });
    gameMenuItems.push_back({ 1, { 32, 32 + 24 }, "RANDOMISE SIZE", 26, cl::white, cl::red });
    gameMenuItems.push_back({ 2, { 32, 32 + 24 + 24 }, "RANDOMISE COLOURS", 26, cl::white, cl::red });
    gameMenuItems.push_back({ 3, { 32, 32 + 24 + 24 + 24 }, "RANDOMISE LIGHTS", 26, cl::white, cl::red });
    gameMenuItems.push_back({ 4, { 32, 32 + 24 + 24 + 24 + 24 }, "PRESS 1 TO HIDE", 26, cl::yellow, cl::yellow, false });
    gameMenu.reset(0);

    Menu optionsMenu;
    std::vector<MenuItem>& optionsMenuItems = optionsMenu.getItems();
    optionsMenuItems.push_back({ 0, { 64, 64 }, "REPLACE_WITH_CODE", 46, cl::white, cl::red });
    optionsMenuItems.push_back({ 1, { 64, 64 + 48 }, "REPLACE_WITH_CODE", 46, cl::white, cl::red });
    optionsMenuItems.push_back({ 2, { 64, 64 + 48 + 48 }, "REPLACE_WITH_CODE", 46, cl::white, cl::red });
    optionsMenu.reset(0);

    RandomGenerator sceneGenerator;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    while (!gExit) {
        WPAD_ScanPads();

        u32 btns_down = WPAD_ButtonsDown(WPAD_CHAN_0);
        u32 btns_held = WPAD_ButtonsHeld(WPAD_CHAN_0);
        // u32 btns_up = WPAD_ButtonsUp(WPAD_CHAN_0);

        GRRLIB_2dMode();

        if (state != ProgramState::MainGame) {
            f32 scale = 0.25f;
            u32 amtX = std::ceil(rmode->fbWidth / (icon->w * scale)) + 1;
            u32 amtY = std::ceil(rmode->xfbHeight / (icon->h * scale)) + 1;

            static f32 scrollTimer = 0;
            for (u32 x = 0; x < amtX; x++) {
                for (u32 y = 0; y < amtY; y++) {
                    // stepX should evaluate to 160
                    f32 stepX = rmode->fbWidth / (amtX - 1);
                    f32 xPos = ((x * stepX) + scrollTimer) - stepX;
                    while (xPos > rmode->fbWidth) {
                        xPos -= rmode->fbWidth + stepX;
                    }

                    f32 stepY = rmode->xfbHeight / (amtY - 1);
                    f32 yPos = ((y * stepY) + scrollTimer) - stepY;
                    while (yPos > rmode->xfbHeight) {
                        yPos -= rmode->xfbHeight + stepY;
                    }

                    GRRLIB_DrawImg(xPos, yPos, icon,
                        0, scale, scale, cl::GetColour(0x55, 0x55, 0x55, 0x64));
                }
            }
            scrollTimer += 0.075671f;
        }

        switch (state) {
        case ProgramState::FadeInText: {
            u8 alpha = std::min(255.0f, fadeTimer);
            f32 scale = 1;
            GRRLIB_DrawImg((rmode->fbWidth / 2) - ((icon->w * scale) / 2),
                (rmode->xfbHeight / 2) - ((icon->h * scale) / 2), icon, 0, scale, scale,
                cl::GetColour(0xFF, 0xFF, 0xFF, alpha));

            if (alpha == 255) {
                static u32 timer = 0;
                if (timer > 64) {
                    fadeTimer = 255;
                    state = ProgramState::FadeOutText;
                }
                timer += 1;
                break;
            }

            fadeTimer += 2.5f;
            break;
        }
        case ProgramState::FadeOutText: {
            u8 alpha = std::max(0.0f, fadeTimer);
            f32 scale = 1;
            GRRLIB_DrawImg((rmode->fbWidth / 2) - ((icon->w * scale) / 2),
                (rmode->xfbHeight / 2) - ((icon->h * scale) / 2), icon, 0, scale, scale,
                cl::GetColour(0xFF, 0xFF, 0xFF, alpha));

            if (alpha == 0) {
                state = ProgramState::Menu;
            }

            fadeTimer -= 2.5f;
            break;
        }
        case ProgramState::Menu: {
            const f32 scale = 0.5f;
            const u32 xpos = (rmode->fbWidth / 2) - ((icon->w * scale) / 2);
            GRRLIB_DrawImg(xpos, 64, icon, 0, scale, scale, cl::white);

            for (MenuItem& item : mainMenuItems) {
                item.m_position.m_x = xpos;
                item.render(gFont);
            }

            if (btns_down & WPAD_BUTTON_UP) {
                mainMenu.moveSelected(MenuDirection::Up);
            } else if (btns_down & WPAD_BUTTON_DOWN) {
                mainMenu.moveSelected(MenuDirection::Down);
            }

            if (btns_down & WPAD_BUTTON_A) {
                MenuItem& selected = mainMenu.getSelected();

                // Start
                if (selected.m_index == 0) {
                    state = ProgramState::MainGame;
                    sceneGenerator.setup();
                }
                // Options
                else if (selected.m_index == 1) {
                    state = ProgramState::Options;
                }
                // Exit
                else if (selected.m_index == 2) {
                    gExit = true;
                }
            }

            break;
        }
        case ProgramState::Options: {
            for (MenuItem& item : optionsMenuItems) {
                if (item.m_index == 0) {
                    char text[0x20];
                    sprintf(text, "Object count: [%d / %d]", gSettings.m_sceneObjCount.m_x, gSettings.m_sceneObjCount.m_y);
                    item.m_text = text;
                } else if (item.m_index == 1) {
                    char type[0x10];
                    switch (gSettings.m_spawnMode) {
                    case ObjectSpawnMode::All:
                        sprintf(type, "All");
                        break;
                    case ObjectSpawnMode::Cube:
                        sprintf(type, "Cube");
                        break;
                    case ObjectSpawnMode::Torus:
                        sprintf(type, "Torus");
                        break;
                    case ObjectSpawnMode::Sphere:
                        sprintf(type, "Sphere");
                        break;
                    }

                    char text[0x20];
                    sprintf(text, "Spawn type: [%s]", type);
                    item.m_text = text;
                } else if (item.m_index == 2) {
                    char text[0x20];
                    sprintf(text, "Light count: [%d / %d]", gSettings.m_lightCount.m_x, gSettings.m_lightCount.m_y);
                    item.m_text = text;
                }

                item.render(gFont);
            }

            if (btns_down & WPAD_BUTTON_UP) {
                optionsMenu.moveSelected(MenuDirection::Up);
            } else if (btns_down & WPAD_BUTTON_DOWN) {
                optionsMenu.moveSelected(MenuDirection::Down);
            }

            if (btns_down & WPAD_BUTTON_B) {
                state = ProgramState::Menu;
            }

            if (btns_down & WPAD_BUTTON_RIGHT) {
                MenuItem& item = optionsMenu.getSelected();
                // Scene object count+
                if (item.m_index == 0) {
                    gSettings.m_sceneObjCount.m_x++;
                    if (gSettings.m_sceneObjCount.m_x > gSettings.m_sceneObjCount.m_y) {
                        gSettings.m_sceneObjCount.m_x -= gSettings.m_sceneObjCount.m_y;
                    }
                } else if (item.m_index == 1) {
                    IncrementSpawnMode(true);
                } else if (item.m_index == 2) {
                    gSettings.m_lightCount.m_x++;
                    if (gSettings.m_lightCount.m_x > gSettings.m_lightCount.m_y) {
                        gSettings.m_lightCount.m_x -= gSettings.m_lightCount.m_y;
                    }
                }
            } else if (btns_down & WPAD_BUTTON_LEFT) {
                MenuItem& item = optionsMenu.getSelected();
                // Scene object count-
                if (item.m_index == 0) {
                    gSettings.m_sceneObjCount.m_x--;
                    if (gSettings.m_sceneObjCount.m_x == 0) {
                        gSettings.m_sceneObjCount.m_x += gSettings.m_sceneObjCount.m_y;
                    }
                } else if (item.m_index == 1) {
                    IncrementSpawnMode(false);
                } else if (item.m_index == 2) {
                    gSettings.m_lightCount.m_x--;
                    if (gSettings.m_lightCount.m_x == 0) {
                        gSettings.m_lightCount.m_x += gSettings.m_lightCount.m_y;
                    }
                }
            }

            break;
        }

        case ProgramState::MainGame: {
            guVector& position = gMainCamera->position();
            position.x = sin(gTimer * 0.05f) * 20;
            position.z = cos(gTimer * 0.05f) * 20;
            position.y = 10 + sin(gTimer * 0.01f) * 5;

            guVector& look = gMainCamera->lookAt();
            look.x = look.y = look.z = 0;
            gMainCamera->applyCamera();

            sceneGenerator.render();

            // Plane
            GRRLIB_ObjectView(0, -1, 0, 0, 0, 0, 50, 0.1f, 50);
            GRRLIB_DrawCube(1, true, cl::white);

            GRRLIB_2dMode();

            if (gSettings.m_showUI) {
                GRRLIB_Rectangle(32, 36, 226, 96 + 24, cl::GetColour(0x44, 0x44, 0x44), true);
                for (MenuItem& item : gameMenuItems) {
                    item.render(gFont);
                }

                if (btns_down & WPAD_BUTTON_UP) {
                    gameMenu.moveSelected(MenuDirection::Up);
                } else if (btns_down & WPAD_BUTTON_DOWN) {
                    gameMenu.moveSelected(MenuDirection::Down);
                } else if (btns_down & WPAD_BUTTON_A) {
                    MenuItem& item = gameMenu.getSelected();

                    // SCENE
                    if (item.m_index == 0) {
                        sceneGenerator.setup();
                    }
                    // SIZE
                    if (item.m_index == 1) {
                        for (auto& obj : sceneGenerator.m_objects) {
                            obj.randomiseScaling();
                        }
                    }
                    // COLOURS
                    if (item.m_index == 2) {
                        for (auto& obj : sceneGenerator.m_objects) {
                            obj.randomiseColour();
                        }
                    }
                    // LIGHTS
                    if (item.m_index == 3) {
                        sceneGenerator.randomiseLights();
                    }
                }
            } else {
                if (btns_down & WPAD_BUTTON_2) {
                    sceneGenerator.setup();
                }
            }

            if (btns_down & WPAD_BUTTON_1) {
                gSettings.m_showUI = !gSettings.m_showUI;
            }

            if (btns_down & WPAD_BUTTON_B) {
                state = ProgramState::Menu;
                mainMenu.reset(0);
            }
            break;
        }
        }

        GRRLIB_Render();

        gTimer += 0.05f;
    }

    delete gMainCamera;
    GRRLIB_FreeTexture(icon);
    GRRLIB_Exit();
    exit(0);
}