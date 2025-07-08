#include <freetype2/ft2build.h>
#include <freetype2/freetype/ftglyph.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>

struct Character {
    float u0, v0, u1, v1;  // UV coordinates
    int width, height;
};

struct TextureAtlas {
    int width, height;
    std::vector<unsigned char> data;  // grayscale bitmap
    std::map<char, Character> chars;
};

class AtlasLoader {
public:
    void createTextureAtlas(TextureAtlas& atlas, const std::string& fontPath, int fontSize, int atlasWidth, int atlasHeight) {
        FT_Library ft;
        FT_Face face;

        if (FT_Init_FreeType(&ft)) {
            std::cerr << "Could not init FreeType library\n";
            return;
        }

        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            std::cerr << "Could not load font: " << fontPath << "\n";
            FT_Done_FreeType(ft);
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        atlas = { atlasWidth, atlasHeight, std::vector<unsigned char>(atlasWidth * atlasHeight, 0), {} };

        int offsetX = 0, offsetY = 0;

        for (unsigned char ch = 32; ch < 128; ++ch) {
            FT_UInt glyph_index = FT_Get_Char_Index(face, ch);
            if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER)) {
                std::cerr << "Could not load glyph: " << ch << "\n";
                continue;
            }

            FT_Bitmap& bitmap = face->glyph->bitmap;
            packCharacter(atlas, bitmap, ch, offsetX, offsetY);
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

private:
    void packCharacter(TextureAtlas& atlas, FT_Bitmap& bitmap, char ch, int& offsetX, int& offsetY) {
        if (offsetX + bitmap.width > atlas.width) {
            offsetX = 0;
            offsetY += bitmap.rows;
        }

        if (offsetY + bitmap.rows > atlas.height) {
            std::cerr << "Atlas is too small to fit all characters!\n";
            return;
        }

        for (int y = 0; y < bitmap.rows; ++y) {
            for (int x = 0; x < bitmap.width; ++x) {
                int dstIndex = (offsetY + y) * atlas.width + (offsetX + x);
                int srcIndex = y * bitmap.pitch + x;
                atlas.data[dstIndex] = bitmap.buffer[srcIndex];
            }
        }

        Character character = {
            (float)offsetX / atlas.width,
            (float)offsetY / atlas.height,
            (float)(offsetX + bitmap.width) / atlas.width,
            (float)(offsetY + bitmap.rows) / atlas.height,
            bitmap.width,
            bitmap.rows
        };

        atlas.chars[ch] = character;
        offsetX += bitmap.width;
    }
};


