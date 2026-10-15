/*
LodePNG Encoder Fuzzer

Copyright (c) 2005-2024 Lode Vandevenne

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

// clang++ -fsanitize=fuzzer,address,undefined -DLODEPNG_MAX_ALLOC=100000000 \
//     lodepng.cpp lodepng_encode_fuzzer.cpp -O3 -o lodepng_encode_fuzzer && \
//     ./lodepng_encode_fuzzer

#include "lodepng.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

/* Valid raw color modes that do not require a palette. */
const size_t num_combinations = 11;

LodePNGColorType colortypes[num_combinations] = {
    LCT_GREY,       LCT_GREY,       LCT_GREY,       LCT_GREY,
    LCT_GREY,       /* 1, 2, 4, 8 or 16 bits */
    LCT_RGB,        LCT_RGB,        /* 8 or 16 bits */
    LCT_GREY_ALPHA, LCT_GREY_ALPHA, /* 8 or 16 bits */
    LCT_RGBA,       LCT_RGBA        /* 8 or 16 bits */
};

unsigned bitdepths[num_combinations] = {
    1, 2, 4, 8, 16, /* gray */
    8, 16,          /* rgb */
    8, 16,          /* gray + alpha */
    8, 16           /* rgb + alpha */
};

unsigned getByte(const uint8_t* data, size_t size, size_t index) {
  return data[index % size];
}

unsigned getDimension(const uint8_t* data, size_t size, size_t index) {
  /* Limit image size to keep encoder fuzzing fast. */
  return 1u + (getByte(data, size, index) % 32u);
}

size_t getRawSize(unsigned w,
                  unsigned h,
                  LodePNGColorType colortype,
                  unsigned bitdepth) {
  LodePNGColorMode mode;
  lodepng_color_mode_init(&mode);
  mode.colortype = colortype;
  mode.bitdepth = bitdepth;

  size_t raw_size = lodepng_get_raw_size(w, h, &mode);

  lodepng_color_mode_cleanup(&mode);
  return raw_size;
}

std::vector<unsigned char> makeImage(const uint8_t* data,
                                     size_t size,
                                     size_t raw_size,
                                     size_t offset) {
  std::vector<unsigned char> image(raw_size);

  for(size_t i = 0; i < raw_size; i++) {
    image[i] = static_cast<unsigned char>(getByte(data, size, i + offset));
  }

  return image;
}

void encodeMemory(const std::vector<unsigned char>& image,
                  unsigned w,
                  unsigned h,
                  LodePNGColorType colortype,
                  unsigned bitdepth) {
  unsigned char* out = 0;
  size_t outsize = 0;

  lodepng_encode_memory(&out,
                        &outsize,
                        image.empty() ? 0 : &image[0],
                        w,
                        h,
                        colortype,
                        bitdepth);

  std::free(out);
}

void encode24(const std::vector<unsigned char>& image,
              unsigned w,
              unsigned h) {
  unsigned char* out = 0;
  size_t outsize = 0;

  lodepng_encode24(&out,
                   &outsize,
                   image.empty() ? 0 : &image[0],
                   w,
                   h);

  std::free(out);
}

void encode32(const std::vector<unsigned char>& image,
              unsigned w,
              unsigned h) {
  unsigned char* out = 0;
  size_t outsize = 0;

  lodepng_encode32(&out,
                   &outsize,
                   image.empty() ? 0 : &image[0],
                   w,
                   h);

  std::free(out);
}

void encodeVector(const std::vector<unsigned char>& image,
                  unsigned w,
                  unsigned h,
                  LodePNGColorType colortype,
                  unsigned bitdepth) {
  std::vector<unsigned char> out;
  lodepng::encode(out, image, w, h, colortype, bitdepth);
}

void encodeState(const std::vector<unsigned char>& image,
                 unsigned w,
                 unsigned h,
                 LodePNGColorType colortype,
                 unsigned bitdepth) {
  std::vector<unsigned char> out;
  lodepng::State state;

  state.info_raw.colortype = colortype;
  state.info_raw.bitdepth = bitdepth;

  lodepng::encode(out, image, w, h, state);
}

void encodeInterlaced(const std::vector<unsigned char>& image,
                      unsigned w,
                      unsigned h,
                      LodePNGColorType colortype,
                      unsigned bitdepth) {
  std::vector<unsigned char> out;
  lodepng::State state;

  state.info_raw.colortype = colortype;
  state.info_raw.bitdepth = bitdepth;
  state.info_png.interlace_method = 1;

  lodepng::encode(out, image, w, h, state);
}

void encodeUncompressed(const std::vector<unsigned char>& image,
                        unsigned w,
                        unsigned h,
                        LodePNGColorType colortype,
                        unsigned bitdepth) {
  std::vector<unsigned char> out;
  lodepng::State state;

  state.info_raw.colortype = colortype;
  state.info_raw.bitdepth = bitdepth;
  state.encoder.zlibsettings.btype = 0;

  lodepng::encode(out, image, w, h, state);
}

void encodeNoLZ77(const std::vector<unsigned char>& image,
                  unsigned w,
                  unsigned h,
                  LodePNGColorType colortype,
                  unsigned bitdepth) {
  std::vector<unsigned char> out;
  lodepng::State state;

  state.info_raw.colortype = colortype;
  state.info_raw.bitdepth = bitdepth;
  state.encoder.zlibsettings.use_lz77 = 0;

  lodepng::encode(out, image, w, h, state);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if(size == 0) return 0;

  size_t color_index = data[size - 1] % num_combinations;

  LodePNGColorType colortype = colortypes[color_index];
  unsigned bitdepth = bitdepths[color_index];

  unsigned w = getDimension(data, size, 0);
  unsigned h = getDimension(data, size, 1);

  size_t raw_size = getRawSize(w, h, colortype, bitdepth);
  std::vector<unsigned char> image = makeImage(data, size, raw_size, 4);

  encodeMemory(image, w, h, colortype, bitdepth);
  encodeVector(image, w, h, colortype, bitdepth);

  std::vector<unsigned char> image24 =
      makeImage(data, size, static_cast<size_t>(w) * h * 3u, 8);
  std::vector<unsigned char> image32 =
      makeImage(data, size, static_cast<size_t>(w) * h * 4u, 16);

  encode24(image24, w, h);
  encode32(image32, w, h);

  encodeInterlaced(image, w, h, colortype, bitdepth);
  encodeUncompressed(image, w, h, colortype, bitdepth);
  encodeNoLZ77(image, w, h, colortype, bitdepth);

  return 0;
}
