#ifndef _COLOUR_H
#define _COLOUR_H

#include <grrlib.h>

namespace util {
// Wrapper for RGBA macro
static inline constexpr u32 getColour(u8 r, u8 g, u8 b, u8 a = 0xFF) { return RGBA(r, g, b, a); }

static constexpr u32 white = getColour(0xFF, 0xFF, 0xFF);
static constexpr u32 black = getColour(0x00, 0x00, 0x00);

// Primary colours
static constexpr u32 red   = getColour(0xFF, 0x00, 0x00);
static constexpr u32 green = getColour(0x00, 0xFF, 0x00);
static constexpr u32 blue  = getColour(0x00, 0x00, 0xFF);

// Secondary colours
static constexpr u32 magenta = getColour(0xFF, 0xFF, 0x00);
static constexpr u32 cyan    = getColour(0x00, 0xFF, 0xFF);
static constexpr u32 yellow  = getColour(0xFF, 0x00, 0xFF);

static constexpr u32 clear = getColour(0x00, 0x00, 0x00, 0x00);
} // namespace util

#endif