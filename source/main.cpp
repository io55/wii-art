#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include <algorithm>
#include <cmath>

#include "icon_jpg.h"
#include "menu_change_ogg.h"
#include "terminus_ttf.h"

#include "globals.h"
#include "math/camera.cpp"
#include "menu.h"
#include "util/colour.h"

enum class ProgramState : u8 {
    FadeInText,
    FadeOutText,

    Menu,

    MainGame
};

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

    Menu menu;
    std::vector<MenuItem>& items = menu.getItems();
    items.push_back({ 0, { 64, 241 }, "START", 46, cl::white, cl::red });
    items.push_back({ 1, { 64, 301 }, "OPTIONS", 46, cl::white, cl::red });
    items.push_back({ 2, { 64, 361 }, "EXIT", 46, cl::white, cl::red });
    menu.reset(0);

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

            for (auto& item : menu.getItems()) {
                item.m_position.m_x = xpos;
                item.render(gFont);
            }

            if (btns_down & WPAD_BUTTON_UP) {
                menu.moveSelected(MenuDirection::Up);
            } else if (btns_down & WPAD_BUTTON_DOWN) {
                menu.moveSelected(MenuDirection::Down);
            }

            if (btns_down & WPAD_BUTTON_A) {
                MenuItem& selected = menu.getSelected();

                // Start
                if (selected.m_index == 0) {
                    state = ProgramState::MainGame;
                }
                // Options
                else if (selected.m_index == 1) {
                }
                // Exit
                else if (selected.m_index == 2) {
                    gExit = true;
                }
            }

            break;
        }
        case ProgramState::MainGame: {
            if (btns_down & WPAD_BUTTON_HOME) {
                state = ProgramState::Menu;
                menu.reset(0);
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