#ifndef _FONT_H
#define _FONT_H

#include <grrlib.h>

class Font {
public:
    Font() = default;

    inline Font(const u8* file_base, const u32 size)
    {
        m_font = GRRLIB_LoadTTF(file_base, size);
    }

    inline void init(const u8* file_base, const u32 size)
    {
        m_font = GRRLIB_LoadTTF(file_base, size);
    }

    inline ~Font()
    {
        GRRLIB_FreeTTF(m_font);
    }

    inline void printf(u32 x, u32 y, const char* msg, u32 size, u32 colour)
    {
        GRRLIB_PrintfTTF(x, y, m_font, msg, size, colour);
    }

    inline GRRLIB_ttfFont* getFont() const { return m_font; }
    inline GRRLIB_ttfFont*& getFont() { return m_font; }

private:
    GRRLIB_ttfFont* m_font = nullptr;
};

#endif