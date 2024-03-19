#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library ft;

if (FT_Init_FreeType(&ft)){
	std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
}

FT_Face face;
if (FT_New_Face(ft, "fonts/OCR A Extended.ttf", 0, &face)) {
	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
}

FT_Set_Pixel_Sizes(face, 0, 48);