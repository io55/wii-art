#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include <algorithm>
#include <cmath>

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
    Vector2<u32> m_sceneObjCount = { 5, 10 };

    ObjectSpawnMode m_spawnMode = ObjectSpawnMode::All;
} gSettings;

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
    gameMenuItems.push_back({ 0, { 64, 64 }, "RANDOMISE SCENE", 26, cl::white, cl::red });
    gameMenuItems.push_back({ 1, { 64, 64 + 24 }, "RANDOMISE SIZE", 26, cl::white, cl::red });
    gameMenuItems.push_back({ 2, { 64, 64 + 24 + 24 }, "RANDOMISE COLOURS", 26, cl::white, cl::red });
    gameMenuItems.push_back({ 3, { 64, 64 + 24 + 24 + 24 }, "RANDOMISE LIGHTS", 26, cl::white, cl::red });
    gameMenu.reset(0);

    Menu optionsMenu;
    std::vector<MenuItem>& optionsMenuItems = optionsMenu.getItems();
    optionsMenuItems.push_back({ 0, { 64, 64 }, "REPLACE_WITH_CODE", 46, cl::white, cl::red });
    optionsMenuItems.push_back({ 1, { 64, 64 + 48 }, "REPLACE_WITH_CODE", 46, cl::white, cl::red });
    optionsMenu.reset(0);

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    while (!gExit) {
        WPAD_ScanPads();

        u32 btns_down = WPAD_ButtonsDown(WPAD_CHAN_0);
        u32 btns_up = WPAD_ButtonsUp(WPAD_CHAN_0);
        u32 btns_held = WPAD_ButtonsHeld(WPAD_CHAN_0);

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
                    char text[0x30];
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

                    char text[0x30];
                    sprintf(text, "Spawn type: [%s]", type);
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
                }
            }

            break;
        }

        case ProgramState::MainGame: {
            gMainCamera->applyCamera();
            GRRLIB_ObjectView(0, 0, -5, 0, 0, 0, 1, 1, 1);
            GRRLIB_DrawCube(1, true, cl::white);

            GRRLIB_2dMode();

            for (MenuItem& item : gameMenuItems) {
                item.render(gFont);
            }

            if (btns_down & WPAD_BUTTON_B) {
                state = ProgramState::Menu;
                mainMenu.reset(0);
            }

            if (btns_down & WPAD_BUTTON_UP) {
                gameMenu.moveSelected(MenuDirection::Up);
            } else if (btns_down & WPAD_BUTTON_DOWN) {
                gameMenu.moveSelected(MenuDirection::Down);
            } else if (btns_down & WPAD_BUTTON_A) {
                MenuItem& item = gameMenu.getSelected();

                // SCENE
                if (item.m_index == 0) {
                }
                // SIZE
                if (item.m_index == 1) {
                }
                // COLOURS
                if (item.m_index == 2) {
                }
                // LIGHTS
                if (item.m_index == 3) {
                }
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