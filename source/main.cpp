#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "icon_jpg.h"
#include "terminus_ttf.h"

#include "game/scenegenerator.cpp"
#include "globals.h"
#include "math/camera.cpp"
#include "menu.h"
#include "settings.h"

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

    float fadeTimer = 0;

    Menu mainMenu;
    mainMenu.addMenuItem({ { 64, 241 }, "START", 46, util::white, util::red });
    mainMenu.addMenuItem({ { 64, 301 }, "SETTINGS", 46, util::white, util::red });
    mainMenu.addMenuItem({ { 64, 361 }, "EXIT", 46, util::white, util::red });
    mainMenu.reset(0);

    Menu gameMenu;
    gameMenu.addMenuItem({ { 32, 32 }, "RANDOMISE SCENE", 26, util::white, util::red });
    gameMenu.addMenuItem({ { 32, 56 }, "RANDOMISE SIZE", 26, util::white, util::red });
    gameMenu.addMenuItem({ { 32, 80 }, "RANDOMISE COLOURS", 26, util::white, util::red });
    gameMenu.addMenuItem({ { 32, 104 }, "RANDOMISE LIGHTS", 26, util::white, util::red });
    gameMenu.addMenuItem({ { 32, 128 }, "PRESS 1 TO HIDE", 26, util::yellow, util::yellow, false });
    gameMenu.reset(0);

    Menu settingsMenu;
    settingsMenu.addMenuItem({ { 64, 64 }, "REPLACE_WITH_CODE", 46, util::white, util::red });
    settingsMenu.addMenuItem({ { 64, 112 }, "REPLACE_WITH_CODE", 46, util::white, util::red });
    settingsMenu.addMenuItem({ { 64, 160 }, "REPLACE_WITH_CODE", 46, util::white, util::red });
    settingsMenu.reset(0);

    /* Main menu flow acts as such:
     * -> Image fades in
     * -> Image fade stops, waits for a second or two
     * -> Image fades out
     * 
     * Logo shows
     * Main menu renders & updates
     * Program state = ProgramState::MainMenu
     */

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    while (!gExit) {
        WPAD_ScanPads();

        u32 btns_down = WPAD_ButtonsDown(WPAD_CHAN_0);

        GRRLIB_2dMode();

        if (gSettings.m_state != ProgramState::MainGame) {
            /* Work out the "step" size for each image to allow for a tiled
             * background, using the scaling of the image as well and an extra
             * pane offset by 1 for the gap left by only generating tiles on
             * the screen but moving them in a direction
             */
            f32 scale = 0.25f;
            u32 amtX = std::ceil(rmode->fbWidth / (icon->w * scale)) + 1;
            u32 amtY = std::ceil(rmode->xfbHeight / (icon->h * scale)) + 1;

            static f32 scrollTimer = 0;
            for (u32 x = 0; x < amtX; x++) {
                for (u32 y = 0; y < amtY; y++) {
                    // stepX should evaluate to 160
                    f32 stepX = rmode->fbWidth / (amtX - 1);

                    // ((0 * 160) + {scrolling_offset}) - 160 = -160.? (fills the gap left by moving tiles)
                    // ((1 * 160) + {scrolling_offset}) - 160 = 0.?
                    // ((2 * 160) + {scrolling_offset}) - 160 = 160.?
                    // etc.
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
                        0, scale, scale, util::GetColour(0x55, 0x55, 0x55, 0x64));
                }
            }
            scrollTimer += 0.07571f;
        }

        switch (gSettings.m_state) {
        case ProgramState::FadeInText: {
            u8 alpha = std::min(255.0f, fadeTimer);
            f32 scale = 1;
            GRRLIB_DrawImg((rmode->fbWidth / 2) - ((icon->w * scale) / 2),
                (rmode->xfbHeight / 2) - ((icon->h * scale) / 2), icon, 0, scale, scale,
                util::GetColour(0xFF, 0xFF, 0xFF, alpha));

            if (alpha == 255) {
                static u32 logoTimer = 0;
                if (logoTimer > 64) {
                    fadeTimer = 255;
                    gSettings.m_state = ProgramState::FadeOutText;
                }
                logoTimer += 1;
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
                util::GetColour(0xFF, 0xFF, 0xFF, alpha));

            if (alpha == 0) {
                gSettings.m_state = ProgramState::Menu;
            }

            fadeTimer -= 2.5f;
            break;
        }
        case ProgramState::Menu: {
            const f32 scale = 0.5f;
            const u32 xpos = (rmode->fbWidth / 2) - ((icon->w * scale) / 2);
            GRRLIB_DrawImg(xpos, 64, icon, 0, scale, scale, util::white);

            for (MenuItem& item : mainMenu.getItems()) {
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
                    gSettings.m_state = ProgramState::MainGame;
                    gSceneGenerator.setup();
                }
                // Options
                else if (selected.m_index == 1) {
                    gSettings.m_state = ProgramState::Options;
                }
                // Exit
                else if (selected.m_index == 2) {
                    gExit = true;
                }
            }

            break;
        }
        case ProgramState::Options: {
            for (MenuItem& item : settingsMenu.getItems()) {
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
                settingsMenu.moveSelected(MenuDirection::Up);
            } else if (btns_down & WPAD_BUTTON_DOWN) {
                settingsMenu.moveSelected(MenuDirection::Down);
            }

            if (btns_down & WPAD_BUTTON_B) {
                gSettings.m_state = ProgramState::Menu;
            }

            if (btns_down & WPAD_BUTTON_RIGHT) {
                MenuItem& item = settingsMenu.getSelected();
                // Scene object count+
                if (item.m_index == 0) {
                    gSettings.moveSceneCount(true);
                } else if (item.m_index == 1) {
                    gSettings.moveSpawnMode(true);
                } else if (item.m_index == 2) {
                    gSettings.moveLightCount(true);
                }
            } else if (btns_down & WPAD_BUTTON_LEFT) {
                MenuItem& item = settingsMenu.getSelected();
                // Scene object count-
                if (item.m_index == 0) {
                    gSettings.moveSceneCount(false);
                } else if (item.m_index == 1) {
                    gSettings.moveSpawnMode(false);
                } else if (item.m_index == 2) {
                    gSettings.moveLightCount(false);
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

            gSceneGenerator.render();

            // Plane
            GRRLIB_ObjectView(0, -1, 0, 0, 0, 0, 50, 0.1f, 50);
            GRRLIB_DrawCube(1, true, util::white);

            GRRLIB_2dMode();

            if (gSettings.m_showUI) {
                GRRLIB_Rectangle(32, 36, 226, 120, util::GetColour(0x44, 0x44, 0x44), true);
                for (MenuItem& item : gameMenu.getItems()) {
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
                        gSceneGenerator.setup();
                    }
                    // SIZE
                    if (item.m_index == 1) {
                        for (auto& obj : gSceneGenerator.m_objects) {
                            obj.rngScaling();
                        }
                    }
                    // COLOURS
                    if (item.m_index == 2) {
                        for (auto& obj : gSceneGenerator.m_objects) {
                            obj.rngColour();
                        }
                    }
                    // LIGHTS
                    if (item.m_index == 3) {
                        gSceneGenerator.randomiseLights();
                    }
                }
            } else {
                if (btns_down & WPAD_BUTTON_2) {
                    gSceneGenerator.setup();
                }
            }

            if (btns_down & WPAD_BUTTON_1) {
                gSettings.m_showUI = !gSettings.m_showUI;
            }

            if (btns_down & WPAD_BUTTON_B) {
                gSettings.m_state = ProgramState::Menu;
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