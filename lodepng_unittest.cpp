/*
LodePNG Unit Test

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

//g++ lodepng.cpp lodepng_util.cpp lodepng_unittest.cpp -Wall -Wextra -Wsign-conversion -pedantic -ansi -O3

/*
Testing instructions:

*) Compile with g++ with all warnings and run the unit test
g++ lodepng.cpp lodepng_util.cpp lodepng_unittest.cpp -Wall -Wextra -Wshadow -pedantic -ansi -O3 && ./a.out

*) Compile with pure ISO C90 and all warnings:
mv lodepng.cpp lodepng.c ; gcc -I ./ lodepng.c examples/example_decode.c -ansi -pedantic -Wall -Wextra -O3 ; mv lodepng.c lodepng.cpp

*) Compile with C with -pedantic but not -ansi flag so it warns about // style comments in C++-only ifdefs
mv lodepng.cpp lodepng.c ; gcc -I ./ lodepng.c examples/example_decode.c -pedantic -Wall -Wextra -O3 ; mv lodepng.c lodepng.cpp

*) try lodepng_benchmark.cpp
g++ lodepng.cpp lodepng_benchmark.cpp -Wall -Wextra -pedantic -ansi -lSDL -O3 && ./a.out
g++ lodepng.cpp lodepng_benchmark.cpp -Wall -Wextra -pedantic -ansi -lSDL -O3 && ./a.out corpus/''*

*) Check if all examples compile without warnings:
g++ -I ./ lodepng.cpp examples/''*.cpp -W -Wall -ansi -pedantic -O3 -c
mv lodepng.cpp lodepng.c ; gcc -I ./ lodepng.c examples/''*.c -W -Wall -ansi -pedantic -O3 -c ; mv lodepng.c lodepng.cpp

*) Check pngdetail.cpp:
g++ lodepng.cpp lodepng_util.cpp pngdetail.cpp -W -Wall -ansi -pedantic -O3 -o pngdetail
./pngdetail testdata/PngSuite/basi0g01.png

*) Test compiling with some code sections with #defines disabled, for unused static function warnings etc...
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_ZLIB
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_PNG
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_DECODER
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_ENCODER
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_DISK
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_ERROR_TEXT
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_CPP
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_ZLIB -DLODEPNG_NO_COMPILE_DECODER
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_ZLIB -DLODEPNG_NO_COMPILE_ENCODER
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_PNG -DLODEPNG_NO_COMPILE_DECODER
g++ lodepng.cpp -W -Wall -ansi -pedantic -O3 -c -DLODEPNG_NO_COMPILE_PNG -DLODEPNG_NO_COMPILE_ENCODER
rm *.o

*) analyze met clang:
clang++ lodepng.cpp --analyze
More verbose:
clang++ --analyze -Xanalyzer -analyzer-output=text lodepng.cpp
Or html, look under lodepng.plist dir afterwards and find the numbered locations in the pages:
clang++ --analyze -Xanalyzer -analyzer-output=html lodepng.cpp

*) check for memory leaks and vulnerabilities with valgrind
comment out the large files tests because they're slow with valgrind
g++ lodepng.cpp lodepng_util.cpp lodepng_unittest.cpp -Wall -Wextra -pedantic -ansi -O3
valgrind --leak-check=full --track-origins=yes ./a.out

*) remove "#include <iostream>" from lodepng.cpp if it's still in there
cat lodepng.cpp | grep iostream
cat lodepng.cpp | grep "#include <"

*) check version dates in copyright message and LODEPNG_VERSION_STRING

*) check year in copyright message at top of all files as well as at bottom of lodepng.h

*) check examples/sdl.cpp with the png test suite images (the "x" ones are expected to show error)
g++ -I ./ lodepng.cpp examples/example_sdl.cpp -Wall -Wextra -pedantic -ansi -O3 -lSDL -o showpng
./showpng testdata/PngSuite/''*.png

*) strip trailing spaces and ensure consistent newlines

*) check diff of lodepng.cpp and lodepng.h before submitting

*/

#include "lodepng.h"
#include "lodepng_util.h"

#include <cmath>
#include <map>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

void fail()
{
  throw 1; //that's how to let a unittest fail
}

template<typename T, typename U>
void assertEquals(const T& expected, const U& actual, const std::string& message = "")
{
  if(expected != (T)actual)
  {
    std::cout << "Error: Not equal! Expected " << expected << " got " << (T)actual << ". "
              << "Message: " << message << std::endl;
    fail();
  }
}

void assertTrue(bool value, const std::string& message = "")
{
  if(!value)
  {
    std::cout << "Error: expected true. " << "Message: " << message << std::endl;
    fail();
  }
}

//assert that no error
void assertNoPNGError(unsigned error, const std::string& message = "")
{
  if(error)
  {
    std::string msg = (message == "") ? lodepng_error_text(error)
                                      : message + std::string(": ") + lodepng_error_text(error);
    assertEquals(0, error, msg);
  }
}

void assertNoError(unsigned error)
{
  if(error)
  {
    assertEquals(0, error, "Expected no error");
  }
}

#define STR_EXPAND(s) #s
#define STR(s) STR_EXPAND(s)
#define ASSERT_EQUALS(e, v) \
{\
  assertEquals(e, v, std::string() + "line " + STR(__LINE__) + ": " + STR(v) + " ASSERT_EQUALS(" + #e + ", " + #v + ")");\
}

static const std::string BASE64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";



//T and U can be std::string or std::vector<unsigned char>
template<typename T, typename U>
void toBase64(T& out, const U& in)
{
  for(size_t i = 0; i < in.size(); i += 3)
  {
    int v = 65536 * in[i];
    if(i + 1 < in.size()) v += 256 * in[i + 1];
    if(i + 2 < in.size()) v += in[i + 2];
    out.push_back(BASE64[(v >> 18) & 0x3f]);
    out.push_back(BASE64[(v >> 12) & 0x3f]);
    if(i + 1 < in.size()) out.push_back(BASE64[(v >> 6) & 0x3f]);
    else out.push_back('=');
    if(i + 2 < in.size()) out.push_back(BASE64[(v >> 0) & 0x3f]);
    else out.push_back('=');
  }
}

int fromBase64(int v)
{
  if(v >= 'A' && v <= 'Z') return (v - 'A');
  if(v >= 'a' && v <= 'z') return (v - 'a' + 26);
  if(v >= '0' && v <= '9') return (v - '0' + 52);
  if(v == '+') return 62;
  if(v == '/') return 63;
  return 0; //v == '='
}

//T and U can be std::string or std::vector<unsigned char>
template<typename T, typename U>
void fromBase64(T& out, const U& in)
{
  for(size_t i = 0; i + 3 < in.size(); i += 4)
  {
    int v = 262144 * fromBase64(in[i]) + 4096 * fromBase64(in[i + 1]) + 64 * fromBase64(in[i + 2]) + fromBase64(in[i + 3]);
    out.push_back((v >> 16) & 0xff);
    if(in[i + 3] != '=') out.push_back((v >> 8) & 0xff);
    if(in[i + 2] != '=') out.push_back((v >> 0) & 0xff);
  }
}

////////////////////////////////////////////////////////////////////////////////

//Test image data
struct Image
{
  std::vector<unsigned char> data;
  unsigned width;
  unsigned height;
  LodePNGColorType colorType;
  unsigned bitDepth;
};

//Utility for debug messages
template<typename T>
std::string valtostr(const T& val)
{
  std::ostringstream sstream;
  sstream << val;
  return sstream.str();
}

//Get number of color channels for a given PNG color type
unsigned getNumColorChannels(unsigned colorType)
{
  switch(colorType)
  {
    case 0: return 1; /*grey*/
    case 2: return 3; /*RGB*/
    case 3: return 1; /*palette*/
    case 4: return 2; /*grey + alpha*/
    case 6: return 4; /*RGBA*/
  }
  return 0; /*unexisting color type*/
}

//Generate a test image with some data in it, the contents of the data is unspecified,
//except the content is not just one plain color, and not true random either to be compressible.
void generateTestImage(Image& image, unsigned width, unsigned height, LodePNGColorType colorType = LCT_RGBA, unsigned bitDepth = 8)
{
  image.width = width;
  image.height = height;
  image.colorType = colorType;
  image.bitDepth = bitDepth;

  size_t bits = bitDepth * getNumColorChannels(colorType); //bits per pixel
  size_t size = (width * height * bits + 7) / 8; //total image size in bytes
  image.data.resize(size);
  unsigned char value = 128;
  for(size_t i = 0; i < size; i++)
  {
    image.data[i] = value++;
  }
}

//Check that the decoded PNG pixels are the same as the pixels in the image
void assertPixels(Image& image, const unsigned char* decoded, const std::string& message)
{
  for(size_t i = 0; i < image.data.size(); i++)
  {
    int byte_expected = image.data[i];
    int byte_actual = decoded[i];

    //last byte is special due to possible random padding bits which need not to be equal
    if(i == image.data.size() - 1)
    {
      size_t numbits = getNumColorChannels(image.colorType) * image.bitDepth * image.width * image.height;
      size_t padding = 8u - (numbits - 8u * (numbits / 8u));
      if(padding != 8u)
      {
        //set all padding bits of both to 0
        for(size_t j = 0; j < padding; j++)
        {
          byte_expected = (byte_expected & (~(1 << j))) % 256;
          byte_actual = (byte_actual & (~(1 << j))) % 256;
        }
      }
    }

    assertEquals(byte_expected, byte_actual, message + " " + valtostr(i));
  }
}

//Test LodePNG encoding and decoding the encoded result, using the C interface
void doCodecTestC(Image& image)
{
  unsigned char* encoded = 0;
  size_t encoded_size = 0;
  unsigned char* decoded = 0;
  unsigned decoded_w;
  unsigned decoded_h;

  struct OnExitScope
  {
    unsigned char* a;
    unsigned char* b;
    OnExitScope(unsigned char* ca, unsigned char* cb) : a(ca), b(cb) {}
    ~OnExitScope()
    {
      free(a);
      free(b);
    }
  } onExitScope(encoded, decoded);

  unsigned error_enc = lodepng_encode_memory(&encoded, &encoded_size, &image.data[0],
                                             image.width, image.height, image.colorType, image.bitDepth);

  if(error_enc != 0) std::cout << "Error: " << lodepng_error_text(error_enc) << std::endl;
  assertNoPNGError(error_enc, "encoder error C");

  //if the image is large enough, compressing it should result in smaller size
  if(image.data.size() > 512) assertTrue(encoded_size < image.data.size(), "compressed size");

  unsigned error_dec = lodepng_decode_memory(&decoded, &decoded_w, &decoded_h,
                                             encoded, encoded_size, image.colorType, image.bitDepth);

  if(error_dec != 0) std::cout << "Error: " << lodepng_error_text(error_dec) << std::endl;
  assertNoPNGError(error_dec, "decoder error C");

  ASSERT_EQUALS(image.width, decoded_w);
  ASSERT_EQUALS(image.height, decoded_h);
  assertPixels(image, decoded, "Pixels C");

  free(decoded);
  free(encoded);
}

//Test LodePNG encoding and decoding the encoded result, using the C++ interface
void doCodecTestCPP(Image& image)
{
  std::vector<unsigned char> encoded;
  std::vector<unsigned char> decoded;
  unsigned decoded_w;
  unsigned decoded_h;

  unsigned error_enc = lodepng::encode(encoded, image.data, image.width, image.height,
                                       image.colorType, image.bitDepth);

  assertNoPNGError(error_enc, "encoder error C++");

  //if the image is large enough, compressing it should result in smaller size
  if(image.data.size() > 512) assertTrue(encoded.size() < image.data.size(), "compressed size");

  unsigned error_dec = lodepng::decode(decoded, decoded_w, decoded_h, encoded, image.colorType, image.bitDepth);

  assertNoPNGError(error_dec, "decoder error C++");

  ASSERT_EQUALS(image.width, decoded_w);
  ASSERT_EQUALS(image.height, decoded_h);
  ASSERT_EQUALS(image.data.size(), decoded.size());
  assertPixels(image, &decoded[0], "Pixels C++");
}

//Test LodePNG encoding and decoding the encoded result, using the C++ interface, with interlace
void doCodecTestInterlaced(Image& image)
{
  std::vector<unsigned char> encoded;
  std::vector<unsigned char> decoded;
  unsigned decoded_w;
  unsigned decoded_h;

  lodepng::State state;
  state.info_png.interlace_method = 1;
  state.info_raw.colortype = image.colorType;
  state.info_raw.bitdepth = image.bitDepth;

  unsigned error_enc = lodepng::encode(encoded, image.data, image.width, image.height, state);

  assertNoPNGError(error_enc, "encoder error C++");

  //if the image is large enough, compressing it should result in smaller size
  if(image.data.size() > 512) assertTrue(encoded.size() < image.data.size(), "compressed size");

  state.info_raw.colortype = image.colorType;
  state.info_raw.bitdepth = image.bitDepth;
  unsigned error_dec = lodepng::decode(decoded, decoded_w, decoded_h, state, encoded);

  assertNoPNGError(error_dec, "decoder error C++");

  ASSERT_EQUALS(image.width, decoded_w);
  ASSERT_EQUALS(image.height, decoded_h);
  ASSERT_EQUALS(image.data.size(), decoded.size());
  assertPixels(image, &decoded[0], "Pixels C++");
}

//Test LodePNG encoding and decoding the encoded result
void doCodecTest(Image& image)
{
  doCodecTestC(image);
  doCodecTestCPP(image);
  doCodecTestInterlaced(image);
}


//Test LodePNG encoding and decoding using some image generated with the given parameters
void codecTest(unsigned width, unsigned height, LodePNGColorType colorType = LCT_RGBA, unsigned bitDepth = 8)
{
  std::cout << "codec test " << width << " " << height << std::endl;
  Image image;
  generateTestImage(image, width, height, colorType, bitDepth);
  doCodecTest(image);
}

std::string removeSpaces(const std::string& s)
{
  std::string result;
  for(size_t i = 0; i < s.size(); i++) if(s[i] != ' ') result += s[i];
  return result;
}

void bitStringToBytes(std::vector<unsigned char>& bytes, const std::string& bits_)
{
  std::string bits = removeSpaces(bits_);
  bytes.resize((bits.size()) + 7 / 8);
  for(size_t i = 0; i < bits.size(); i++)
  {
    size_t j = i / 8;
    size_t k = i % 8;
    char c = bits[i];
    if(k == 0) bytes[j] = 0;
    if(c == '1') bytes[j] |= (1 << (7 - k));
  }
}

/*
test color convert on a single pixel. Testing palette and testing color keys is
not supported by this function. Pixel values given using bits in an std::string
of 0's and 1's.
*/
void colorConvertTest(const std::string& bits_in, LodePNGColorType colorType_in, unsigned bitDepth_in,
                      const std::string& bits_out, LodePNGColorType colorType_out, unsigned bitDepth_out)
{
  std::cout << "color convert test " << bits_in << " - " << bits_out << std::endl;

  std::vector<unsigned char> expected, actual, image;
  bitStringToBytes(expected, bits_out);
  actual.resize(expected.size());
  bitStringToBytes(image, bits_in);
  LodePNGColorMode mode_in, mode_out;
  lodepng_color_mode_init(&mode_in);
  lodepng_color_mode_init(&mode_out);
  mode_in.colortype = colorType_in;
  mode_in.bitdepth = bitDepth_in;
  mode_out.colortype = colorType_out;
  mode_out.bitdepth = bitDepth_out;
  unsigned error = lodepng_convert(&actual[0], &image[0], &mode_out, &mode_in, 1, 1);

  assertNoPNGError(error, "convert error");

  for(size_t i = 0; i < expected.size(); i++)
  {
    assertEquals((int)expected[i], (int)actual[i], "byte " + valtostr(i));
  }

  lodepng_color_mode_cleanup(&mode_in);
  lodepng_color_mode_cleanup(&mode_out);
}

void testOtherPattern1()
{
  std::cout << "codec other pattern 1" << std::endl;

  Image image1;
  size_t w = 192;
  size_t h = 192;
  image1.width = w;
  image1.height = h;
  image1.colorType = LCT_RGBA;
  image1.bitDepth = 8;
  image1.data.resize(w * h * 4u);
  for(size_t y = 0; y < h; y++)
  for(size_t x = 0; x < w; x++)
  {
    //pattern 1
    image1.data[4u * w * y + 4u * x + 0u] = (unsigned char)(127 * (1 + std::sin((                    x * x +                     y * y) / (w * h / 8.0))));
    image1.data[4u * w * y + 4u * x + 1u] = (unsigned char)(127 * (1 + std::sin(((w - x - 1) * (w - x - 1) +                     y * y) / (w * h / 8.0))));
    image1.data[4u * w * y + 4u * x + 2u] = (unsigned char)(127 * (1 + std::sin((                    x * x + (h - y - 1) * (h - y - 1)) / (w * h / 8.0))));
    image1.data[4u * w * y + 4u * x + 3u] = (unsigned char)(127 * (1 + std::sin(((w - x - 1) * (w - x - 1) + (h - y - 1) * (h - y - 1)) / (w * h / 8.0))));
  }

  doCodecTest(image1);
}

void testOtherPattern2()
{
  std::cout << "codec other pattern 2" << std::endl;

  Image image1;
  size_t w = 192;
  size_t h = 192;
  image1.width = w;
  image1.height = h;
  image1.colorType = LCT_RGBA;
  image1.bitDepth = 8;
  image1.data.resize(w * h * 4u);
  for(size_t y = 0; y < h; y++)
  for(size_t x = 0; x < w; x++)
  {
    image1.data[4u * w * y + 4u * x + 0u] = 255 * !(x & y);
    image1.data[4u * w * y + 4u * x + 1u] = x ^ y;
    image1.data[4u * w * y + 4u * x + 2u] = x | y;
    image1.data[4u * w * y + 4u * x + 3u] = 255;
  }

  doCodecTest(image1);
}

void testSinglePixel(int r, int g, int b, int a)
{
  std::cout << "codec single pixel " << r << " " << g << " " << b << " " << a << std::endl;
  Image pixel;
  pixel.width = 1;
  pixel.height = 1;
  pixel.colorType = LCT_RGBA;
  pixel.bitDepth = 8;
  pixel.data.resize(4);
  pixel.data[0] = r;
  pixel.data[1] = g;
  pixel.data[2] = b;
  pixel.data[3] = a;

  doCodecTest(pixel);
}

void testColor(int r, int g, int b, int a)
{
  std::cout << "codec test color " << r << " " << g << " " << b << " " << a << std::endl;
  Image image;
  image.width = 20;
  image.height = 20;
  image.colorType = LCT_RGBA;
  image.bitDepth = 8;
  image.data.resize(20 * 20 * 4);
  for(size_t y = 0; y < 20; y++)
  for(size_t x = 0; x < 20; x++)
  {
    image.data[20 * 4 * y + 4 * x + 0] = r;
    image.data[20 * 4 * y + 4 * x + 0] = g;
    image.data[20 * 4 * y + 4 * x + 0] = b;
    image.data[20 * 4 * y + 4 * x + 0] = a;
  }

  doCodecTest(image);

  Image image2 = image;
  image2.data[3] = 0; //one fully transparent pixel
  doCodecTest(image2);
  image2.data[3] = 128; //one semi transparent pixel
  doCodecTest(image2);

  Image image3 = image;
  // add 255 different colors
  for(size_t i = 0; i < 255; i++) {
    image.data[i * 4 + 0] = i;
    image.data[i * 4 + 1] = i;
    image.data[i * 4 + 2] = i;
    image.data[i * 4 + 3] = 255;
  }
  doCodecTest(image3);
  // a 256th color
  image.data[255 * 4 + 0] = 255;
  image.data[255 * 4 + 1] = 255;
  image.data[255 * 4 + 2] = 255;
  image.data[255 * 4 + 3] = 255;
  doCodecTest(image3);

  testSinglePixel(r, g, b, a);
}

void testSize(unsigned w, unsigned h)
{
  std::cout << "codec test size " << w << " " << h << std::endl;
  Image image;
  image.width = w;
  image.height = h;
  image.colorType = LCT_RGBA;
  image.bitDepth = 8;
  image.data.resize(w * h * 4);
  for(size_t y = 0; y < h; y++)
  for(size_t x = 0; x < w; x++)
  {
    image.data[w * 4 * y + 4 * x + 0] = x % 256;
    image.data[w * 4 * y + 4 * x + 0] = y % 256;
    image.data[w * 4 * y + 4 * x + 0] = 255;
    image.data[w * 4 * y + 4 * x + 0] = 255;
  }

  doCodecTest(image);
}

void testPNGCodec()
{
  codecTest(1, 1);
  codecTest(2, 2);
  codecTest(1, 1, LCT_GREY, 1);
  codecTest(7, 7, LCT_GREY, 1);
  codecTest(127, 127);
  codecTest(127, 127, LCT_GREY, 1);

  testOtherPattern1();
  testOtherPattern2();

  testColor(255, 255, 255, 255);
  testColor(0, 0, 0, 255);
  testColor(1, 2, 3, 255);
  testColor(255, 0, 0, 255);
  testColor(0, 255, 0, 255);
  testColor(0, 0, 255, 255);
  testColor(0, 0, 0, 255);
  testColor(1, 1, 1, 255);
  testColor(1, 1, 1, 1);
  testColor(0, 0, 0, 128);
  testColor(255, 0, 0, 128);
  testColor(127, 127, 127, 255);
  testColor(128, 128, 128, 255);
  testColor(127, 127, 127, 128);
  testColor(128, 128, 128, 128);
  //transparent single pixels
  testColor(0, 0, 0, 0);
  testColor(255, 0, 0, 0);
  testColor(1, 2, 3, 0);
  testColor(255, 255, 255, 0);
  testColor(254, 254, 254, 0);

  // This is mainly to test the Adam7 interlacing
  for(unsigned h = 1; h < 12; h++)
  for(unsigned w = 1; w < 12; w++)
  {
    testSize(w, h);
  }
}

//Tests some specific color conversions with specific color bit combinations
void testColorConvert()
{
  //test color conversions to RGBA8
  colorConvertTest("1", LCT_GREY, 1, "11111111 11111111 11111111 11111111", LCT_RGBA, 8);
  colorConvertTest("10", LCT_GREY, 2, "10101010 10101010 10101010 11111111", LCT_RGBA, 8);
  colorConvertTest("1001", LCT_GREY, 4, "10011001 10011001 10011001 11111111", LCT_RGBA, 8);
  colorConvertTest("10010101", LCT_GREY, 8, "10010101 10010101 10010101 11111111", LCT_RGBA, 8);
  colorConvertTest("10010101 11111110", LCT_GREY_ALPHA, 8, "10010101 10010101 10010101 11111110", LCT_RGBA, 8);
  colorConvertTest("10010101 00000001 11111110 00000001", LCT_GREY_ALPHA, 16, "10010101 10010101 10010101 11111110", LCT_RGBA, 8);
  colorConvertTest("01010101 00000000 00110011", LCT_RGB, 8, "01010101 00000000 00110011 11111111", LCT_RGBA, 8);
  colorConvertTest("01010101 00000000 00110011 10101010", LCT_RGBA, 8, "01010101 00000000 00110011 10101010", LCT_RGBA, 8);
  colorConvertTest("10101010 01010101 11111111 00000000 11001100 00110011", LCT_RGB, 16, "10101010 11111111 11001100 11111111", LCT_RGBA, 8);
  colorConvertTest("10101010 01010101 11111111 00000000 11001100 00110011 11100111 00011000", LCT_RGBA, 16, "10101010 11111111 11001100 11100111", LCT_RGBA, 8);

  //test color conversions to RGB8
  colorConvertTest("1", LCT_GREY, 1, "11111111 11111111 11111111", LCT_RGB, 8);
  colorConvertTest("10", LCT_GREY, 2, "10101010 10101010 10101010", LCT_RGB, 8);
  colorConvertTest("1001", LCT_GREY, 4, "10011001 10011001 10011001", LCT_RGB, 8);
  colorConvertTest("10010101", LCT_GREY, 8, "10010101 10010101 10010101", LCT_RGB, 8);
  colorConvertTest("10010101 11111110", LCT_GREY_ALPHA, 8, "10010101 10010101 10010101", LCT_RGB, 8);
  colorConvertTest("10010101 00000001 11111110 00000001", LCT_GREY_ALPHA, 16, "10010101 10010101 10010101", LCT_RGB, 8);
  colorConvertTest("01010101 00000000 00110011", LCT_RGB, 8, "01010101 00000000 00110011", LCT_RGB, 8);
  colorConvertTest("01010101 00000000 00110011 10101010", LCT_RGBA, 8, "01010101 00000000 00110011", LCT_RGB, 8);
  colorConvertTest("10101010 01010101 11111111 00000000 11001100 00110011", LCT_RGB, 16, "10101010 11111111 11001100", LCT_RGB, 8);
  colorConvertTest("10101010 01010101 11111111 00000000 11001100 00110011 11100111 00011000", LCT_RGBA, 16, "10101010 11111111 11001100", LCT_RGB, 8);

  //test color conversions to RGBA16
  colorConvertTest("1", LCT_GREY, 1, "11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111", LCT_RGBA, 16);
  colorConvertTest("10", LCT_GREY, 2, "10101010 10101010 10101010 10101010 10101010 10101010 11111111 11111111", LCT_RGBA, 16);

  //test greyscale color conversions
  colorConvertTest("1", LCT_GREY, 1, "11111111", LCT_GREY, 8);
  colorConvertTest("1", LCT_GREY, 1, "1111111111111111", LCT_GREY, 16);
  colorConvertTest("0", LCT_GREY, 1, "00000000", LCT_GREY, 8);
  colorConvertTest("0", LCT_GREY, 1, "0000000000000000", LCT_GREY, 16);
  colorConvertTest("11", LCT_GREY, 2, "11111111", LCT_GREY, 8);
  colorConvertTest("11", LCT_GREY, 2, "1111111111111111", LCT_GREY, 16);
  colorConvertTest("10", LCT_GREY, 2, "10101010", LCT_GREY, 8);
  colorConvertTest("10", LCT_GREY, 2, "1010101010101010", LCT_GREY, 16);
  colorConvertTest("1000", LCT_GREY, 4, "10001000", LCT_GREY, 8);
  colorConvertTest("1000", LCT_GREY, 4, "1000100010001000", LCT_GREY, 16);
  colorConvertTest("10110101", LCT_GREY, 8, "1011010110110101", LCT_GREY, 16);
  colorConvertTest("1011010110110101", LCT_GREY, 16, "10110101", LCT_GREY, 8);

  //others
  colorConvertTest("11111111 11111111 11111111 00000000 00000000 00000000", LCT_RGB, 1, "10", LCT_GREY, 1);
}

//This tests color conversions from any color model to any color model, with any bit depth
//But it tests only with colors black and white, because that are the only colors every single model supports
void testColorConvert2()
{
  std::cout << "testColorConvert2" << std::endl;
  struct Combo
  {
    LodePNGColorType colortype;
    unsigned bitdepth;
  };

  Combo combos[15] =
  {
    { LCT_GREY, 1},
    { LCT_GREY, 2},
    { LCT_GREY, 4},
    { LCT_GREY, 8},
    { LCT_GREY, 16},
    { LCT_RGB, 8},
    { LCT_RGB, 16},
    { LCT_PALETTE, 1},
    { LCT_PALETTE, 2},
    { LCT_PALETTE, 4},
    { LCT_PALETTE, 8},
    { LCT_GREY_ALPHA, 8},
    { LCT_GREY_ALPHA, 16},
    { LCT_RGBA, 8},
    { LCT_RGBA, 16},
  };

  lodepng::State state;
  LodePNGColorMode& mode_in = state.info_png.color;
  LodePNGColorMode& mode_out = state.info_raw;
  LodePNGColorMode mode_8;
  lodepng_color_mode_init(&mode_8);

  for(size_t i = 0; i < 256; i++)
  {
    size_t j = i == 1 ? 255 : i;
    lodepng_palette_add(&mode_in, j, j, j, 255);
    lodepng_palette_add(&mode_out, j, j, j, 255);
  }

  for(size_t i = 0; i < 15; i++)
  {
    mode_in.colortype = combos[i].colortype;
    mode_in.bitdepth = combos[i].bitdepth;

    for(size_t j = 0; j < 15; j++)
    {
      mode_out.colortype = combos[i].colortype;
      mode_out.bitdepth = combos[i].bitdepth;

      unsigned char eight[36] = {
          0,0,0,255, 255,255,255,255,
          0,0,0,255, 255,255,255,255,
          255,255,255,255, 0,0,0,255,
          255,255,255,255, 255,255,255,255,
          0,0,0,255 }; //input in RGBA8
      unsigned char in[72]; //custom input color type
      unsigned char out[72]; //custom output color type
      unsigned char eight2[36]; //back in RGBA8 after all conversions to check correctness
      unsigned error = 0;

      error |= lodepng_convert(in, eight, &mode_in, &mode_8, 3, 3);
      if(!error) error |= lodepng_convert(out, in, &mode_out, &mode_in, 3, 3); //Test input to output type
      if(!error) error |= lodepng_convert(eight2, out, &mode_8, &mode_out, 3, 3);

      if(!error)
      {
        for(size_t k = 0; k < 36; k++)
        {
          if(eight[k] != eight2[k])
          {
            error = 99999;
            break;
          }
        }
      }

      if(error)
      {
        std::cout << "Error " << error << " i: " << i << " j: " << j
          << " colortype i: " << combos[i].colortype
          << " bitdepth i: " << combos[i].bitdepth
          << " colortype j: " << combos[j].colortype
          << " bitdepth j: " << combos[j].bitdepth
          << std::endl;
        if(error != 99999) assertNoPNGError(error);
        else fail();
      }
    }
  }
}

//if compressible is true, the test will also assert that the compressed string is smaller
void testCompressStringZlib(const std::string& text, bool compressible)
{
  if(text.size() < 500) std::cout << "compress test with text: " << text << std::endl;
  else std::cout << "compress test with text length: " << text.size() << std::endl;

  std::vector<unsigned char> in(text.size());
  for(size_t i = 0; i < text.size(); i++) in[i] = (unsigned char)text[i];
  unsigned char* out = 0;
  size_t outsize = 0;
  unsigned error = 0;

  error = lodepng_zlib_compress(&out, &outsize, in.empty() ? 0 : &in[0], in.size(), &lodepng_default_compress_settings);
  assertNoPNGError(error);
  if(compressible) assertTrue(outsize < in.size());

  unsigned char* out2 = 0;
  size_t outsize2 = 0;

  error = lodepng_zlib_decompress(&out2, &outsize2, out, outsize, &lodepng_default_decompress_settings);
  assertNoPNGError(error);
  ASSERT_EQUALS(outsize2, in.size());
  for(size_t i = 0; i < in.size(); i++) ASSERT_EQUALS(in[i], out2[i]);

  free(out);
  free(out2);
}

void testCompressZlib()
{
  testCompressStringZlib("", false);
  testCompressStringZlib("a", false);
  testCompressStringZlib("aa", false);
  testCompressStringZlib("ababababababababababababababababababababababababababababababababababababababababababab", true);
  testCompressStringZlib("abaaaabaabbbaabbabbababbbbabababbbaabbbaaaabbbbabbbabbbaababbbbbaaabaabbabaaaabbbbbbab", true);
  testCompressStringZlib("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab", true);
  testCompressStringZlib("omnomnomnomnomnomnomnomnomnomnom", true);
  testCompressStringZlib("the quick brown fox jumps over the lazy dog. the quick brown fox jumps over the lazy dog.", true);
  testCompressStringZlib("abracadabra", false);
  testCompressStringZlib("hello hello hello hello hello hello hello hello hello hello hello?", true);
  testCompressStringZlib("WPgZX2D*um0H::,4/KU\"kt\"Ne\"#Qa.&#<aF9{jag]|{hv,IXez\
\\DKn5zYdV{XxBi=n|1J-TwakWvp[b8|-kOcZ@QkAxJSMeZ0l&<*w0BP/CXM(LFH'", false);
  testCompressStringZlib("asdfhlkhfafsduyfbasiuytfgbiasuidygiausygdifaubsydfsdf", false);
  testCompressStringZlib("418541499849814614617987416457317375467441841687487", true);
  testCompressStringZlib("3.141592653589793238462643383279502884197169399375105820974944592307816406286", true);
  testCompressStringZlib("lodepng_zlib_decompress(&out2, &outsize2, out, outsize, &lodepng_default_decompress_settings);", true);
}

void testDiskCompressZlib(const std::string& filename)
{
  std::cout << "testDiskCompressZlib: File " << filename << std::endl;

  std::vector<unsigned char> buffer;
  lodepng::load_file(buffer, filename);
  std::string f;
  for(size_t i = 0; i < buffer.size(); i++) f += (char)buffer[i];
  testCompressStringZlib(f, false);
}

void testDiskPNG(const std::string& filename)
{
  std::cout << "testDiskPNG: File " << filename << std::endl;

  Image image;
  image.colorType = LCT_RGB;
  image.bitDepth = 8;
  unsigned error = lodepng::decode(image.data, image.width, image.height, filename, image.colorType, image.bitDepth);
  assertNoPNGError(error);

  doCodecTest(image);
}

std::vector<unsigned> strtovector(const std::string& numbers)
{
  std::vector<unsigned> result;
  std::stringstream ss(numbers);
  unsigned i;
  while(ss >> i) result.push_back(i);
  return result;
}

void doTestHuffmanCodeLengths(const std::string& expectedstr, const std::string& counts, size_t bitlength)
{
  std::vector<unsigned> expected = strtovector(expectedstr);
  std::vector<unsigned> count = strtovector(counts);
  std::cout << "doTestHuffmanCodeLengths: " << counts << std::endl;
  std::vector<unsigned> result(count.size());
  unsigned error = lodepng_huffman_code_lengths(&result[0], &count[0], count.size(), bitlength);
  assertNoPNGError(error, "errorcode");
  std::stringstream ss1, ss2;
  for(size_t i = 0; i < count.size(); i++)
  {
    ss1 << expected[i] << " ";
    ss2 << result[i] << " ";
  }
  assertEquals(ss1.str(), ss2.str(), "value");
}

void testHuffmanCodeLengths()
{
  bool atleasttwo = true; //LodePNG generates at least two, instead of at least one, symbol
  if(atleasttwo)
  {
    doTestHuffmanCodeLengths("1 1", "0 0", 16);
    doTestHuffmanCodeLengths("1 1 0", "0 0 0", 16);
    doTestHuffmanCodeLengths("1 1", "1 0", 16);
    doTestHuffmanCodeLengths("1 1 0 0 0 0 0 0 0", "0 0 0 0 0 0 0 0 0", 16);
    doTestHuffmanCodeLengths("1 1 0 0 0 0 0 0 0", "1 0 0 0 0 0 0 0 0", 16);
    doTestHuffmanCodeLengths("1 1 0 0 0 0 0 0 0", "0 1 0 0 0 0 0 0 0", 16);
    doTestHuffmanCodeLengths("1 0 0 0 0 0 0 0 1", "0 0 0 0 0 0 0 0 1", 16);
    doTestHuffmanCodeLengths("0 0 0 0 0 0 0 1 1", "0 0 0 0 0 0 0 1 1", 16);
  }
  else
  {
    doTestHuffmanCodeLengths("1 0", "0 0", 16);
    doTestHuffmanCodeLengths("1 0 0", "0 0 0", 16);
    doTestHuffmanCodeLengths("1 0", "1 0", 16);
    doTestHuffmanCodeLengths("1", "1", 16);
    doTestHuffmanCodeLengths("1", "0", 16);
  }
  doTestHuffmanCodeLengths("1 1", "1 1", 16);
  doTestHuffmanCodeLengths("1 1", "1 100", 16);
  doTestHuffmanCodeLengths("2 2 1", "1 2 3", 16);
  doTestHuffmanCodeLengths("2 1 2", "2 3 1", 16);
  doTestHuffmanCodeLengths("1 2 2", "3 1 2", 16);
  doTestHuffmanCodeLengths("3 3 2 1", "1 30 31 32", 16);
  doTestHuffmanCodeLengths("2 2 2 2", "1 30 31 32", 2);
  doTestHuffmanCodeLengths("5 5 4 4 4 3 3 1", "1 2 3 4 5 6 7 500", 16);
}

/*
Create a PNG image with all known chunks (except only one of tEXt or zTXt) plus
unknown chunks, and a palette.
*/
void createComplexPNG(std::vector<unsigned char>& png)
{
  unsigned w = 16, h = 17;
  std::vector<unsigned char> image(w * h);
  for(size_t i = 0; i < w * h; i++)
  {
    image[i] = i % 256;
  }

  lodepng::State state;
  LodePNGInfo& info = state.info_png;
  info.color.colortype = LCT_PALETTE;
  info.color.bitdepth = 8;
  state.info_raw.colortype = LCT_PALETTE;
  state.info_raw.bitdepth = 8;
  state.encoder.auto_convert = false;
  state.encoder.text_compression = 1;
  state.encoder.add_id = 1;
  for(size_t i = 0; i < 256; i++)
  {
    lodepng_palette_add(&info.color, i, i, i, i);
    lodepng_palette_add(&state.info_raw, i, i, i, i);
  }

  info.background_defined = 1;
  info.background_r = 127;

  lodepng_add_text(&info, "key0", "string0");
  lodepng_add_text(&info, "key1", "string1");

  lodepng_add_itext(&info, "ikey0", "ilangtag0", "itranskey0", "istring0");
  lodepng_add_itext(&info, "ikey1", "ilangtag1", "itranskey1", "istring1");

  info.time_defined = 1;
  info.time.year = 2012;
  info.time.month = 1;
  info.time.day = 2;
  info.time.hour = 3;
  info.time.minute = 4;
  info.time.second = 5;

  info.phys_defined = 1;
  info.phys_x = 1;
  info.phys_y = 2;
  info.phys_unit = 1;

  lodepng_chunk_create(&info.unknown_chunks_data[0], &info.unknown_chunks_size[0], 3, "uNKa", (unsigned char*)"a00");
  lodepng_chunk_create(&info.unknown_chunks_data[0], &info.unknown_chunks_size[0], 3, "uNKa", (unsigned char*)"a01");
  lodepng_chunk_create(&info.unknown_chunks_data[1], &info.unknown_chunks_size[1], 3, "uNKb", (unsigned char*)"b00");
  lodepng_chunk_create(&info.unknown_chunks_data[2], &info.unknown_chunks_size[2], 3, "uNKc", (unsigned char*)"c00");

  unsigned error = lodepng::encode(png, &image[0], w, h, state);
  assertNoPNGError(error);
}

std::string extractChunkNames(const std::vector<unsigned char>& png)
{
  const unsigned char* chunk = &png[8];
  char name[5];
  std::string result = "";
  for(;;)
  {
    lodepng_chunk_type(name, chunk);
    result += (std::string(" ") + name);
    if(std::string(name) == "IEND") break;
    chunk = lodepng_chunk_next_const(chunk);
    assertTrue(chunk < &png.back(), "jumped out of chunks");
  }
  return result;
}

void testComplexPNG()
{
  std::cout << "testComplexPNG" << std::endl;

  std::vector<unsigned char> png;
  createComplexPNG(png);

  lodepng::State state;
  LodePNGInfo& info = state.info_png;
  unsigned w, h;
  std::vector<unsigned char> image;
  unsigned error = lodepng::decode(image, w, h, state, &png[0], png.size());
  assertNoPNGError(error);

  ASSERT_EQUALS(16, w);
  ASSERT_EQUALS(17, h);
  ASSERT_EQUALS(1, info.background_defined);
  ASSERT_EQUALS(127, info.background_r);
  ASSERT_EQUALS(1, info.time_defined);
  ASSERT_EQUALS(2012, info.time.year);
  ASSERT_EQUALS(1, info.time.month);
  ASSERT_EQUALS(2, info.time.day);
  ASSERT_EQUALS(3, info.time.hour);
  ASSERT_EQUALS(4, info.time.minute);
  ASSERT_EQUALS(5, info.time.second);
  ASSERT_EQUALS(1, info.phys_defined);
  ASSERT_EQUALS(1, info.phys_x);
  ASSERT_EQUALS(2, info.phys_y);
  ASSERT_EQUALS(1, info.phys_unit);

  std::string chunknames = extractChunkNames(png);
  //std::string expectednames = " IHDR uNKa uNKa PLTE tRNS bKGD pHYs uNKb IDAT tIME tEXt tEXt tEXt iTXt iTXt uNKc IEND";
  std::string expectednames = " IHDR uNKa uNKa PLTE tRNS bKGD pHYs uNKb IDAT tIME zTXt zTXt tEXt iTXt iTXt uNKc IEND";
  ASSERT_EQUALS(expectednames, chunknames);

  //TODO: test strings and unknown chunks too
}

//test that, by default, it chooses filter type zero for all scanlines if the image has a palette
void testPaletteFilterTypesZero()
{
  std::cout << "testPaletteFilterTypesZero" << std::endl;

  std::vector<unsigned char> png;
  createComplexPNG(png);

  std::vector<unsigned char> filterTypes;
  lodepng::getFilterTypes(filterTypes, png);

  ASSERT_EQUALS(17, filterTypes.size());
  for(size_t i = 0; i < 17; i++) ASSERT_EQUALS(0, filterTypes[i]);
}

//tests that there are no crashes with auto color chooser in case of palettes with translucency etc...
void testPaletteToPaletteConvert()
{
  std::cout << "testPaletteToPaletteConvert" << std::endl;
  unsigned error;
  unsigned w = 16, h = 16;
  std::vector<unsigned char> image(w * h);
  for(size_t i = 0; i < w * h; i++) image[i] = i % 256;
  lodepng::State state;
  LodePNGInfo& info = state.info_png;
  info.color.colortype = state.info_raw.colortype = LCT_PALETTE;
  info.color.bitdepth = state.info_raw.bitdepth = 8;
  ASSERT_EQUALS(true, state.encoder.auto_convert);
  for(size_t i = 0; i < 256; i++)
  {
    lodepng_palette_add(&info.color, i, i, i, i);
  }
  std::vector<unsigned char> png;
  for(size_t i = 0; i < 256; i++)
  {
    lodepng_palette_add(&state.info_raw, i, i, i, i);
  }
  error = lodepng::encode(png, &image[0], w, h, state);
  assertNoPNGError(error);
}

//for this test, you have to choose palette colors that cause LodePNG to actually use a palette,
//so don't use all greyscale colors for example
void doRGBAToPaletteTest(unsigned char* palette, size_t size, LodePNGColorType expectedType = LCT_PALETTE)
{
  std::cout << "testRGBToPaletteConvert " << size << std::endl;
  unsigned error;
  unsigned w = size, h = 257 /*LodePNG encodes no palette if image is too small*/;
  std::vector<unsigned char> image(w * h * 4);
  for(size_t i = 0; i < image.size(); i++) image[i] = palette[i % (size * 4)];
  std::vector<unsigned char> png;
  error = lodepng::encode(png, &image[0], w, h);
  assertNoPNGError(error);
  lodepng::State state;
  std::vector<unsigned char> image2;
  error = lodepng::decode(image2, w, h, state, png);
  assertNoPNGError(error);
  ASSERT_EQUALS(image.size(), image2.size());
  for(size_t i = 0; i < image.size(); i++) ASSERT_EQUALS(image[i], image2[i]);

  ASSERT_EQUALS(expectedType, state.info_png.color.colortype);
  if(expectedType == LCT_PALETTE)
  {

    ASSERT_EQUALS(size, state.info_png.color.palettesize);
    for(size_t i = 0; i < size * 4; i++) ASSERT_EQUALS(state.info_png.color.palette[i], image[i]);
  }
}

void testRGBToPaletteConvert()
{
  unsigned char palette1[4] = {1,2,3,4};
  doRGBAToPaletteTest(palette1, 1);
  unsigned char palette2[8] = {1,2,3,4, 5,6,7,8};
  doRGBAToPaletteTest(palette2, 2);
  unsigned char palette3[12] = {1,1,1,255, 20,20,20,255, 20,20,21,255};
  doRGBAToPaletteTest(palette3, 3);

  std::vector<unsigned char> palette;
  for(int i = 0; i < 256; i++)
  {
    palette.push_back(i);
    palette.push_back(5);
    palette.push_back(6);
    palette.push_back(128);
  }
  doRGBAToPaletteTest(&palette[0], 256);
  palette.push_back(5);
  palette.push_back(6);
  palette.push_back(7);
  palette.push_back(8);
  doRGBAToPaletteTest(&palette[0], 257, LCT_RGBA);
}

void testColorKeyConvert()
{
  std::cout << "testColorKeyConvert" << std::endl;
  unsigned error;
  unsigned w = 32, h = 32;
  std::vector<unsigned char> image(w * h * 4);
  for(size_t i = 0; i < w * h; i++)
  {
    image[i * 4 + 0] = i % 256;
    image[i * 4 + 1] = i / 256;
    image[i * 4 + 2] = 0;
    image[i * 4 + 3] = i == 23 ? 0 : 255;
  }
  std::vector<unsigned char> png;
  error = lodepng::encode(png, &image[0], w, h);
  assertNoPNGError(error);

  lodepng::State state;
  std::vector<unsigned char> image2;
  error = lodepng::decode(image2, w, h, state, png);
  assertNoPNGError(error);
  ASSERT_EQUALS(32, w);
  ASSERT_EQUALS(32, h);
  ASSERT_EQUALS(1, state.info_png.color.key_defined);
  ASSERT_EQUALS(23, state.info_png.color.key_r);
  ASSERT_EQUALS(0, state.info_png.color.key_g);
  ASSERT_EQUALS(0, state.info_png.color.key_b);
  ASSERT_EQUALS(image.size(), image2.size());
  for(size_t i = 0; i < image.size(); i++)
  {
    ASSERT_EQUALS(image[i], image2[i]);
  }
}

void testNoAutoConvert()
{
  std::cout << "testNoAutoConvert" << std::endl;
  unsigned error;
  unsigned w = 32, h = 32;
  std::vector<unsigned char> image(w * h * 4);
  for(size_t i = 0; i < w * h; i++)
  {
    image[i * 4 + 0] = (i % 2) ? 255 : 0;
    image[i * 4 + 1] = (i % 2) ? 255 : 0;
    image[i * 4 + 2] = (i % 2) ? 255 : 0;
    image[i * 4 + 3] = 0;
  }
  std::vector<unsigned char> png;
  lodepng::State state;
  state.info_png.color.colortype = LCT_RGBA;
  state.info_png.color.bitdepth = 8;
  state.encoder.auto_convert = false;
  error = lodepng::encode(png, &image[0], w, h, state);
  assertNoPNGError(error);

  lodepng::State state2;
  std::vector<unsigned char> image2;
  error = lodepng::decode(image2, w, h, state2, png);
  assertNoPNGError(error);
  ASSERT_EQUALS(32, w);
  ASSERT_EQUALS(32, h);
  ASSERT_EQUALS(LCT_RGBA, state2.info_png.color.colortype);
  ASSERT_EQUALS(8, state2.info_png.color.bitdepth);
  ASSERT_EQUALS(image.size(), image2.size());
  for(size_t i = 0; i < image.size(); i++)
  {
    ASSERT_EQUALS(image[i], image2[i]);
  }
}

unsigned char flipBit(unsigned char c, int bitpos)
{
  return c ^ (1 << bitpos);
}

//Test various broken inputs. Returned errors are not checked, what is tested is
//that is doesn't crash, and, when run with valgrind, no memory warnings are
//given.
void testFuzzing()
{
  std::cout << "testFuzzing" << std::endl;
  std::vector<unsigned char> png;
  createComplexPNG(png);
  std::vector<unsigned char> broken = png;
  std::vector<unsigned char> result;
  std::map<unsigned, unsigned> errors;
  unsigned w, h;
  lodepng::State state;
  state.decoder.ignore_crc = 1;
  state.decoder.zlibsettings.ignore_adler32 = 1;
  for(size_t i = 0; i < png.size(); i++)
  {
    result.clear();
    broken[i] = ~png[i];
    errors[lodepng::decode(result, w, h, state, broken)]++;
    broken[i] = 0;
    errors[lodepng::decode(result, w, h, state, broken)]++;
    for(int j = 0; j < 8; j++)
    {
      broken[i] = flipBit(png[i], j);
      errors[lodepng::decode(result, w, h, state, broken)]++;
    }
    broken[i] = 255;
    errors[lodepng::decode(result, w, h, state, broken)]++;
    broken[i] = png[i]; //fix it again for the next test
  }
  std::cout << "testFuzzing shrinking" << std::endl;
  broken = png;
  while(broken.size() > 0)
  {
    broken.resize(broken.size() - 1);
    errors[lodepng::decode(result, w, h, state, broken)]++;
  }

  //For fun, print the number of each error
  std::cout << "Fuzzing error code counts: ";
  for(std::map<unsigned, unsigned>::iterator it = errors.begin(); it != errors.end(); ++it)
  {
    std::cout << it->first << ":" << it->second << ", ";
  }
  std::cout << std::endl;
}

void testCustomZlibCompress()
{
  std::cout << "testCustomZlibCompress" << std::endl;
  Image image;
  generateTestImage(image, 5, 5, LCT_RGBA, 8);

  std::vector<unsigned char> encoded;
  int customcontext = 5;

  struct TestFun {
    static unsigned custom_zlib(unsigned char**, size_t*,
                          const unsigned char*, size_t,
                          const LodePNGCompressSettings* settings)
    {
      ASSERT_EQUALS(5, *(int*)(settings->custom_context));
      return 5555; //return a custom error code to prove this function was called
    }
  };

  lodepng::State state;
  state.encoder.zlibsettings.custom_zlib = TestFun::custom_zlib;
  state.encoder.zlibsettings.custom_context = &customcontext;

  unsigned error = lodepng::encode(encoded, image.data, image.width, image.height,
                                   state);

  ASSERT_EQUALS(5555, error);
}

void testCustomZlibCompress2()
{
  std::cout << "testCustomZlibCompress2" << std::endl;
  Image image;
  generateTestImage(image, 5, 5, LCT_RGBA, 8);

  std::vector<unsigned char> encoded;

  lodepng::State state;
  state.encoder.zlibsettings.custom_zlib = lodepng_zlib_compress;

  unsigned error = lodepng::encode(encoded, image.data, image.width, image.height,
                                   state);
  assertNoPNGError(error);

  std::vector<unsigned char> decoded;
  unsigned w, h;
  state.decoder.zlibsettings.ignore_adler32 = 0;
  state.decoder.ignore_crc = 0;
  error = lodepng::decode(decoded, w, h, state, encoded);
  assertNoPNGError(error);
  ASSERT_EQUALS(5, w);
  ASSERT_EQUALS(5, h);
}

void testCustomDeflate()
{
  std::cout << "testCustomDeflate" << std::endl;
  Image image;
  generateTestImage(image, 5, 5, LCT_RGBA, 8);

  std::vector<unsigned char> encoded;
  int customcontext = 5;

  struct TestFun {
    static unsigned custom_deflate(unsigned char**, size_t*,
                                   const unsigned char*, size_t,
                                   const LodePNGCompressSettings* settings)
    {
      ASSERT_EQUALS(5, *(int*)(settings->custom_context));
      return 5555; //return a custom error code to prove this function was called
    }
  };

  lodepng::State state;
  state.encoder.zlibsettings.custom_deflate = TestFun::custom_deflate;
  state.encoder.zlibsettings.custom_context = &customcontext;

  unsigned error = lodepng::encode(encoded, image.data, image.width, image.height,
                                   state);

  ASSERT_EQUALS(5555, error);
}

void testCustomZlibDecompress()
{
  std::cout << "testCustomZlibDecompress" << std::endl;
  Image image;
  generateTestImage(image, 5, 5, LCT_RGBA, 8);

  std::vector<unsigned char> encoded;

  unsigned error_enc = lodepng::encode(encoded, image.data, image.width, image.height,
                                   image.colorType, image.bitDepth);
  assertNoPNGError(error_enc, "encoder error not expected");


  std::vector<unsigned char> decoded;
  unsigned w, h;
  int customcontext = 5;

  struct TestFun {
    static unsigned custom_zlib(unsigned char**, size_t*,
                          const unsigned char*, size_t,
                          const LodePNGDecompressSettings* settings)
    {
      ASSERT_EQUALS(5, *(int*)(settings->custom_context));
      return 5555; //return a custom error code to prove this function was called
    }
  };

  lodepng::State state;
  state.decoder.zlibsettings.custom_zlib = TestFun::custom_zlib;
  state.decoder.zlibsettings.custom_context = &customcontext;
  state.decoder.zlibsettings.ignore_adler32 = 0;
  state.decoder.ignore_crc = 0;
  unsigned error = lodepng::decode(decoded, w, h, state, encoded);

  ASSERT_EQUALS(5555, error);
}

void testCustomInflate()
{
  std::cout << "testCustomInflate" << std::endl;
  Image image;
  generateTestImage(image, 5, 5, LCT_RGBA, 8);

  std::vector<unsigned char> encoded;

  unsigned error_enc = lodepng::encode(encoded, image.data, image.width, image.height,
                                   image.colorType, image.bitDepth);
  assertNoPNGError(error_enc, "encoder error not expected");


  std::vector<unsigned char> decoded;
  unsigned w, h;
  int customcontext = 5;

  struct TestFun {
    static unsigned custom_inflate(unsigned char**, size_t*,
                                   const unsigned char*, size_t,
                                   const LodePNGDecompressSettings* settings)
    {
      ASSERT_EQUALS(5, *(int*)(settings->custom_context));
      return 5555; //return a custom error code to prove this function was called
    }
  };

  lodepng::State state;
  state.decoder.zlibsettings.custom_inflate = TestFun::custom_inflate;
  state.decoder.zlibsettings.custom_context = &customcontext;
  state.decoder.zlibsettings.ignore_adler32 = 0;
  state.decoder.ignore_crc = 0;
  unsigned error = lodepng::decode(decoded, w, h, state, encoded);

  ASSERT_EQUALS(5555, error);
}

void doPngSuiteTinyTest(const std::string& base64, unsigned w, unsigned h,
                        unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  lodepng::State state;
  std::vector<unsigned char> png;
  fromBase64(png, base64);
  unsigned w2, h2;
  std::vector<unsigned char> image;
  unsigned error = lodepng::decode(image, w2, h2, state, png);
  assertNoPNGError(error);
  ASSERT_EQUALS(w, w2);
  ASSERT_EQUALS(h, h2);
  ASSERT_EQUALS((int)r, (int)image[0]);
  ASSERT_EQUALS((int)g, (int)image[1]);
  ASSERT_EQUALS((int)b, (int)image[2]);
  ASSERT_EQUALS((int)a, (int)image[3]);

  state.encoder.auto_convert = false;
  std::vector<unsigned char> png2;
  error = lodepng::encode(png2, image, w, h, state);
  assertNoPNGError(error);
  std::vector<unsigned char> image2;
  error = lodepng::decode(image2, w2, h2, state, png2);
  assertNoPNGError(error);
  for(size_t i = 0; i < image.size(); i++) ASSERT_EQUALS(image[i], image2[i]);
}

/*checks that both png suite images have the exact same pixel content, e.g. to check that
it decodes an interlaced and non-interlaced corresponding png suite image equally*/
void doPngSuiteEqualTest(const std::string& base64a, const std::string& base64b)
{
  lodepng::State state;
  std::vector<unsigned char> pnga, pngb;
  fromBase64(pnga, base64a);
  fromBase64(pngb, base64b);
  unsigned wa, ha, wb, hb;
  std::vector<unsigned char> imagea, imageb;
  assertNoPNGError(lodepng::decode(imagea, wa, ha, state, pnga));
  assertNoPNGError(lodepng::decode(imageb, wb, hb, state, pngb));
  ASSERT_EQUALS(wa, wb);
  ASSERT_EQUALS(ha, hb);

  size_t size = wa * ha * 4;
  for(size_t i = 0; i < size; i++)
  {
    if(imagea[i] != imageb[i])
    {
      std::cout << "x: " << ((i / 4) % wa) << " y: " << ((i / 4) / wa) << " c: " << i % 4 << std::endl;
      ASSERT_EQUALS((int)imagea[i], (int)imageb[i]);
    }
  }
}

void testPngSuiteTiny()
{
  std::cout << "testPngSuiteTiny" << std::endl;
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABAQMAAAFS3GZcAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                     "BAQEd/i1owAAAANQTFRFAAD/injSVwAAAApJREFUeJxjYAAAAAIAAUivpHEAAAAASUVORK5CYII=",
                     1, 1, 0, 0, 255, 255); //s01n3p01.png
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABAQMAAAAl21bKAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                     "BAQEd/i1owAAAANQTFRFAAD/injSVwAAAApJREFUeJxjYAAAAAIAAUivpHEAAAAASUVORK5CYII=",
                     1, 1, 0, 0, 255, 255); //s01i3p01.png
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHAgMAAAC5PL9AAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                     "BAQEd/i1owAAAAxQTFRF/wB3AP93//8AAAD/G0OznAAAABpJREFUeJxj+P+H4WoMw605DDfmgEgg"
                     "+/8fAHF5CrkeXW0HAAAAAElFTkSuQmCC",
                     7, 7, 0, 0, 255, 255); //s07n3p02.png
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHAgMAAAHOO4/WAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                     "BAQEd/i1owAAAAxQTFRF/wB3AP93//8AAAD/G0OznAAAACVJREFUeJxjOMBwgOEBwweGDQyvGf4z"
                     "/GFIAcI/DFdjGG7MAZIAweMMgVWC+YkAAAAASUVORK5CYII=",
                     7, 7, 0, 0, 255, 255); //s07i3p02.png
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAACAAAAAgAgMAAAAOFJJnAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                     "AQEBfC53ggAAAAxQTFRFAP8A/wAA//8AAAD/ZT8rugAAACJJREFUeJxj+B+6igGEGfAw8MnBGKug"
                     "LHwMqNL/+BiDzD0AvUl/geqJjhsAAAAASUVORK5CYII=",
                     32, 32, 0, 0, 255, 255); //basn3p02.png
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAACAAAAAgAQMAAABJtOi3AAAABGdBTUEAAYagMeiWXwAAAAZQTFRF"
                     "7v8iImb/bBrSJgAAABVJREFUeJxj4AcCBjTiAxCgEwOkDgC7Hz/Bk4JmWQAAAABJRU5ErkJggg==",
                     32, 32, 238, 255, 34, 255); //basn3p01.png
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAACAAAAAgEAAAAAAGgflrAAAABGdBTUEAAYagMeiWXwAAAF5JREFU"
                     "eJzV0jEKwDAMQ1E5W+9/xtygk8AoezLVKgSj2Y8/OICnuFcTE2OgOoJgHQiZAN2C9kDKBOgW3AZC"
                     "JkC3oD2QMgG6BbeBkAnQLWgPpExgP28H7E/0GTjPfwAW2EvYX64rn9cAAAAASUVORK5CYII=",
                     32, 32, 0, 0, 0, 255); //basn0g16.png
  doPngSuiteTinyTest("iVBORw0KGgoAAAANSUhEUgAAACAAAAAgEAAAAAFxhsn9AAAABGdBTUEAAYagMeiWXwAAAOJJREFU"
                     "eJy1kTsOwjAQRMdJCqj4XYHD5DAcj1Okyg2okCyBRLOSC0BDERKCI7xJVmgaa/X8PFo7oESJEtka"
                     "TeLDjdjjgCMe7eTE96FGd3AL7HvZsdNEaJMVo0GNGm775bgwW6Afj/SAjAY+JsYNXIHtz2xYxTXi"
                     "UoOek4AbFcCnDYEK4NMGsgXcMrGHJytkBX5HIP8FAhVANIMVIBVANMPfgUAFEM3wAVyG5cxcecY5"
                     "/dup3LVFa1HXmA61LY59f6Ygp1Eg1gZGQaBRILYGdxoFYmtAGgXx9YmCfPD+RMHwuuAFVpjuiRT/"
                     "//4AAAAASUVORK5CYII=",
                     32, 32, 0, 0, 0, 255); //basi0g16.png

  //s01n3p01.png s01i3p01.png
  doPngSuiteEqualTest("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABAQMAAAFS3GZcAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                      "BAQEd/i1owAAAANQTFRFAAD/injSVwAAAApJREFUeJxjYAAAAAIAAUivpHEAAAAASUVORK5CYII=",
                      "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABAQMAAAAl21bKAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                      "BAQEd/i1owAAAANQTFRFAAD/injSVwAAAApJREFUeJxjYAAAAAIAAUivpHEAAAAASUVORK5CYII=");
  //s07n3p02.png and s07i3p02.png
  doPngSuiteEqualTest("iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHAgMAAAC5PL9AAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                      "BAQEd/i1owAAAAxQTFRF/wB3AP93//8AAAD/G0OznAAAABpJREFUeJxj+P+H4WoMw605DDfmgEgg"
                      "+/8fAHF5CrkeXW0HAAAAAElFTkSuQmCC",
                      "iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHAgMAAAHOO4/WAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
                      "BAQEd/i1owAAAAxQTFRF/wB3AP93//8AAAD/G0OznAAAACVJREFUeJxjOMBwgOEBwweGDQyvGf4z"
                      "/GFIAcI/DFdjGG7MAZIAweMMgVWC+YkAAAAASUVORK5CYII=");
  //basn0g16.png and basi0g16.png
  doPngSuiteEqualTest("iVBORw0KGgoAAAANSUhEUgAAACAAAAAgEAAAAAAGgflrAAAABGdBTUEAAYagMeiWXwAAAF5JREFU"
                      "eJzV0jEKwDAMQ1E5W+9/xtygk8AoezLVKgSj2Y8/OICnuFcTE2OgOoJgHQiZAN2C9kDKBOgW3AZC"
                      "JkC3oD2QMgG6BbeBkAnQLWgPpExgP28H7E/0GTjPfwAW2EvYX64rn9cAAAAASUVORK5CYII=",
                      "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgEAAAAAFxhsn9AAAABGdBTUEAAYagMeiWXwAAAOJJREFU"
                      "eJy1kTsOwjAQRMdJCqj4XYHD5DAcj1Okyg2okCyBRLOSC0BDERKCI7xJVmgaa/X8PFo7oESJEtka"
                      "TeLDjdjjgCMe7eTE96FGd3AL7HvZsdNEaJMVo0GNGm775bgwW6Afj/SAjAY+JsYNXIHtz2xYxTXi"
                      "UoOek4AbFcCnDYEK4NMGsgXcMrGHJytkBX5HIP8FAhVANIMVIBVANMPfgUAFEM3wAVyG5cxcecY5"
                      "/dup3LVFa1HXmA61LY59f6Ygp1Eg1gZGQaBRILYGdxoFYmtAGgXx9YmCfPD+RMHwuuAFVpjuiRT/"
                      "//4AAAAASUVORK5CYII=");
}

void testChunkUtil()
{
  std::cout << "testChunkUtil" << std::endl;
  std::vector<unsigned char> png;
  createComplexPNG(png);

  std::vector<std::string> names[3];
  std::vector<std::vector<unsigned char> > chunks[3];

  assertNoError(lodepng::getChunks(names, chunks, png));

  std::vector<std::vector<unsigned char> > chunks2[3];
  chunks2[0].push_back(chunks[2][2]); //zTXt
  chunks2[1].push_back(chunks[2][3]); //tEXt
  chunks2[2].push_back(chunks[2][4]); //iTXt

  assertNoError(lodepng::insertChunks(png, chunks2));

  std::string chunknames = extractChunkNames(png);
  //                                        chunks2[0]                    chunks2[1]                                   chunks2[2]
  //                                             v                             v                                            v
  std::string expectednames = " IHDR uNKa uNKa zTXt PLTE tRNS bKGD pHYs uNKb tEXt IDAT tIME zTXt zTXt tEXt iTXt iTXt uNKc iTXt IEND";
  ASSERT_EQUALS(expectednames, chunknames);

  std::vector<unsigned char> image;
  unsigned w, h;
  assertNoPNGError(lodepng::decode(image, w, h, png));
}

//Test that when decoding to 16-bit per channel, it always uses big endian consistently.
//It should always output big endian, the convention used inside of PNG, even though x86 CPU's are little endian.
void test16bitColorEndianness()
{
  std::cout << "test16bitColorEndianness" << std::endl;

  //basn0g16.png from the PNG test suite
  std::string base64 = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgEAAAAAAGgflrAAAABGdBTUEAAYagMeiWXwAAAF5JREFU"
                       "eJzV0jEKwDAMQ1E5W+9/xtygk8AoezLVKgSj2Y8/OICnuFcTE2OgOoJgHQiZAN2C9kDKBOgW3AZC"
                       "JkC3oD2QMgG6BbeBkAnQLWgPpExgP28H7E/0GTjPfwAW2EvYX64rn9cAAAAASUVORK5CYII=";
  std::vector<unsigned char> png;
  fromBase64(png, base64);
  unsigned w, h;
  std::vector<unsigned char> image;
  lodepng::State state;

  // Decode from 16-bit grey image to 16-bit per channel RGBA
  state.info_raw.bitdepth = 16;
  assertNoPNGError(lodepng::decode(image, w, h, state, png));
  ASSERT_EQUALS(0x09, image[8]);
  ASSERT_EQUALS(0x00, image[9]);

  // Decode from 16-bit grey image to 16-bit grey raw image (no conversion)
  image.clear();
  state = lodepng::State();
  state.decoder.color_convert = false;
  assertNoPNGError(lodepng::decode(image, w, h, state, png));
  ASSERT_EQUALS(0x09, image[2]);
  ASSERT_EQUALS(0x00, image[3]);

  // Decode from 16-bit per channel RGB image to 16-bit per channel RGBA
  base64 = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgEAIAAACsiDHgAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
           "DQ0N0DeNwQAAAH5JREFUeJztl8ENxEAIAwcJ6cpI+q8qKeNepAgelq2dCjz4AdQM1jRcf3WIDQ13"
           "qUNsiBBQZ1gR0cARUFIz3pug3586wo5+rOcfIaBOsCSggSOgpcB8D4D3R9DgfUyECIhDbAhp4Ajo"
           "KPD+CBq8P4IG72MiQkCdYUVEA0dAyQcwUyZpXH92ZwAAAABJRU5ErkJggg=="; //cs3n2c16.png
  png.clear();
  fromBase64(png, base64);
  image.clear();
  state = lodepng::State();
  state.info_raw.bitdepth = 16;
  assertNoPNGError(lodepng::decode(image, w, h, state, png));
  ASSERT_EQUALS(0x1f, image[258]);
  ASSERT_EQUALS(0xf9, image[259]);

  // Decode from 16-bit per channel RGB image to 16-bit per channel RGBA raw image (no conversion)
  image.clear();
  state = lodepng::State();
  state.decoder.color_convert = false;
  assertNoPNGError(lodepng::decode(image, w, h, state, png));

  ASSERT_EQUALS(0x1f, image[194]);
  ASSERT_EQUALS(0xf9, image[195]);

  image.clear();
  state = lodepng::State();

  // Decode from palette image to 16-bit per channel RGBA
  base64 = "iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHAgMAAAC5PL9AAAAABGdBTUEAAYagMeiWXwAAAANzQklU"
           "BAQEd/i1owAAAAxQTFRF/wB3AP93//8AAAD/G0OznAAAABpJREFUeJxj+P+H4WoMw605DDfmgEgg"
           "+/8fAHF5CrkeXW0HAAAAAElFTkSuQmCC"; //s07n3p02.png
  png.clear();
  fromBase64(png, base64);
  image.clear();
  state = lodepng::State();
  state.info_raw.bitdepth = 16;
  assertNoPNGError(lodepng::decode(image, w, h, state, png));
  ASSERT_EQUALS(0x77, image[84]);
  ASSERT_EQUALS(0x77, image[85]);
}

void testPredefinedFilters() {
  size_t w = 32, h = 32;
  std::cout << "testPredefinedFilters" << std::endl;
  Image image;
  generateTestImage(image, w, h, LCT_RGBA, 8);

  // everything to filter type '3'
  std::vector<unsigned char> predefined(h, 3);
  lodepng::State state;
  state.encoder.filter_strategy = LFS_PREDEFINED;
  state.encoder.filter_palette_zero = 0;
  state.encoder.predefined_filters = &predefined[0];

  std::vector<unsigned char> png;
  unsigned error = lodepng::encode(png, &image.data[0], w, h, state);
  assertNoError(error);

  std::vector<unsigned char> outfilters;
  error = lodepng::getFilterTypes(outfilters, png);
  assertNoError(error);

  ASSERT_EQUALS(outfilters.size(), h);
  for(size_t i = 0; i < h; i++) ASSERT_EQUALS(3, outfilters[i]);
}

void testWrongWindowSizeGivesError() {
  std::vector<unsigned char> png;
  unsigned w = 32, h = 32;
  Image image;
  generateTestImage(image, w, h);
  unsigned error = 0;

  lodepng::State state;
  state.encoder.zlibsettings.windowsize = 0;
  error = lodepng::encode(png, &image.data[0], w, h, state);
  ASSERT_EQUALS(60, error);
  state.encoder.zlibsettings.windowsize = 65536;
  error = lodepng::encode(png, &image.data[0], w, h, state);
  ASSERT_EQUALS(60, error);
  state.encoder.zlibsettings.windowsize = 1000; // not power of two
  error = lodepng::encode(png, &image.data[0], w, h, state);
  ASSERT_EQUALS(90, error);
  state.encoder.zlibsettings.windowsize = 256;
  error = lodepng::encode(png, &image.data[0], w, h, state);
  ASSERT_EQUALS(0, error);
}

void addColor(std::vector<unsigned char>& colors, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  colors.push_back(r);
  colors.push_back(g);
  colors.push_back(b);
  colors.push_back(a);
}

void addColor16(std::vector<unsigned char>& colors, unsigned short r, unsigned short g, unsigned short b, unsigned short a)
{
  colors.push_back(r & 255);
  colors.push_back((r >> 8) & 255);
  colors.push_back(g & 255);
  colors.push_back((g >> 8) & 255);
  colors.push_back(b & 255);
  colors.push_back((b >> 8) & 255);
  colors.push_back(a & 255);
  colors.push_back((a >> 8) & 255);
}

// colors is in RGBA, inbitdepth must be 8 or 16, the amount of bits per channel.
// colortype and bitdepth are the expected values. insize is amount of pixels. So the amount of bytes is insize * 4 * (inbitdepth / 8)
void testAutoColorModel(const std::vector<unsigned char>& colors, unsigned inbitdepth, LodePNGColorType colortype, unsigned bitdepth, bool key)
{
  std::cout << "testAutoColorModel " << inbitdepth << " " << colortype << " " << bitdepth << " " << key << std::endl;
  size_t innum = colors.size() / 4 * inbitdepth / 8;
  size_t num = innum < 65536 ? 65536 : innum; // Make image bigger so the convert doesn't avoid palette due to small image.
  std::vector<unsigned char> colors2(num * 4 * (inbitdepth / 8));
  for(size_t i = 0; i < colors2.size(); i++) colors2[i] = colors[i % colors.size()];

  std::vector<unsigned char> png;
  lodepng::encode(png, colors2, num, 1, LCT_RGBA, inbitdepth);

  // now extract the color type it chose
  unsigned w, h;
  lodepng::State state;
  std::vector<unsigned char> decoded;
  lodepng::decode(decoded, w, h, state, png);
  ASSERT_EQUALS(num, w);
  ASSERT_EQUALS(1, h);
  ASSERT_EQUALS(colortype, state.info_png.color.colortype);
  ASSERT_EQUALS(bitdepth, state.info_png.color.bitdepth);
  ASSERT_EQUALS(key, state.info_png.color.key_defined);
  if(inbitdepth == 8) { for(size_t i = 0; i < colors.size(); i++) ASSERT_EQUALS(colors[i], decoded[i]); }
  else { for(size_t i = 0; i < colors.size() / 2; i++) ASSERT_EQUALS(colors[i * 2], decoded[i]); }
}

void testAutoColorModels()
{
  std::vector<unsigned char> grey1;
  for(size_t i = 0; i < 2; i++) addColor(grey1, i * 255, i * 255, i * 255, 255);
  testAutoColorModel(grey1, 8, LCT_GREY, 1, false);

  std::vector<unsigned char> grey2;
  for(size_t i = 0; i < 4; i++) addColor(grey2, i * 85, i * 85, i * 85, 255);
  testAutoColorModel(grey2, 8, LCT_GREY, 2, false);


  std::vector<unsigned char> grey4;
  for(size_t i = 0; i < 16; i++) addColor(grey4, i * 17, i * 17, i * 17, 255);
  testAutoColorModel(grey4, 8, LCT_GREY, 4, false);


  std::vector<unsigned char> grey8;
  for(size_t i = 0; i < 256; i++) addColor(grey8, i, i, i, 255);
  testAutoColorModel(grey8, 8, LCT_GREY, 8, false);


  std::vector<unsigned char> grey16;
  for(size_t i = 0; i < 257; i++) addColor16(grey16, i, i, i, 65535);
  testAutoColorModel(grey16, 16, LCT_GREY, 16, false);

  std::vector<unsigned char> palette;
  addColor(palette, 0, 0, 1, 255);
  testAutoColorModel(palette, 8, LCT_PALETTE, 1, false);
  addColor(palette, 0, 0, 2, 255);
  testAutoColorModel(palette, 8, LCT_PALETTE, 1, false);
  for(int i = 3; i <= 4; i++) addColor(palette, 0, 0, i, 255);
  testAutoColorModel(palette, 8, LCT_PALETTE, 2, false);
  for(int i = 5; i <= 7; i++) addColor(palette, 0, 0, i, 255);
  testAutoColorModel(palette, 8, LCT_PALETTE, 4, false);
  for(int i = 8; i <= 17; i++) addColor(palette, 0, 0, i, 255);
  testAutoColorModel(palette, 8, LCT_PALETTE, 8, false);
  addColor(palette, 0, 0, 18, 0); // transparent
  testAutoColorModel(palette, 8, LCT_PALETTE, 8, false);
  addColor(palette, 0, 0, 18, 1); // translucent
  testAutoColorModel(palette, 8, LCT_PALETTE, 8, false);

  std::vector<unsigned char> rgb = grey8;
  addColor(rgb, 255, 0, 0, 255);
  testAutoColorModel(rgb, 8, LCT_RGB, 8, false);

  std::vector<unsigned char> rgb_key = rgb;
  addColor(rgb_key, 128, 0, 0, 0);
  testAutoColorModel(rgb_key, 8, LCT_RGB, 8, true);

  std::vector<unsigned char> rgb_key2 = rgb_key;
  addColor(rgb_key2, 128, 0, 0, 255); // same color but opaque ==> no more key
  testAutoColorModel(rgb_key2, 8, LCT_RGBA, 8, false);

  std::vector<unsigned char> rgb_key3 = rgb_key;
  addColor(rgb_key3, 128, 0, 0, 255); // semi-translucent ==> no more key
  testAutoColorModel(rgb_key3, 8, LCT_RGBA, 8, false);

  std::vector<unsigned char> rgb_key4 = rgb_key;
  addColor(rgb_key4, 128, 0, 0, 255);
  addColor(rgb_key4, 129, 0, 0, 255); // two different transparent colors ==> no more key
  testAutoColorModel(rgb_key4, 8, LCT_RGBA, 8, false);

  std::vector<unsigned char> grey1_key = grey1;
  grey1_key[7] = 0;
  testAutoColorModel(grey1_key, 8, LCT_GREY, 1, true);

  std::vector<unsigned char> grey2_key = grey2;
  grey2_key[7] = 0;
  testAutoColorModel(grey2_key, 8, LCT_GREY, 2, true);

  std::vector<unsigned char> grey4_key = grey4;
  grey4_key[7] = 0;
  testAutoColorModel(grey4_key, 8, LCT_GREY, 4, true);

  std::vector<unsigned char> grey8_key = grey8;
  grey8_key[7] = 0;
  testAutoColorModel(grey8_key, 8, LCT_GREY, 8, true);

  std::vector<unsigned char> small16;
  addColor16(small16, 1, 0, 0, 65535);
  testAutoColorModel(small16, 16, LCT_RGB, 16, false);

  std::vector<unsigned char> small16a;
  addColor16(small16a, 1, 0, 0, 1);
  testAutoColorModel(small16a, 16, LCT_RGBA, 16, false);

  std::vector<unsigned char> not16;
  addColor16(not16, 257, 257, 257, 0);
  testAutoColorModel(not16, 16, LCT_PALETTE, 1, false);

  std::vector<unsigned char> alpha16;
  addColor16(alpha16, 257, 0, 0, 10000);
  testAutoColorModel(alpha16, 16, LCT_RGBA, 16, false);
}

void testPaletteToPaletteDecode() {
  std::cout << "testPaletteToPaletteDecode" << std::endl;
  // It's a bit big for a 2x2 image... but this tests needs one with 256 palette entries in it.
  std::string base64 = "iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAMAAABFaP0WAAAAA3NCSVQICAjb4U/gAAADAFBMVEUA"
                       "AAAAADMAAGYAAJkAAMwAAP8AMwAAMzMAM2YAM5kAM8wAM/8AZgAAZjMAZmYAZpkAZswAZv8AmQAA"
                       "mTMAmWYAmZkAmcwAmf8AzAAAzDMAzGYAzJkAzMwAzP8A/wAA/zMA/2YA/5kA/8wA//8zAAAzADMz"
                       "AGYzAJkzAMwzAP8zMwAzMzMzM2YzM5kzM8wzM/8zZgAzZjMzZmYzZpkzZswzZv8zmQAzmTMzmWYz"
                       "mZkzmcwzmf8zzAAzzDMzzGYzzJkzzMwzzP8z/wAz/zMz/2Yz/5kz/8wz//9mAABmADNmAGZmAJlm"
                       "AMxmAP9mMwBmMzNmM2ZmM5lmM8xmM/9mZgBmZjNmZmZmZplmZsxmZv9mmQBmmTNmmWZmmZlmmcxm"
                       "mf9mzABmzDNmzGZmzJlmzMxmzP9m/wBm/zNm/2Zm/5lm/8xm//+ZAACZADOZAGaZAJmZAMyZAP+Z"
                       "MwCZMzOZM2aZM5mZM8yZM/+ZZgCZZjOZZmaZZpmZZsyZZv+ZmQCZmTOZmWaZmZmZmcyZmf+ZzACZ"
                       "zDOZzGaZzJmZzMyZzP+Z/wCZ/zOZ/2aZ/5mZ/8yZ///MAADMADPMAGbMAJnMAMzMAP/MMwDMMzPM"
                       "M2bMM5nMM8zMM//MZgDMZjPMZmbMZpnMZszMZv/MmQDMmTPMmWbMmZnMmczMmf/MzADMzDPMzGbM"
                       "zJnMzMzMzP/M/wDM/zPM/2bM/5nM/8zM////AAD/ADP/AGb/AJn/AMz/AP//MwD/MzP/M2b/M5n/"
                       "M8z/M///ZgD/ZjP/Zmb/Zpn/Zsz/Zv//mQD/mTP/mWb/mZn/mcz/mf//zAD/zDP/zGb/zJn/zMz/"
                       "zP///wD//zP//2b//5n//8z///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                       "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                       "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABlenwdAAABAHRSTlP/////////////////////////"
                       "////////////////////////////////////////////////////////////////////////////"
                       "////////////////////////////////////////////////////////////////////////////"
                       "////////////////////////////////////////////////////////////////////////////"
                       "//////////////////////////////////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                       "AAAAAAAAAAAAG8mZagAAAAlwSFlzAAAOTQAADpwB3vacVwAAAA5JREFUCJlj2CLHwHodAATjAa+k"
                       "lTE5AAAAAElFTkSuQmCC";
  std::vector<unsigned char> png;
  fromBase64(png, base64);

  std::vector<unsigned char> image;
  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, png, LCT_PALETTE, 8);
  ASSERT_EQUALS(0, error);
  ASSERT_EQUALS(2, width);
  ASSERT_EQUALS(2, height);
  ASSERT_EQUALS(180, image[0]);
  ASSERT_EQUALS(30, image[1]);
  ASSERT_EQUALS(5, image[2]);
  ASSERT_EQUALS(215, image[3]);
}

//2-bit palette
void testPaletteToPaletteDecode2() {
  std::cout << "testPaletteToPaletteDecode2" << std::endl;
  std::string base64 = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgAgMAAAAOFJJnAAAADFBMVEX/AAAA/wAAAP/////7AGD2AAAAE0lEQVR4AWMQhAKG3VCALDIqAgDl2WYBCQHY9gAAAABJRU5ErkJggg==";
  std::vector<unsigned char> png;
  fromBase64(png, base64);

  std::vector<unsigned char> image;
  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, png, LCT_PALETTE, 8);
  ASSERT_EQUALS(0, error);
  ASSERT_EQUALS(32, width);
  ASSERT_EQUALS(32, height);
  ASSERT_EQUALS(0, image[0]);
  ASSERT_EQUALS(1, image[1]);

  //Now add a user-specified output palette, that differs from the input palette. That should give error 82.
  LodePNGState state;
  lodepng_state_init(&state);
  state.info_raw.colortype = LCT_PALETTE;
  state.info_raw.bitdepth = 8;
  lodepng_palette_add(&state.info_raw, 0, 0, 0, 255);
  lodepng_palette_add(&state.info_raw, 1, 1, 1, 255);
  lodepng_palette_add(&state.info_raw, 2, 2, 2, 255);
  lodepng_palette_add(&state.info_raw, 3, 3, 3, 255);
  unsigned char* image2 = 0;
  unsigned error2 = lodepng_decode(&image2, &width, &height, &state, &png[0], png.size());
  ASSERT_EQUALS(82, error2);
  lodepng_state_cleanup(&state);
  free(image2);
}

void doMain()
{
  //PNG
  testPNGCodec();
  testPngSuiteTiny();
  testPaletteFilterTypesZero();
  testComplexPNG();
  testPredefinedFilters();
  testFuzzing();
  testWrongWindowSizeGivesError();
  testPaletteToPaletteDecode();
  testPaletteToPaletteDecode2();

  //Colors
  testColorKeyConvert();
  testColorConvert();
  testColorConvert2();
  testPaletteToPaletteConvert();
  testRGBToPaletteConvert();
  test16bitColorEndianness();
  testAutoColorModels();
  testNoAutoConvert();

  //Zlib
  testCompressZlib();
  testHuffmanCodeLengths();
  testCustomZlibCompress();
  testCustomZlibCompress2();
  testCustomDeflate();
  testCustomZlibDecompress();
  testCustomInflate();

  //lodepng_util
  testChunkUtil();

  std::cout << "\ntest successful" << std::endl;
}

int main()
{
  try
  {
    doMain();
  }
  catch(...)
  {
    std::cout << "error!" << std::endl;
  }

  return 0;
}
