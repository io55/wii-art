#include "menu.h"

void Menu::moveSelected(MenuDirection direction)
{
    m_items[m_curIdx].m_selected = false;

    switch (direction) {
    case MenuDirection::Up:
        m_curIdx--;
        break;
    case MenuDirection::Down:
        m_curIdx++;
        break;
    default:
        break;
    }

    if (m_curIdx == -1) {
        m_curIdx = m_items.size() - 1;
        /* Traverse backwards in the menu until we find a selectable item */
        while (!getSelected().m_selectable) {
            m_curIdx--;
        }
    } else if (m_curIdx == static_cast<s32>(m_items.size()) || !m_items[m_curIdx].m_selectable) {
        m_curIdx = 0;
    }

    m_items[m_curIdx].m_selected = true;
}

void Menu::reset(s32 idxResetTo)
{
    m_items[m_curIdx].m_selected = false;

    m_items[idxResetTo].m_selected = true;
    m_curIdx                       = idxResetTo;
}