#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

#include "icon_jpg.h"
#include "terminus_ttf.h"

#include "game/scenegenerator.cpp"
#include "globals.h"
#include "math/camera.h"
#include "menu.h"
#include "options.h"

static inline void PopulateMenuWithStride(Menu& menu, u32 fontSize, math::Vector2u position, math::Vector2u stride,
                                          std::vector<const char*> items)
{
    math::Vector2u current = position;
    for (const char* item : items) {
        menu.addMenuItem({ { current.m_x, current.m_y }, item, fontSize, util::white, util::red });
        current.m_x += stride.m_x;
        current.m_y += stride.m_y;
    }
}

enum class OptionMenuItems : u8 { ObjectCount = 0, WireframeObjectCount, SpawnMode, LightCount };
enum class MainMenuItems : u8 { Start, Options, Extra, Quit };
enum class ExtraMenuItems : u8 { ChangeLog, Controls, GX_S55_1, GX_S55_2 };

int main(int argc, char** argv)
{
    srand(time(NULL));

    GRRLIB_Init();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS);

    gMainCamera = new Camera({ 0, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0 });
    gFont.init(terminus_ttf, terminus_ttf_size);
    GRRLIB_texImg* icon = GRRLIB_LoadTextureJPG(icon_jpg);

    float fadeTimer = 0;

    Menu mainMenu;
    PopulateMenuWithStride(mainMenu, 46, math::Vector2u(64, 80 + static_cast<u32>(icon->h * 0.5f)),
                           math::Vector2u(0, 48), { "Start", "Options", "Extras", "Exit" });
    mainMenu.reset(0);

    Menu extraMenu;
    PopulateMenuWithStride(extraMenu, 56, math::Vector2u(64, 64), math::Vector2u(32, 64),
                           { "What's new?", "Controls", "GX_S55_S1", "GX_S55_S2" });
    extraMenu.reset(0);

    Menu gameMenu;
    PopulateMenuWithStride(gameMenu, 26, math::Vector2u(32, 32), math::Vector2u(0, 24),
                           { "Randomise scene", "Randomise size", "Randomise colours", "Randomise lights" });
    gameMenu.addMenuItem({ math::Vector2u(32, 128), "PRESS 1 TO HIDE", 26, util::yellow, util::yellow, false });
    gameMenu.reset(0);

    Menu optionsMenu;
    PopulateMenuWithStride(optionsMenu, 46, math::Vector2u(64, 64), math::Vector2u(0, 48),
                           { "REPLACE", "REPLACE", "REPLACE", "REPLACE" });
    optionsMenu.reset(0);

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
        u32 btns_held = WPAD_ButtonsHeld(WPAD_CHAN_0);

        GRRLIB_2dMode();

        if (gOptions.m_state != ProgramState::MainGame && gOptions.m_state != ProgramState::GX_S55_S1
            && gOptions.m_state != ProgramState::GX_S55_S2) {
            /* Calculate the positional offset and size for each image to allow
             * seamless tiled background, using the scaling of the image as well
             * as an extra pane that is offset by 1 to fill a gap also generated
             * by this algorithm
             */
            f32 scale = 0.25f;
            u32 amtX  = std::ceil(rmode->fbWidth / (icon->w * scale)) + 1;
            u32 amtY  = std::ceil(rmode->xfbHeight / (icon->h * scale)) + 1;

            static f32 scrollTimer = 0;
            for (u32 x = 0; x < amtX; x++) {
                for (u32 y = 0; y < amtY; y++) {
                    /* stepX should evaluate to 160 */
                    f32 stepX = rmode->fbWidth / (amtX - static_cast<f32>(1));

                    /* ((0 * 160) + {scrolling_offset}) - 160 = -160.? (fills
                     * the gap left by moving tiles)
                     * ((1 * 160) + {scrolling_offset}) - 160 = 0.?
                     * ((2 * 160) + {scrolling_offset}) - 160 = 160.?
                     * etc.
                     */
                    f32 xPos = ((x * stepX) + scrollTimer) - stepX;
                    while (xPos > rmode->fbWidth) {
                        xPos -= rmode->fbWidth + stepX;
                    }

                    f32 stepY = rmode->xfbHeight / (amtY - static_cast<f32>(1));
                    f32 yPos  = ((y * stepY) + scrollTimer) - stepY;
                    while (yPos > rmode->xfbHeight) {
                        yPos -= rmode->xfbHeight + stepY;
                    }

                    GRRLIB_DrawImg(xPos, yPos, icon, 0, scale, scale, util::getColour(0x55, 0x55, 0x55, 0x64));
                }
            }
            scrollTimer += 0.25f;
        }

        switch (gOptions.m_state) {
        case ProgramState::FadeInText: {
            u8 alpha = std::min(255.0f, fadeTimer);
            GRRLIB_DrawImg((rmode->fbWidth / static_cast<float>(2)) - (icon->w / 2),
                           (rmode->xfbHeight / static_cast<float>(2)) - (icon->h / 2), icon, 0, 1, 1,
                           util::getColour(0xFF, 0xFF, 0xFF, alpha));

            if (alpha == 255) {
                static u32 logoTimer = 0;
                if (logoTimer > 64) {
                    fadeTimer        = 255;
                    gOptions.m_state = ProgramState::FadeOutText;
                }
                logoTimer += 1;
                break;
            }

            fadeTimer += 2.5f;
            break;
        }
        case ProgramState::FadeOutText: {
            u8 alpha = std::max(0.0f, fadeTimer);
            GRRLIB_DrawImg((rmode->fbWidth / static_cast<float>(2)) - (icon->w / 2),
                           (rmode->xfbHeight / static_cast<float>(2)) - (icon->h / 2), icon, 0, 1, 1,
                           util::getColour(0xFF, 0xFF, 0xFF, alpha));

            if (alpha == 0) {
                gOptions.m_state = ProgramState::MainMenu;
            }

            fadeTimer -= 2.5f;
            break;
        }
        case ProgramState::MainMenu: {
            constexpr f32 scale = 0.5f;
            /* Place image in the middle of the screen */
            const u32 xpos = (rmode->fbWidth / static_cast<float>(2)) - ((icon->w * scale) / 2);
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
                const MenuItem& selected = mainMenu.getSelected();

                switch ((MainMenuItems)selected.m_index) {
                case MainMenuItems::Start:
                    gOptions.m_state = ProgramState::MainGame;
                    gSceneGenerator.setup();
                    break;
                case MainMenuItems::Options:
                    gOptions.m_state = ProgramState::Options;
                    break;
                case MainMenuItems::Extra:
                    gOptions.m_state = ProgramState::Extras;
                    break;
                case MainMenuItems::Quit:
                    gExit = true;
                    break;
                default:
                    break;
                }
            }

            break;
        }
        case ProgramState::Options: {
            for (MenuItem& item : optionsMenu.getItems()) {
                char text[0x20];
                memset(text, '\0', 0x20);

                switch ((OptionMenuItems)item.m_index) {
                case OptionMenuItems::ObjectCount:
                    sprintf(text, "Object count: [%d / %d]", gOptions.m_sceneObjCount.m_x,
                            gOptions.m_sceneObjCount.m_y);
                    break;
                case OptionMenuItems::WireframeObjectCount:
                    sprintf(text, "WF obj count: [%d / %d]", gOptions.m_wfObjCount.m_x, gOptions.m_wfObjCount.m_y);
                    break;
                case OptionMenuItems::LightCount:
                    sprintf(text, "Light count: [%d / %d]", gOptions.m_lightCount.m_x, gOptions.m_lightCount.m_y);
                    break;
                case OptionMenuItems::SpawnMode: {
                    char type[0x10];

                    switch (gOptions.m_spawnMode) {
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

                    sprintf(text, "Spawn type: [%s]", type);
                    break;
                }
                default:
                    break;
                }

                item.m_text = text;
                item.render(gFont);
            }

            if (btns_down & WPAD_BUTTON_UP) {
                optionsMenu.moveSelected(MenuDirection::Up);
            } else if (btns_down & WPAD_BUTTON_DOWN) {
                optionsMenu.moveSelected(MenuDirection::Down);
            }

            if (btns_down & WPAD_BUTTON_B) {
                gOptions.m_state = ProgramState::MainMenu;
            }

            if (btns_down & WPAD_BUTTON_RIGHT) {
                const MenuItem& item = optionsMenu.getSelected();
                switch (item.m_index) {
                case OptionMenuItems::ObjectCount:
                    gOptions.toggleOptionObjectCount(true);
                    break;
                case OptionMenuItems::WireframeObjectCount:
                    gOptions.toggleOptionWfObjCount(true);
                    break;
                case OptionMenuItems::SpawnMode:
                    gOptions.toggleOptionSpawnMode(true);
                    break;
                case OptionMenuItems::LightCount:
                    gOptions.toggleOptionLightCount(true);
                    break;
                default:
                    break;
                }
            } else if (btns_down & WPAD_BUTTON_LEFT) {
                const MenuItem& item = optionsMenu.getSelected();
                switch ((OptionMenuItems)item.m_index) {
                case OptionMenuItems::ObjectCount:
                    gOptions.toggleOptionObjectCount(false);
                    break;
                case OptionMenuItems::WireframeObjectCount:
                    gOptions.toggleOptionWfObjCount(false);
                    break;
                case OptionMenuItems::SpawnMode:
                    gOptions.toggleOptionSpawnMode(false);
                    break;
                case OptionMenuItems::LightCount:
                    gOptions.toggleOptionLightCount(false);
                    break;
                default:
                    break;
                }
            }
            break;
        }

        case ProgramState::Extras: {
            for (MenuItem& item : extraMenu.getItems()) {
                item.render(gFont);
            }

            if (btns_down & WPAD_BUTTON_UP) {
                extraMenu.moveSelected(MenuDirection::Up);
            } else if (btns_down & WPAD_BUTTON_DOWN) {
                extraMenu.moveSelected(MenuDirection::Down);
            }

            if (btns_down & WPAD_BUTTON_A) {
                MenuItem& selected = extraMenu.getSelected();

                switch ((ExtraMenuItems)selected.m_index) {
                case ExtraMenuItems::ChangeLog:
                    gOptions.m_state = ProgramState::ChangeLog;
                    break;
                case ExtraMenuItems::Controls:
                    gOptions.m_state = ProgramState::Controls;
                    break;
                case ExtraMenuItems::GX_S55_1:
                    gOptions.m_state = ProgramState::GX_S55_S1;
                    break;
                case ExtraMenuItems::GX_S55_2:
                    gOptions.m_state = ProgramState::GX_S55_S2;
                    break;
                default:
                    break;
                }
            }

            if (btns_down & WPAD_BUTTON_B) {
                gOptions.m_state = ProgramState::MainMenu;
            }
            break;
        }
        case ProgramState::Controls: {
            if (btns_down & WPAD_BUTTON_B) {
                gOptions.m_state = ProgramState::Extras;
            }

            gFont.printf(64, 64, "Game Controls", 46, util::white);
            gFont.printf(64, 96 + 28 * 1, "1) 1 - hide UI", 26, util::white);
            gFont.printf(64, 96 + 28 * 2, "   (after hiding UI)", 26, util::white);
            gFont.printf(64, 96 + 28 * 3, "2) 2 - regenerate scene", 26, util::white);
            gFont.printf(64, 96 + 28 * 4, "3) A - stop Camera rotation", 26, util::white);

            gFont.printf(64, 244, "Menu-specific Controls", 46, util::white);
            gFont.printf(64, (244 + 32) + 28 * 1, "1) DPAD Up / Down    - change selection", 26, util::white);
            gFont.printf(64, (244 + 32) + 28 * 2, "2) DPAD Right / Left - modify selection", 26, util::white);
            gFont.printf(64, (244 + 32) + 28 * 3, "3) A - select", 26, util::white);
            gFont.printf(64, (244 + 32) + 28 * 4, "4) B - back", 26, util::white);

            break;
        }
        case ProgramState::ChangeLog: {
            if (btns_down & WPAD_BUTTON_B) {
                gOptions.m_state = ProgramState::Extras;
            }

            gFont.printf(64, 64, "What's new?", 56,
                         util::getColour(abs(sin(gTimer / 10)) * 255, abs(cos(gTimer / 10)) * 255, 0));
            gFont.printf(64, 96 + 28 * 1, "Version 1.1", 36, util::white);
            gFont.printf(64, 96 + 32 * 2, "- Added extras, changelog and controls", 26, util::white);
            gFont.printf(64, 96 + 32 * 3, "  screen", 26, util::white);
            gFont.printf(64, 96 + 32 * 4, "- Added a pause button in the main scene", 26, util::white);
            gFont.printf(64, 96 + 32 * 5, "- Added scenes from an old project (GX_S55)", 26, util::white);
            gFont.printf(64, 96 + 32 * 6, "- Internal code clean-up", 26, util::white);

            break;
        }
        case ProgramState::GX_S55_S1: {
            if (btns_down & WPAD_BUTTON_B) {
                gOptions.m_state = ProgramState::Extras;
            }

            static f32 gxTimer      = 0;
            const f32 xyBase        = (sin(gxTimer / 17.5f) * 20);
            gMainCamera->position() = { xyBase - 20, xyBase - 10, static_cast<f32>((cos(gxTimer / 17.5) * 20) - 20) };
            gMainCamera->lookAt()   = { 0, 0, 0 };
            gMainCamera->applyCamera();

            GRRLIB_SetLightAmbient(util::black);
            GRRLIB_SetLightDiff(0, gMainCamera->position(), 5, 2, util::getColour(0xFF, 0xAA, 0x00));

            for (u32 lanes = 0; lanes < 10; lanes++) {
                for (u32 i = 0; i < 15; i++) {
                    GRRLIB_ObjectViewInv(lanes * 1.5f, 0, i * 2 / lanes, 0, 0, gxTimer * (3 + 1.15f * i), 1, 1, 1);
                    GRRLIB_DrawCube(1, true, 0xFFFFFFFF);
                }
            }
            gxTimer += 0.05f;

            break;
        }
        case ProgramState::GX_S55_S2: {
            if (btns_down & WPAD_BUTTON_B) {
                gOptions.m_state = ProgramState::Extras;
            }

            static f32 cameraRotation  = 0;
            static f32 cameraYOffset   = 7.5f;
            constexpr f32 offset       = 20.0f;
            constexpr f32 cameraOffset = 25.0f;
            if (btns_held & WPAD_BUTTON_LEFT) {
                cameraRotation -= 0.025f;
            } else if (btns_held & WPAD_BUTTON_RIGHT) {
                cameraRotation += 0.025f;
            }

            if (btns_held & WPAD_BUTTON_UP) {
                cameraYOffset = std::min(cameraYOffset + 0.1f, 42.5f);
            } else if (btns_held & WPAD_BUTTON_DOWN) {
                cameraYOffset = std::max(cameraYOffset - 0.1f, 2.5f);
            }

            gMainCamera->lookAt()   = { offset, 0, offset };
            gMainCamera->position() = { static_cast<f32>(offset + sin(cameraRotation) * cameraOffset), cameraYOffset,
                                        static_cast<f32>(offset + cos(cameraRotation) * cameraOffset) };
            gMainCamera->applyCamera();

            GRRLIB_SetLightAmbient(0x000000FF);

            guVector lightPos
                = { static_cast<f32>(abs(sin(gTimer / 4) * cameraOffset)), static_cast<f32>(8 + abs(cos(gTimer))),
                    static_cast<f32>(abs(cos(gTimer / 4) * cameraOffset)) };
            GRRLIB_ObjectViewInv(lightPos.x, lightPos.y - 2, lightPos.z, 0, 0, 0, 1, 1, 1);
            GRRLIB_DrawSphere(1, 10, 10, true, util::white);
            GRRLIB_SetLightDiff(0, lightPos, 10, 10, 0x623499FF);

            static float timer = 0;
            const f32 stride   = 2;
            for (u32 i = 0; i < 20; i++) {
                for (u32 j = 0; j < 20; j++) {
                    guVector pos = { i * stride, 0, j * stride };

                    if (i == 10 && j == 10) {
                        GRRLIB_ObjectViewInv(pos.x, 3, pos.z, 0, 0, 0, 1, 1, 1);
                        GRRLIB_DrawCube(1, true, util::red);
                    }

                    pos.y += sin((gTimer * j) / 10) / 5;
                    pos.y += cos((gTimer * i) / 10) / 10;

                    GRRLIB_ObjectViewInv(pos.x, pos.y, pos.z, 0, 0, 0, 1, 1, 1);
                    GRRLIB_DrawCylinder(
                        1, 1.5f, 10, i % static_cast<u32>(timer) == 0 || j % static_cast<u32>(timer) == 0, util::white);
                }
            }
            timer += 0.05f;
            if (timer > 19.5f) {
                timer = 0.5f;
            }

            break;
        }

        case ProgramState::MainGame: {
            static f32 camRotTimer = 0;
            static guVector camPos;
            if (!(!gOptions.m_showUI && btns_held & WPAD_BUTTON_A)) {
                camPos.x = sin(camRotTimer * 0.05f) * 20;
                camPos.z = cos(camRotTimer * 0.05f) * 20;
                camPos.y = 10 + sin(camRotTimer * 0.01f) * 5;
                camRotTimer += 0.05f;
            }

            gMainCamera->position() = camPos;
            gMainCamera->lookAt()   = { 0, 0, 0 };
            gMainCamera->applyCamera();

            gSceneGenerator.render();

            GRRLIB_2dMode();

            if (gOptions.m_showUI) {
                GRRLIB_Rectangle(32, 36, 226, 120, util::getColour(0x44, 0x44, 0x44), true);
                for (MenuItem& item : gameMenu.getItems()) {
                    item.render(gFont);
                }

                if (btns_down & WPAD_BUTTON_UP) {
                    gameMenu.moveSelected(MenuDirection::Up);
                } else if (btns_down & WPAD_BUTTON_DOWN) {
                    gameMenu.moveSelected(MenuDirection::Down);
                }

                if (btns_down & WPAD_BUTTON_A) {
                    const MenuItem& item = gameMenu.getSelected();

                    switch (item.m_index) {
                    case 0:
                        gSceneGenerator.setup();
                        break;
                    case 1:
                        for (auto& obj : gSceneGenerator.m_objects) {
                            obj.rngScaling();
                        }
                        for (auto& obj : gSceneGenerator.m_wfObjects) {
                            obj.rngScaling();
                        }
                        break;
                    case 2:
                        for (auto& obj : gSceneGenerator.m_objects) {
                            obj.rngColour();
                        }
                        for (auto& obj : gSceneGenerator.m_wfObjects) {
                            obj.rngColour();
                        }
                        break;
                    case 3:
                        gSceneGenerator.randomiseLights();
                        break;
                    default:
                        break;
                    }
                }
            } else if (btns_down & WPAD_BUTTON_2) {
                gSceneGenerator.setup();
            }

            if (btns_down & WPAD_BUTTON_1) {
                gOptions.m_showUI = !gOptions.m_showUI;
            }

            if (btns_down & WPAD_BUTTON_B) {
                gOptions.m_state = ProgramState::MainMenu;
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