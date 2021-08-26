#ifndef _MENU_H
#define _MENU_H

#include "math/vector2.h"
#include "util/colour.h"
#include "util/font.h"
#include <gctypes.h>
#include <vector>

struct MenuItem {
    u32 m_index = 0;

    Vector2<u32> m_position = { 0, 0 };

    const char* m_text;
    u32 m_textSize = 12; // aka fontSize

    u32 m_defaultCol = cl::white;
    u32 m_selectedCol = cl::red;

    bool m_selected = false;
    bool m_selectable = true;

    inline MenuItem(u32 idx, Vector2<u32> pos, const char* text, u32 textSize, u32 defaultCol, u32 selectedCol, bool selectable = true)
        : m_index(idx)
        , m_position(pos)
        , m_text(text)
        , m_textSize(textSize)
        , m_defaultCol(defaultCol)
        , m_selectedCol(selectedCol)
        , m_selectable(selectable)
    {
    }

    inline void render(Font& font)
    {
        font.printf(m_position.m_x, m_position.m_y, m_text, m_textSize, m_selected ? m_selectedCol : m_defaultCol);
    }
};

enum class MenuDirection : u8 {
    Up = 0,
    Down,
};

class Menu {
public:
    Menu() = default;
    ~Menu() = default;

    const std::vector<MenuItem>& getItems() const { return m_items; }
    std::vector<MenuItem>& getItems() { return m_items; }

    const s32 getSelectedIndex() const { return m_curIdx; }
    s32& getSelectedIndex() { return m_curIdx; }

    const MenuItem& getSelected() const { return m_items[m_curIdx]; }
    MenuItem& getSelected() { return m_items[m_curIdx]; }

    void moveSelected(MenuDirection);
    void reset(s32 idxResetTo);

private:
    std::vector<MenuItem> m_items;
    s32 m_curIdx = 0;
};

#endif