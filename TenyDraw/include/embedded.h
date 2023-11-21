#pragma once

#define ATLAS_WIDTH             869
#define ATLAS_HEIGHT            869

#define GLYPH_SIZE              36
#define NUMBER_GLYPHS_SECTION   109
#define NUMBER_GLYPHS_TOTAL     (NUMBER_GLYPHS_SECTION + 1)

#define NUMBER_FONTS            4
#define X16_OFFSET              (NUMBER_GLYPHS_SECTION * GLYPH_SIZE * 0)
#define X32_OFFSET              (NUMBER_GLYPHS_SECTION * GLYPH_SIZE * 1)
#define X48_OFFSET              (NUMBER_GLYPHS_SECTION * GLYPH_SIZE * 2)
#define X64_OFFSET              (NUMBER_GLYPHS_SECTION * GLYPH_SIZE * 3)
#define QUAD_OFFSET             (NUMBER_GLYPHS_TOTAL * GLYPH_SIZE)

const unsigned char* get_atlas_data();
const unsigned char* get_glyph_data();
