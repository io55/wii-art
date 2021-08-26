#ifndef _COLOUR_H
#define _COLOUR_H

#include <grrlib.h>

namespace util {
// Wrapper for RGBA macro
static inline constexpr u32 GetColour(u8 r, u8 g, u8 b, u8 a = 0xFF) { return RGBA(r, g, b, a); }

static constexpr u32 white = GetColour(0xFF, 0xFF, 0xFF);
static constexpr u32 black = GetColour(0x00, 0x00, 0x00);

// Primary colours
static constexpr u32 red = GetColour(0xFF, 0x00, 0x00);
static constexpr u32 green = GetColour(0x00, 0xFF, 0x00);
static constexpr u32 blue = GetColour(0x00, 0x00, 0xFF);

// Secondary colours
static constexpr u32 magenta = GetColour(0xFF, 0xFF, 0x00);
static constexpr u32 cyan = GetColour(0x00, 0xFF, 0xFF);
static constexpr u32 yellow = GetColour(0xFF, 0x00, 0xFF);

static constexpr u32 clear = GetColour(0x00, 0x00, 0x00, 0x00);
}

#endif