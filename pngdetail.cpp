/*
LodePNG pngdetail

Copyright (c) 2005-2015 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

//g++ lodepng_util.cpp lodepng.cpp pngdetail.cpp -ansi -pedantic -Wall -Wextra -o pngdetail -O3


/*
Utility program that shows a lot of information in the console about a PNG file,
including color type, text chunks, the names and sizes of all chunks in the
image, all the zlib compression blocks and symbols, etc...

compression info:
./pngdetail -sfczB image.png

everything, 8-bit:
./pngdetail -sPlLA#cfzB7 image.png

everything, 16-bit:
./pngdetail -sPlLA@cfzB7 image.png

everything except huge output:
./pngdetail -sPlAcfzB image.png
*/

#include "lodepng.h"
#include "lodepng_util.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <algorithm>

struct Options
{
  bool show_png_summary; //show filesize, pixels and color type on single line
  bool show_png_info; //show things like filesize, width, height, palette size, ...
  bool show_extra_png_info; //in addition to show_png_info, show extra info
  bool show_palette; //show all palette values
  bool show_palette_pixels; //show palette indices of pixels
  bool show_ascii_art; //show ascii art image of the whole PNG
  int ascii_art_size;
  bool show_colors_hex; //show all pixel colors in RGBA CSS hex format
  bool show_colors_hex_16; //show all pixels colors with 16-bit per channel RGBA info
  bool show_chunks; //show the PNG chunk names and their lengths
  bool show_chunks2; //alternate form to print chunks
  bool show_filters; //show the PNG filter of each scanline (not supported for interlaced PNGs currently)
  bool zlib_info; //show basic zlib info
  bool zlib_blocks; //show type, tree info, code length summaries and sizes for each zlib block
  bool zlib_counts; //in addition to the zlib_blocks info, show counts of occurrences all symbols
  bool zlib_full; //in addition to the zlib_blocks info, show all symbols, one per line (huge output)
  bool use_hex; //show some sizes or positions in hexadecimal

  Options() : show_png_summary(false), show_png_info(false), show_extra_png_info(false),
              show_palette(false), show_palette_pixels(false),
              show_ascii_art(false), ascii_art_size(40), show_colors_hex(false), show_colors_hex_16(false),
              show_chunks(false), show_chunks2(false), show_filters(false),
              zlib_info(false), zlib_blocks(false), zlib_counts(false), zlib_full(false), use_hex(false)
  {
  }
};

std::string colorTypeString(LodePNGColorType type)
{
  std::string name;
  switch(type)
  {
    case LCT_GREY: name = "grey"; break;
    case LCT_RGB: name = "rgb"; break;
    case LCT_PALETTE: name = "palette"; break;
    case LCT_GREY_ALPHA: name = "grey with alpha"; break;
    case LCT_RGBA: name = "rgba"; break;
    default: name = "invalid"; break;
  }
  std::stringstream ss;
  ss << type << " (" << name << ")";
  return ss.str();
}

/*
Display general info about the PNG.
*/
void displayPNGInfo(const LodePNGInfo& info, const Options& options)
{
  const LodePNGColorMode& color = info.color;

  if(options.show_extra_png_info)
  {
    std::cout << "Compression method: " << info.compression_method << std::endl;
    std::cout << "Filter method: " << info.filter_method << std::endl;
  }
  std::cout << "Color type: " << colorTypeString(color.colortype) << std::endl;
  std::cout << "Bit depth: " << color.bitdepth << std::endl;
  if(options.show_extra_png_info)
  {
    std::cout << "Bits per pixel: " << lodepng_get_bpp(&color) << std::endl;
    std::cout << "Channels per pixel: " << lodepng_get_channels(&color) << std::endl;
    std::cout << "Is greyscale type: " << lodepng_is_greyscale_type(&color) << std::endl;
    std::cout << "Can have alpha: " << lodepng_can_have_alpha(&color) << std::endl;
  }
  if (!options.show_palette) std::cout << "Palette size: " << color.palettesize << std::endl;
  if(options.show_extra_png_info) std::cout << "Has color key: " << color.key_defined << std::endl;
  if(color.key_defined)
  {
    std::cout << "Color key rgb: " << color.key_r
              << ", " << color.key_g
              << ", " << color.key_b << std::endl;
  }
  if(info.background_defined)
  {
    if(color.colortype == LCT_PALETTE)
    {
      std::cout << "Background index: " << info.background_r << std::endl;
    }
    else
    {
      std::cout << "Background rgb: " << info.background_r
                << ", " << info.background_g
                << ", " << info.background_b << std::endl;
    }
  }
  std::cout << "Interlace method: " << info.interlace_method << std::endl;
  if(options.show_extra_png_info) std::cout << "Texts: " << info.text_num << std::endl;
  for(size_t i = 0; i < info.text_num; i++)
  {
    std::cout << "Text: " << info.text_keys[i] << ": " << info.text_strings[i] << std::endl;
  }
  if(options.show_extra_png_info) std::cout << "International texts: " << info.itext_num << std::endl;
  for(size_t i = 0; i < info.itext_num; i++)
  {
    std::cout << "Text: "
              << info.itext_keys[i] << ", "
              << info.itext_langtags[i] << ", "
              << info.itext_transkeys[i] << ": "
              << info.itext_strings[i] << std::endl;
  }
  if(options.show_extra_png_info) std::cout << "Time defined: " << info.time_defined << std::endl;
  if(info.time_defined)
  {
    const LodePNGTime& time = info.time;
    std::cout << "year: " << time.year << std::endl;
    std::cout << "month: " << time.month << std::endl;
    std::cout << "day: " << time.day << std::endl;
    std::cout << "hour: " << time.hour << std::endl;
    std::cout << "minute: " << time.minute << std::endl;
    std::cout << "second: " << time.second << std::endl;
  }
  if(options.show_extra_png_info) std::cout << "Physics defined: " << info.phys_defined << std::endl;
  if(info.phys_defined)
  {
    std::cout << "physics X: " << info.phys_x << std::endl;
    std::cout << "physics Y: " << info.phys_y << std::endl;
    std::cout << "physics unit: " << info.phys_unit << std::endl;
  }
}


/*
Display the names and sizes of all chunks in the PNG file.
*/
void displayChunkNames(const std::vector<unsigned char>& buffer, const Options& options)
{
  std::vector<std::string> names;
  std::vector<size_t> sizes;
  unsigned error = lodepng::getChunkInfo(names, sizes, buffer);
  if(error) std::cout << "Error while identifying chunks. Listing identified chunks anyway." << std::endl;

  if(options.show_chunks2)
  {
    std::cout << "Chunk types: ";
    for(size_t i = 0; i < names.size(); i++) std::cout << names[i] << " ";
    std::cout << std::endl;
    std::cout << "Chunk sizes: ";
    for(size_t i = 0; i < sizes.size(); i++) std::cout << sizes[i] << " ";
    std::cout << std::endl;
  }
  else
  {
    std::cout << "Chunks (type: lengths):";
    std::string last_type;
    for(size_t i = 0; i < names.size(); i++)
    {
      if(last_type != names[i])
      {
        std::cout << std::endl;
        std::cout << " " << names[i] << ": ";
      }
      last_type = names[i];

      std::cout << sizes[i] << " ";
    }
    std::cout << std::endl;
  }

  std::map<std::string, bool> typedict;
  for(size_t i = 0; i < names.size(); i++) {
    typedict[names[i]] = true;
  }

  if(!error) {
    if(!typedict["IHDR"]) std::cout << "Error: no IHDR chunk" << std::endl;
    if(!typedict["IDAT"]) std::cout << "Error: no IDAT chunk" << std::endl;
    if(!typedict["IEND"]) std::cout << "Error: no IEND chunk" << std::endl;
  }
}

void RGBtoHSL(unsigned char r, unsigned char g, unsigned char b, unsigned char* h, unsigned char* s, unsigned char* l) {
  int cmax = std::max<int>(r, std::max<int>(g, b));
  int cmin = std::min<int>(r, std::min<int>(g, b));
  if(cmin == cmax) {
    *h = *s = 0;
    *l = r;
  } else {
    int sum = cmin + cmax;
    int diff = cmax - cmin;
    *l = sum / 2;
    *s = 255 * diff / ((*l < 128) ? sum : (512 - sum));
    int hi = (r == cmax) ? (255 * (g - b) / diff) : ((g == cmax) ? (512 + 255 * (b - r) / diff) : (1024 + 255 * (r - g) / diff));
    *h = ((hi / 6) & 255);
  }
}

/*
HCT: Hue, Chroma, Tone: returns a linear combination between a pure hue and a greyscale value.
*) Chroma: The linear combination factor: 255 for pure hue, 0 for pure greyscale
*) Tone: greyscale to mix with: 0 = black (shade), 255 = white (tint), in between = grey (tone)
*/
void RGBtoHCT(unsigned char r, unsigned char g, unsigned char b, unsigned char* h, unsigned char* c, unsigned char* t) {
  int cmax = std::max<int>(r, std::max<int>(g, b));
  int cmin = std::min<int>(r, std::min<int>(g, b));
  RGBtoHSL(r, g, b, h, c, t);
  *c = cmax - cmin;
  *t = *c == 255 ? 0 : 255 * cmin / (255 + cmin - cmax);
}

// add 32 to get small letter instead of capital
char HueToLetter(int h) {
  char hl = 'R';
  // 12 unique hue letters for 30 degree increment hues.
  if(h < 11 || h >= 244) hl = 'R';  // red
  else if(h >= 11 && h < 32) hl = 'O';  // orange
  else if(h >= 32 && h < 53) hl = 'Y';  // yellow
  else if(h >= 53 && h < 74) hl = 'L';  // lime (officialy "chartreuse" but c is for cyan)
  else if(h >= 74 && h < 96) hl = 'G';  // green
  else if(h >= 96 && h < 117) hl = 'T';  // turquoise (officially "spring green" but that name overlaps green)
  else if(h >= 117 && h < 138) hl = 'C';  // cyan
  else if(h >= 138 && h < 159) hl = 'A';  // azure
  else if(h >= 159 && h < 181) hl = 'B';  // blue
  else if(h >= 181 && h < 202) hl = 'V';  // violet
  else if(h >= 202 && h < 223) hl = 'M';  // magenta
  else if(h >= 223 && h < 244) hl = 'F';  // fuchsia (officially "rose" but r is for red)
  return hl;
}

char lightnessToLetter(int l) {
  int c = ' ';
  if(l < 16) c = ' ';
  else if(l < 48) c = '.';
  else if(l < 80) c = ':';
  else if(l < 112) c = '-';
  else if(l < 144) c = '!';
  else if(l < 176) c = '*';
  else if(l < 208) c = '+'; // The + looks denser than the * in a terminal...
  else if(l < 240) c = '=';
  else c = '#';
  return c;
}

// Both v and result are assumed in range 0-255
// range is the size of an individual bucket. A value in roughly range [-range/2, range/2) can get added to v.
// E.g. if there are 12 hue letters, give 255/12 = 21 as range
static inline int applyDither(int v, int range, int x, int y, bool wrap) {
  // ordered dithering pattern; ranges from 0-15, so multiply with 17 to have 0-255
  static const int pattern[16] = {0,8,2,10,
                                  12,4,14,6,
                                  3,11,1,9,
                                  15,7,13,5};
  int d = pattern[(x & 3) + 4 * (y & 3)] * 17 - 128; // range: -128 to 127
  if(wrap) return (v + d * range / 256) & 255;
  else return std::max(0, std::min(255, v + d * range / 256));
}

// x and y are to use for dithering
// inverted inverts black and white, for in case black text on white background is used (by default it assumes white text on black background)
char RGBtoLetter(unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned x, unsigned y, bool dither = true, bool inverted = false) {
  if(a < 255) {
    r = a * r / 255;
    g = a * g / 255;
    b = a * b / 255;
  }

  if(dither) {
    unsigned char h, c, t;
    RGBtoHCT(r, g, b, &h, &c, &t);
    int l = (std::max(std::max(r, g), b) + std::min(std::min(r, g), b)) / 2;
    if(inverted) {
      l = 255 - l;
      t = 255 - t;
    }
    if(applyDither(c, 254, x, y, false) >= 128) {
      char letter = HueToLetter(applyDither(h, 21, x, y, true));
      bool smallcaps = applyDither(l, 64, x+2, y+2, false) < 80;
      return letter + (smallcaps ? 32 : 0);
    }
    else return lightnessToLetter(applyDither(l, 31, x, y, false));
  } else {
    unsigned char h, s, l;
    RGBtoHSL(r, g, b, &h, &s, &l);
    if(inverted) l = 255 - l;

    char hl = HueToLetter(h);
    char c = ' ';
    if(l < 24 || l > 232 || s < 64) {
      c = lightnessToLetter(l);
    } else {
      if(l < 128) c = hl + 32;
      else c = hl;
    }
    return c;
  }
}

/*
Show ASCII art preview of the image
image is given in 16-bit big endian
*/
void displayAsciiArt(const std::vector<unsigned char>& image, unsigned w, unsigned h, unsigned asciiw)
{
  if(w > 0 && h > 0)
  {
    //std::cout << "ASCII Art Preview: " << std::endl;
    unsigned w2 = asciiw;
    if(w < w2) w2 = w;
    unsigned h2 = h * w2 / w;
    h2 = (h2 * 2) / 3; //compensate for non-square characters in terminal
    if(h2 > (w2 * 2)) h2 = w2 * 2; //avoid too large output

    std::cout << '+';
    for(unsigned x = 0; x < w2; x++) std::cout << '-';
    std::cout << '+' << std::endl;
    for(unsigned y = 0; y < h2; y++)
    {
      std::cout << "|";
      for(unsigned x = 0; x < w2; x++)
      {
        unsigned x2 = x * w / w2;
        unsigned y2 = y * h / h2;
        int r = image[y2 * w * 8 + x2 * 8 + 0];
        int g = image[y2 * w * 8 + x2 * 8 + 2];
        int b = image[y2 * w * 8 + x2 * 8 + 4];
        int a = image[y2 * w * 8 + x2 * 8 + 6];
        char symbol = RGBtoLetter(r, g, b, a, x, y, true, false);
        std::cout << (char)symbol;
      }
      std::cout << "|";
      std::cout << std::endl;
    }
    std::cout << '+';
    for(unsigned x = 0; x < w2; x++) std::cout << '-';
    std::cout << '+' << std::endl;
  }
}

//sixteen: print 16 bits per pixel
//alpha: print alpha channel
//input image ALWAYS given in 16-bit per channel RGBA
void displayColorsHex(const std::vector<unsigned char>& image, unsigned w, unsigned h, bool sixteen)
{
  std::ios_base::fmtflags flags = std::cout.flags();

  if(w > 0 && h > 0)
  {
    std::cout << "Colors (CSS RGBA hex format):" << std::endl;

    for(unsigned y = 0; y < h; y++)
    {
      std::cout.flags(flags); //print line numbers in hex or dec whatever it originally was
      std::cout << y << ":";
      for(unsigned x = 0; x < w; x++)
      {
        size_t index = y * w * 8 + x * 8;
        if (sixteen)
        {
          int r = image[index + 0] * 256 + image[index + 1];
          int g = image[index + 2] * 256 + image[index + 3];
          int b = image[index + 4] * 256 + image[index + 5];
          int a = image[index + 6] * 256 + image[index + 7];
          std::cout << std::hex << std::setfill('0') << " #" << std::setw(4) << r << std::setw(4) << g << std::setw(4) << b << std::setw(4) << a;
        }
        else
        {
          int r = image[index + 0];
          int g = image[index + 2];
          int b = image[index + 4];
          int a = image[index + 6];
          std::cout << std::hex << std::setfill('0') << " #" << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b << std::setw(2) << a;
        }
      }
      std::cout << std::endl;
    }
  }

  std::cout.flags(flags);
}


/*
Show the filtertypes of each scanline in this PNG image.
*/
void displayFilterTypes(const std::vector<unsigned char>& buffer)
{
  std::vector<std::vector<unsigned char> > types;
  unsigned error = lodepng::getFilterTypesInterlaced(types, buffer);
  if(error)
  {
    std::cout << "Error getting filter types" << std::endl;
    return;
  }

  if(types.size() == 7)
  {
    std::cout << "Filter types (Adam7 interlaced):" << std::endl;
    for(int j = 0; j < 7; j++)
    {
      std::cout << " Pass " << (j + 1) << ": ";
      for(size_t i = 0; i < types[j].size(); i++)
      {
        std::cout << (int)(types[j][i]);
      }
      std::cout << std::endl;
    }
  }
  else
  {
    std::cout << "Filter types: ";
    for(size_t i = 0; i < types[0].size(); i++)
    {
      std::cout << (int)(types[0][i]);
    }
    std::cout << std::endl;
  }
}

//image type MUST be palette
void displayPalette(const std::vector<unsigned char>& buffer)
{
  unsigned w, h;
  lodepng::State state;
  std::vector<unsigned char> out;

  state.decoder.color_convert = 0;

  lodepng::decode(out, w, h, state, buffer);

  std::cout << "Palette size: " << state.info_png.color.palettesize << std::endl;
  std::cout << "Palette colors: ";
  std::ios_base::fmtflags flags = std::cout.flags();
  std::cout << std::hex << std::setfill('0');
  for(size_t i = 0; i < state.info_png.color.palettesize; i++)
  {
    unsigned char* p = &state.info_png.color.palette[i * 4];
    std::cout << "#" << std::setw(2) << (int)p[0] << std::setw(2) << (int)p[1] << std::setw(2) << (int)p[2] << std::setw(2) << (int)p[3] << " ";
  }
  std::cout.flags(flags);
  std::cout << std::endl;


  if (state.info_png.color.colortype == LCT_PALETTE)
  {
    std::vector<size_t> count(256, 0);
    size_t outofbounds = 0;

    for(size_t i = 0; i < w * h; i++)
    {
      int value = lodepng::getPaletteValue(&out[0], i, state.info_raw.bitdepth);
      count[value]++;
      if(value >= (int)state.info_raw.palettesize) outofbounds++;
    }

    std::cout << "Palette count: ";
    for(size_t i = 0; i < state.info_raw.palettesize; i++)
    {
      std::cout << count[i] << " ";
    }
    std::cout << std::endl;

    if(outofbounds > 0) std::cout << "Out of bounds palette values: " << outofbounds << std::endl;
  }
}

//image type MUST be palette
void displayPalettePixels(const std::vector<unsigned char>& buffer)
{
  unsigned w, h;
  lodepng::State state;
  std::vector<unsigned char> out;

  state.decoder.color_convert = 0;

  lodepng::decode(out, w, h, state, buffer);


  if (state.info_png.color.colortype == LCT_PALETTE)
  {
    std::cout << "Pixel palette indices:" << std::endl;
    for(size_t i = 0; i < w * h; i++)
    {
      int value = lodepng::getPaletteValue(&out[0], i, state.info_raw.bitdepth);
      std::cout << value << ", ";
      if(i % w == w - 1) std::cout << std::endl;
    }
  }
}

void printZlibInfo(const std::vector<unsigned char>& in, const Options& options)
{
  if(!options.zlib_info && !options.zlib_blocks) return;

  std::vector<lodepng::ZlibBlockInfo> zlibinfo;
  lodepng::extractZlibInfo(zlibinfo, in);

  if(options.zlib_info)
  {
    //std::cout << "Zlib info: " << std::endl;
    size_t compressed = 0;
    size_t uncompressed = 0;
    std::vector<size_t> boundaries_compressed;
    std::vector<size_t> boundaries_uncompressed;
    for(size_t i = 0; i < zlibinfo.size(); i++)
    {
      compressed += zlibinfo[i].compressedbits / 8;
      uncompressed += zlibinfo[i].uncompressedbytes;
      boundaries_compressed.push_back(compressed);
      boundaries_uncompressed.push_back(uncompressed);
    }

    std::cout << "Compressed size: " << compressed << std::endl;
    std::cout << "Uncompressed size: " << uncompressed << std::endl;
    std::cout << "Amount of zlib blocks: " << zlibinfo.size() << std::endl;
    if(zlibinfo.size() > 1)
    {
      std::cout << "Block sizes (uncompressed): ";
      for(size_t i = 0; i < zlibinfo.size(); i++)
          std::cout << zlibinfo[i].uncompressedbytes << " ";
      std::cout << std::endl;
      std::cout << "Block sizes (compressed): ";
      for(size_t i = 0; i < zlibinfo.size(); i++)
          std::cout << (zlibinfo[i].compressedbits / 8) << " ";
      std::cout << std::endl;
      std::cout << "Block boundaries (uncompressed): ";
      for(size_t i = 0; i + 1 < boundaries_uncompressed.size(); i++)
          std::cout << boundaries_uncompressed[i] << " ";
      std::cout << std::endl;
      std::cout << "Block boundaries (compressed): ";
      for(size_t i = 0; i + 1 < boundaries_compressed.size(); i++)
          std::cout << boundaries_compressed[i] << " ";
      std::cout << std::endl;
    }
  }

  if(options.zlib_blocks)
  {
    for(size_t i = 0; i < zlibinfo.size(); i++)
    {
      const lodepng::ZlibBlockInfo& info = zlibinfo[i];

      std::cout << "Zlib block " << i << ":" << std::endl;
      std::cout << " block type: " << info.btype << std::endl;

      size_t compressedsize = info.compressedbits / 8;
      size_t uncompressedsize = info.uncompressedbytes;
      std::cout << " block compressed: " << compressedsize << " (" << compressedsize / 1024 << "K) (" << info.compressedbits << " bits)" << std::endl;
      std::cout << " block uncompressed: " << uncompressedsize << " (" << uncompressedsize / 1024 << "K)" << std::endl;

      if(info.btype > 2)
      {
        std::cout << "Error: Invalid Block Type" << std::endl;
        return;
      }

      if(info.btype == 2)
      {
        std::cout << " encoded trees size: " << info.treebits / 8 << " (" << info.treebits << " bits)" << std::endl;
        std::cout << " HLIT: " << info.hlit << std::endl;
        std::cout << " HDIST: " << info.hdist << std::endl;
        std::cout << " HCLEN: " << info.hclen << std::endl;
        std::cout << std::hex;
        std::cout << " code length code lengths: "; for(size_t j = 0; j < 19; j++) std::cout << info.clcl[j]; std::cout << std::endl;
        if(!options.use_hex) std::cout << std::dec;
        if(options.zlib_full)
        {
          for(size_t j = 0; j < info.treecodes.size(); j++)
          {
            int code = info.treecodes[j];
            if(code < 17)
            {
               std::cout << " tree: " << code << std::endl;
            }
            else
            {
              j++;
              std::cout << " tree: " << code << " rep: " << info.treecodes[j] << std::endl;
            }

          }
        }

        std::cout << std::hex;
        std::cout << " lit code lengths 0-127  : "; for(size_t j = 0; j < 128; j++) std::cout << info.litlenlengths[j]; std::cout << std::endl;
        std::cout << " lit code lengths 128-255: "; for(size_t j = 128; j < 256; j++) std::cout << info.litlenlengths[j]; std::cout << std::endl;
        std::cout << " end code length         : "; std::cout << info.litlenlengths[256]; std::cout << std::endl;
        std::cout << " len code lengths        : "; for(size_t j = 257; j < 288; j++) std::cout << info.litlenlengths[j]; std::cout << std::endl;
        std::cout << " dist code lengths       : "; for(size_t j = 0; j < 32; j++) std::cout << info.distlengths[j]; std::cout << std::endl;
        if(!options.use_hex) std::cout << std::dec;
      }


      if(info.btype != 0)
      {
        std::cout << " code counts: lit: " << info.numlit << ", len/dist: " << info.numlen << ", total: " << (info.numlit + info.numlen + 1) << ", with dists: " << (info.numlit + 2 * info.numlen + 1) << std::endl;

        if(options.zlib_full)
        {
          for(size_t j = 0; j < info.lz77_lcode.size(); j++)
          {
            int symbol = info.lz77_lcode[j];
            if(symbol == 256)
            {
              std::cout << " end" << std::endl;
            }
            else if(symbol < 256)
            {
              std::cout << " lit: " << symbol << std::endl;
            }
            else
            {
              std::cout << " len: " << info.lz77_lvalue[j] << ", dist: " << info.lz77_dvalue[j] << std::endl;
            }
          }
        }

        if(options.zlib_counts)
        {
          std::vector<size_t> ll_count(288, 0);
          std::vector<size_t> d_count(32, 0);
          for(size_t j = 0; j < info.lz77_lcode.size(); j++)
          {
            int symbol = info.lz77_lcode[j];
            if(symbol <= 256)
            {
              ll_count[symbol]++;
            }
            else
            {
              ll_count[symbol]++;
              d_count[info.lz77_dcode[j]]++;
            }
          }
          std::cout << " lit code 0-63 counts   : "; for(size_t j = 0; j < 64; j++) std::cout << ll_count[j] << " "; std::cout << std::endl;
          std::cout << " lit code 64-127 counts : "; for(size_t j = 64; j < 128; j++) std::cout << ll_count[j] << " "; std::cout << std::endl;
          std::cout << " lit code 128-191 counts: "; for(size_t j = 128; j < 192; j++) std::cout << ll_count[j] << " "; std::cout << std::endl;
          std::cout << " lit code 192-255 counts: "; for(size_t j = 192; j < 256; j++) std::cout << ll_count[j] << " "; std::cout << std::endl;
          std::cout << " end code count         : "; std::cout << ll_count[256] << " "; std::cout << std::endl;
          std::cout << " len code counts        : "; for(size_t j = 257; j < 288; j++) std::cout << ll_count[j] << " "; std::cout << std::endl;
          std::cout << " dist code counts       : "; for(size_t j = 0; j < 32; j++) std::cout << d_count[j] << " "; std::cout << std::endl;
        }
      }
    }
  }
}

void showHelp()
{
  std::cout << "pngdetail by Lode Vandevenne\n"
               "Shows detailed information about a PNG image and its compression\n"
               "Usage: pngdetail [filename] [options]...\n"
               "Options:\n"
               "-s: show PNG file summary on one line\n"
               "-p: show PNG file info\n"
               "-P: show extra PNG file info\n"
               "-l: show palette (if any)\n"
               "-a: show ascii art rendering of PNG image. Letters ROYLGTCABVMF indicate hue (L=lime, T=turquoise, A=azure, F=fuchsia, ...).\n"
               "-A: show larger ascii art rendering of PNG image. Adding more A's makes it larger.\n"
               "-#: show every pixel color in CSS RGBA hex format (huge output)\n"
               "-@: show every pixel color with 16-bit per channel (huge output)\n"
               "-c: show PNG chunks\n"
               "-C: show PNG chunks (alternate format)\n"
               "-f: show PNG filters\n"
               "-z: show Zlib info\n"
               "-b: show Zlib blocks\n"
               "-B: show Zlib block symbol counts\n"
               "-7: show all lz77 values (huge output)\n"
               "-x: print most integer numbers in hexadecimal (includes e.g. year, num unique colors, ...)\n"
            << std::endl;
}

// number of unique RGBA colors in the image
// the input image is in 16-bit per channel color, so 8 chars per pixel
size_t countColors(std::vector<unsigned char> image, unsigned w, unsigned h) {
  struct ColorTree
  {
    ColorTree* children[16]; /*up to 16 pointers to ColorTree of next level*/
    int index; /*the payload. Only has a meaningful value if this is in the last level*/

    ColorTree()
    {
      int i;
      for(i = 0; i < 16; i++) children[i] = 0;
      index = -1;
    }

    ~ColorTree()
    {
      int i;
      for(i = 0; i < 16; i++)
      {
        if(children[i]) delete children[i];
      }
    }

    /*returns -1 if color not present, its index otherwise*/
    int get(unsigned short r, unsigned short g, unsigned short b, unsigned short a) const {
      const ColorTree* tree = this;
      int bit = 0;
      for(bit = 0; bit < 8; bit++)
      {
        int i = 8 * ((r >> bit) & 1) + 4 * ((g >> bit) & 1) + 2 * ((b >> bit) & 1) + 1 * ((a >> bit) & 1);
        if(!tree->children[i]) return -1;
        else tree = tree->children[i];
      }
      return tree ? tree->index : -1;
    }

    bool has(unsigned short r, unsigned short g, unsigned short b, unsigned short a) {
      return get(r, g, b, a) >= 0;
    }

    /*color is not allowed to already exist.
    Index should be >= 0 (it's signed to be compatible with using -1 for "doesn't exist")*/
    void add(unsigned short r, unsigned short g, unsigned short b, unsigned short a, int index) {
      ColorTree* tree = this;
      int bit;
      for(bit = 0; bit < 8; bit++)
      {
        int i = 8 * ((r >> bit) & 1) + 4 * ((g >> bit) & 1) + 2 * ((b >> bit) & 1) + 1 * ((a >> bit) & 1);
        if(!tree->children[i])
        {
          tree->children[i] = new ColorTree();
        }
        tree = tree->children[i];
      }
      tree->index = index;
    }
  };

  size_t count = 0;
  ColorTree tree;
  for(unsigned y = 0; y < h; y++) {
    for(unsigned x = 0; x < w; x++) {
      unsigned short r = 256 * image[y * 8 * w + x * 8 + 0] + image[y * 8 * w + x * 8 + 1];
      unsigned short g = 256 * image[y * 8 * w + x * 8 + 2] + image[y * 8 * w + x * 8 + 3];
      unsigned short b = 256 * image[y * 8 * w + x * 8 + 4] + image[y * 8 * w + x * 8 + 5];
      unsigned short a = 256 * image[y * 8 * w + x * 8 + 6] + image[y * 8 * w + x * 8 + 7];
      if(!tree.has(r, g, b, a)) {
        tree.add(r, g, b, a, count);
        count++;
      }
    }
  }
  return count;
}

unsigned showFileInfo(const std::string& filename, const Options& options)
{
  std::vector<unsigned char> buffer;
  std::vector<unsigned char> image;
  unsigned w, h;

  unsigned error = lodepng::load_file(buffer, filename); //load the image file with given filename

  if(error)
  {
    std::cout << "Loading error " << error << ": " << lodepng_error_text(error) << std::endl;
    return error;
  }

  lodepng::State state;
  state.info_raw.colortype = LCT_RGBA;
  state.info_raw.bitdepth = 16;
  error = lodepng::decode(image, w, h, state, buffer);

  // In case of checksum errors, disable checksums
  while (error == 57 || error == 58) {
    if(error == 57)
    {
      std::cout << "Error: invalid CRC checksum" << std::endl;
      state.decoder.ignore_crc = 1;
      error = lodepng::decode(image, w, h, state, buffer);
    }

    if(error == 58)
    {
      std::cout << "Error: invalid Adler32 checksum" << std::endl;
      state.decoder.zlibsettings.ignore_adler32 = 1;
      error = lodepng::decode(image, w, h, state, buffer);
    }
  }

  if(error)
  {
    std::cout << "Decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    // Do not return: some sections may still show partial info about a corrupted PNG.
  }

  bool extra_newlines = false;

  if(!error && options.show_png_summary)
  {
    std::cout << "Filesize: " << buffer.size() << " (" << buffer.size() / 1024 << "K)" << ", ";
    std::cout << w << "x" << h << ", ";
    std::cout << "Color: " << colorTypeString(state.info_png.color.colortype) << ", " << state.info_png.color.bitdepth << " bit" << std::endl;
    if(extra_newlines) std::cout << std::endl;
  }

  if(!error && options.show_png_info)
  {
    std::cout << "Filesize: " << buffer.size() << " (" << buffer.size() / 1024 << "K)" << std::endl;
    std::cout << "Width: " << w << std::endl;
    std::cout << "Height: " << h << std::endl;

    if(options.show_extra_png_info) std::cout << "Num pixels: " << w * h << std::endl;
    std::cout << "Num unique colors: " << countColors(image, w, h) << std::endl;
    if(options.show_extra_png_info && w > 0 && h > 0)
    {
      double r = 0, g = 0, b = 0, a = 0;
      for(unsigned y = 0; y < h; y++) {
        for(unsigned x = 0; x < w; x++) {
          r += 256 * image[y * 8 * w + x * 8 + 0] + image[y * 8 * w + x * 8 + 1];
          g += 256 * image[y * 8 * w + x * 8 + 2] + image[y * 8 * w + x * 8 + 3];
          b += 256 * image[y * 8 * w + x * 8 + 4] + image[y * 8 * w + x * 8 + 5];
          a += 256 * image[y * 8 * w + x * 8 + 6] + image[y * 8 * w + x * 8 + 7];
        }
      }
      r /= (w * h * 257.0);
      g /= (w * h * 257.0);
      b /= (w * h * 257.0);
      a /= (w * h * 257.0);
      std::cout << "Average color: " << r << ", " << g << ", " << b << ", " << a << std::endl;
    }

    displayPNGInfo(state.info_png, options);
    if(extra_newlines) std::cout << std::endl;
  }

  if(options.show_chunks || options.show_chunks2)
  {
    displayChunkNames(buffer, options);
    if(extra_newlines) std::cout << std::endl;
  }

  if(options.show_filters)
  {
    displayFilterTypes(buffer);
    if(extra_newlines) std::cout << std::endl;
  }

  if(options.show_palette)
  {
    displayPalette(buffer);
    if(extra_newlines) std::cout << std::endl;
  }

  if(options.show_palette_pixels)
  {
    displayPalettePixels(buffer);
    if(extra_newlines) std::cout << std::endl;
  }

  if (!error && options.show_ascii_art)
  {
    displayAsciiArt(image, w, h, options.ascii_art_size);
    if(extra_newlines) std::cout << std::endl;
  }

  if (!error && (options.show_colors_hex || options.show_colors_hex_16))
  {
    displayColorsHex(image, w, h, options.show_colors_hex_16);
    if(extra_newlines) std::cout << std::endl;
  }

  printZlibInfo(buffer, options);
  return 0;
}

int main(int argc, char *argv[])
{
  Options options;
  bool options_chosen = false;

  std::vector<std::string> filenames;
  for (int i = 1; i < argc; i++)
  {
    std::string s = argv[i];
    if(s[0] == '-' && s.size() > 1)
    {
      if(s != "-x") options_chosen = true; //only selecting hexadecimal is no choice, keep the defaults
      for(size_t j = 1; j < s.size(); j++)
      {
        char c = s[j];
        if(c == 'h')
        {
          showHelp();
          return 0;
        }
        else if(c == 's') options.show_png_summary = true;
        else if(c == 'p') options.show_png_info = true;
        else if(c == 'P')
        {
          options.show_png_info = true;
          options.show_extra_png_info = true;
        }
        else if(c == 'l') options.show_palette = true;
        else if(c == 'L') options.show_palette_pixels = true;
        else if(c == 'a') options.show_ascii_art = true;
        else if(c == 'A')
        {
          options.show_ascii_art = true;
          options.ascii_art_size += 40;
        }
        else if(c == '#') options.show_colors_hex = true;
        else if(c == '@') options.show_colors_hex_16 = true;
        else if(c == 'c') options.show_chunks = true;
        else if(c == 'C') options.show_chunks2 = true;
        else if(c == 'f') options.show_filters = true;
        else if(c == 'z') options.zlib_info = true;
        else if(c == 'b') options.zlib_blocks = true;
        else if(c == 'B')
        {
          options.zlib_blocks = true;
          options.zlib_counts = true;
        }
        else if(c == '7')
        {
          options.zlib_blocks = true;
          options.zlib_full = true;
        }
        else if(c == 'x')
        {
          options.use_hex = true;
          std::cout << std::hex;
        }
        else
        {
          std::cout << "Unknown flag: " << c << ". Use -h for help" << std::endl;
          return 0;
        }

      }
    }
    else filenames.push_back(s);
  }

  if(filenames.empty())
  {
    std::cout << "Please provide a filename to preview" << std::endl;
    showHelp();
    return 0;
  }

  if(!options_chosen)
  {
    //fill in defaults
    options.show_png_info = true;
    options.show_chunks = true;
    options.show_filters = true;
    options.zlib_info = true;
  }

  for(size_t i = 0; i < filenames.size(); i++)
  {
    if(filenames.size() > 1) std::cout << filenames[i] << std::endl;
    showFileInfo(filenames[i], options);
  }
}
