LodePNG Documentation
=====================

# 1. about

PNG is a file format to store raster images losslessly with good compression,
supporting different color types and alpha channel.

LodePNG is a PNG codec according to the Portable Network Graphics (PNG)
Specification (Second Edition) - W3C Recommendation 10 November 2003.

The specifications used are:

* [Portable Network Graphics (PNG) Specification (Second Edition)](http://www.w3.org/TR/2003/REC-PNG-20031110)
* [RFC 1950 ZLIB Compressed Data Format version 3.3](http://www.gzip.org/zlib/rfc-zlib.html)
* [RFC 1951 DEFLATE Compressed Data Format Specification ver 1.3](http://www.gzip.org/zlib/rfc-deflate.html)

The most recent version of LodePNG can currently be found at
[http://lodev.org/lodepng/](lodev.org)

LodePNG works both in C (ISO C90) and C++, with a C++ wrapper that adds
extra functionality.

LodePNG exists out of two files:

* lodepng.h: the header file for both C and C++
* lodepng.c(pp): give it the name lodepng.c or lodepng.cpp (or .cc) depending on your usage

If you want to start using LodePNG right away without reading this doc, get the
examples from the LodePNG website to see how to use it in code, or check the
smaller examples in chapter 13 here.

LodePNG is simple but only supports the basic requirements. To achieve
simplicity, the following design choices were made: There are no dependencies
on any external library. There are functions to decode and encode a PNG with
a single function call, and extended versions of these functions taking a
LodePNGState struct allowing to specify or get more information. By default
the colors of the raw image are always RGB or RGBA, no matter what color type
the PNG file uses. To read and write files, there are simple functions to
convert the files to/from buffers in memory.

This all makes LodePNG suitable for loading textures in games, demos and small
programs, ... It's less suitable for full fledged image editors, loading PNGs
over network (it requires all the image data to be available before decoding can
begin), life-critical systems, ...

## 1.1. supported features

The following features are supported by the decoder:

* decoding of PNGs with any color type, bit depth and interlace mode, to a 24- or 32-bit color raw image,
   or the same color type as the PNG
* encoding of PNGs, from any raw image to 24- or 32-bit color, or the same color type as the raw image
* Adam7 interlace and deinterlace for any color type
* loading the image from harddisk or decoding it from a buffer from other sources than harddisk
* support for alpha channels, including RGBA color model, translucent palettes and color keying
* zlib decompression (inflate)
* zlib compression (deflate)
* CRC32 and ADLER32 checksums
* handling of unknown chunks, allowing making a PNG editor that stores custom and unknown chunks.
* the following chunks are supported (generated/interpreted) by both encoder and decoder:
  *  IHDR: header information
  *  PLTE: color palette
  *  IDAT: pixel data
  *  IEND: the final chunk
  *  tRNS: transparency for palettized images
  *  tEXt: textual information
  *  zTXt: compressed textual information
  *  iTXt: international textual information
  *  bKGD: suggested background color
  *  pHYs: physical dimensions
  *  tIME: modification time

## 1.2. features not supported

The following features are _not_ supported:

* some features needed to make a conformant PNG-Editor might be still missing.
* partial loading/stream processing. All data must be available and is processed in one call.
* The following public chunks are not supported but treated as unknown chunks by LodePNG
    cHRM, gAMA, iCCP, sRGB, sBIT, hIST, sPLT
   Some of these are not supported on purpose: LodePNG wants to provide the RGB values
   stored in the pixels, not values modified by system dependent gamma or color models.


# 2. C and C++ version

The C version uses buffers allocated with alloc that you need to free()
yourself. You need to use init and cleanup functions for each struct whenever
using a struct from the C version to avoid exploits and memory leaks.

The C++ version has extra functions with ```std::vectors``` in the interface and the
```lodepng::State``` class which is a ```LodePNGState``` with constructor and destructor.

These files work without modification for both C and C++ compilers because all
the additional C++ code is in ```#ifdef __cplusplus``` blocks that make C-compilers
ignore it, and the C code is made to compile both with strict ISO C90 and C++.

To use the C++ version, you need to rename the source file to lodepng.cpp
(instead of lodepng.c), and compile it with a C++ compiler.

To use the C version, you need to rename the source file to lodepng.c (instead
of lodepng.cpp), and compile it with a C compiler.


# 3. Security

Even if carefully designed, it's always possible that LodePNG contains possible
exploits. If you discover one, please let me know, and it will be fixed.

When using LodePNG, care has to be taken with the C version of LodePNG, as well
as the C-style structs when working with C++. The following conventions are used
for all C-style structs:

* if a struct has a corresponding init function, always call the init function when making a new one
* if a struct has a corresponding cleanup function, call it before the struct disappears to avoid memory leaks
* if a struct has a corresponding copy function, use the copy function instead of "=".
 The destination must also be inited already.


# 4. Decoding

Decoding converts a PNG compressed image to a raw pixel buffer.

Most documentation on using the decoder is at its declarations in the header
above. For C, simple decoding can be done with functions such as
```lodepng_decode32```, and more advanced decoding can be done with the struct
```LodePNGState``` and ```lodepng_decode```. For C++, all decoding can be done with the
various lodepng::decode functions, and ```lodepng::State``` can be used for advanced
features.

When using the ```LodePNGState```, it uses the following fields for decoding:

* ```LodePNGInfo info_png```: it stores extra information about the PNG (the input) in here
* ```LodePNGColorMode info_raw```: here you can say what color mode of the raw image (the output) you want to get
* ```LodePNGDecoderSettings decoder```: you can specify a few extra settings for the decoder to use

### LodePNGInfo info_png

After decoding, this contains extra information of the PNG image, except the actual
pixels, width and height because these are already gotten directly from the decoder
functions.

It contains for example the original color type of the PNG image, text comments,
suggested background color, etc... More details about the ```LodePNGInfo``` struct are
at its declaration documentation.

### LodePNGColorMode info_raw

When decoding, here you can specify which color type you want
the resulting raw image to be. If this is different from the colortype of the
PNG, then the decoder will automatically convert the result. This conversion
always works, except if you want it to convert a color PNG to greyscale or to
a palette with missing colors.

By default, 32-bit color is used for the result.

### LodePNGDecoderSettings decoder

The settings can be used to ignore the errors created by invalid CRC and Adler32
chunks, and to disable the decoding of tEXt chunks.

There's also a setting ```color_convert```, true by default. If false, no conversion
is done, the resulting data will be as it was in the PNG (after decompression)
and you'll have to puzzle the colors of the pixels together yourself using the
color type information in the ```LodePNGInfo```.


# 5. Encoding

Encoding converts a raw pixel buffer to a PNG compressed image.

Most documentation on using the encoder is at its declarations in the header
above. For C, simple encoding can be done with functions such as
```lodepng_encode32```, and more advanced decoding can be done with the struct
```LodePNGState``` and ```lodepng_encode```. For C++, all encoding can be done with the
various ```lodepng::encode``` functions, and ```lodepng::State``` can be used for advanced
features.

Like the decoder, the encoder can also give errors. However it gives less errors
since the encoder input is trusted, the decoder input (a PNG image that could
be forged by anyone) is not trusted.

When using the ```LodePNGState```, it uses the following fields for encoding:

* ```LodePNGInfo info_png```: here you specify how you want the PNG (the output) to be.
* ```LodePNGColorMode info_raw```: here you say what color type of the raw image (the input) has
* ```LodePNGEncoderSettings encoder```: you can specify a few settings for the encoder to use

### LodePNGInfo info_png

When encoding, you use this the opposite way as when decoding: for encoding,
you fill in the values you want the PNG to have before encoding. By default it's
not needed to specify a color type for the PNG since it's automatically chosen,
but it's possible to choose it yourself given the right settings.

The encoder will not always exactly match the ```LodePNGInfo``` struct you give,
it tries as close as possible. Some things are ignored by the encoder. The
encoder uses, for example, the following settings from it when applicable:
colortype and bitdepth, text chunks, time chunk, the color key, the palette, the
background color, the interlace method, unknown chunks, ...

When encoding to a PNG with colortype 3, the encoder will generate a PLTE chunk.
If the palette contains any colors for which the alpha channel is not 255 (so
there are translucent colors in the palette), it'll add a tRNS chunk.

### LodePNGColorMode info_raw

You specify the color type of the raw image that you give to the input here,
including a possible transparent color key and palette you happen to be using in
your raw image data.

By default, 32-bit color is assumed, meaning your input has to be in RGBA
format with 4 bytes (```unsigned chars```) per pixel.

### LodePNGEncoderSettings encoder

The following settings are supported (some are in sub-structs):

* ```auto_convert```: when this option is enabled, the encoder will
   automatically choose the smallest possible color mode (including color key) that
   can encode the colors of all pixels without information loss.
* ```btype```: the block type for LZ77. 0 = uncompressed, 1 = fixed huffman tree,
   2 = dynamic huffman tree (best compression). Should be 2 for proper
   compression.
* ```use_lz77```: whether or not to use LZ77 for compressed block types. Should be
   true for proper compression.
* ```windowsize```: the window size used by the LZ77 encoder (1 - 32768). Has value
   2048 by default, but can be set to 32768 for better, but slow, compression.
* ```force_palette```: if colortype is 2 or 6, you can make the encoder write a PLTE
   chunk if ```force_palette``` is true. This can used as suggested palette to convert
   to by viewers that don't support more than 256 colors (if those still exist)
* ```add_id```: add text chunk "Encoder: LodePNG <version>" to the image.
* ```text_compression```: default 1. If 1, it'll store texts as zTXt instead of tEXt chunks.
  zTXt chunks use zlib compression on the text. This gives a smaller result on
  large texts but a larger result on small texts (such as a single program name).
  It's all tEXt or all zTXt though, there's no separate setting per text yet.


# 6. color conversions

An important thing to note about LodePNG, is that the color type of the PNG, and
the color type of the raw image, are completely independent. By default, when
you decode a PNG, you get the result as a raw image in the color type you want,
no matter whether the PNG was encoded with a palette, greyscale or RGBA color.
And if you encode an image, by default LodePNG will automatically choose the PNG
color type that gives good compression based on the values of colors and amount
of colors in the image. It can be configured to let you control it instead as
well, though.

To be able to do this, LodePNG does conversions from one color mode to another.
It can convert from almost any color type to any other color type, except the
following conversions: RGB to greyscale is not supported, and converting to a
palette when the palette doesn't have a required color is not supported. This is
not supported on purpose: this is information loss which requires a color
reduction algorithm that is beyong the scope of a PNG encoder (yes, RGB to grey
is easy, but there are multiple ways if you want to give some channels more
weight).

By default, when decoding, you get the raw image in 32-bit RGBA or 24-bit RGB
color, no matter what color type the PNG has. And by default when encoding,
LodePNG automatically picks the best color model for the output PNG, and expects
the input image to be 32-bit RGBA or 24-bit RGB. So, unless you want to control
the color format of the images yourself, you can skip this chapter.

## 6.1. PNG color types

A PNG image can have many color types, ranging from 1-bit color to 64-bit color,
as well as palettized color modes. After the zlib decompression and unfiltering
in the PNG image is done, the raw pixel data will have that color type and thus
a certain amount of bits per pixel. If you want the output raw image after
decoding to have another color type, a conversion is done by LodePNG.

The PNG specification gives the following color types:

* 0: greyscale, bit depths 1, 2, 4, 8, 16
* 2: RGB, bit depths 8 and 16
* 3: palette, bit depths 1, 2, 4 and 8
* 4: greyscale with alpha, bit depths 8 and 16
* 6: RGBA, bit depths 8 and 16

Bit depth is the amount of bits per pixel per color channel. So the total amount
of bits per pixel is: amount of channels * bitdepth.

## 6.2. color conversions

As explained in the sections about the encoder and decoder, you can specify
color types and bit depths in ```info_png``` and ```info_raw``` to change the default
behaviour.

If, when decoding, you want the raw image to be something else than the default,
you need to set the color type and bit depth you want in the ```LodePNGColorMode```,
or the parameters colortype and bitdepth of the simple decoding function.

If, when encoding, you use another color type than the default in the raw input
image, you need to specify its color type and bit depth in the ```LodePNGColorMode```
of the raw image, or use the parameters colortype and bitdepth of the simple
encoding function.

If, when encoding, you don't want LodePNG to choose the output PNG color type
but control it yourself, you need to set auto_convert in the encoder settings
to false, and specify the color type you want in the ```LodePNGInfo``` of the
encoder (including palette: it can generate a palette if ```auto_convert``` is true,
otherwise not).

If the input and output color type differ (whether user chosen or auto chosen),
LodePNG will do a color conversion, which follows the rules below, and may
sometimes result in an error.

To avoid some confusion:

* the decoder converts from PNG to raw image
* the encoder converts from raw image to PNG
* the colortype and bitdepth in ```LodePNGColorMode info_raw```, are those of the raw image
* the colortype and bitdepth in the color field of ```LodePNGInfo info_png```, are those of the PNG
* when encoding, the color type in ```LodePNGInfo``` is ignored if ```auto_convert```
 is enabled, it is automatically generated instead
* when decoding, the color type in ```LodePNGInfo``` is set by the decoder to that of the original
 PNG image, but it can be ignored since the raw image has the color type you requested instead
* if the color type of the ```LodePNGColorMode``` and PNG image aren't the same, a conversion
 between the color types is done if the color types are supported. If it is not
 supported, an error is returned. If the types are the same, no conversion is done.
* even though some conversions aren't supported, LodePNG supports loading PNGs from any
 colortype and saving PNGs to any colortype, sometimes it just requires preparing
 the raw image correctly before encoding.
* both encoder and decoder use the same color converter.

Non supported color conversions:

* color to greyscale: no error is thrown, but the result will look ugly because
only the red channel is taken
* anything to palette when that palette does not have that color in it: in this
case an error is thrown

Supported color conversions:

* anything to 8-bit RGB, 8-bit RGBA, 16-bit RGB, 16-bit RGBA
* any grey or grey+alpha, to grey or grey+alpha
* anything to a palette, as long as the palette has the requested colors in it
* removing alpha channel
* higher to smaller bitdepth, and vice versa

If you want no color conversion to be done (e.g. for speed or control):

* In the encoder, you can make it save a PNG with any color type by giving the
raw color mode and ```LodePNGInfo``` the same color mode, and setting ```auto_convert``` to
false.
* In the decoder, you can make it store the pixel data in the same color type
as the PNG has, by setting the ```color_convert``` setting to false. Settings in
```info_raw``` are then ignored.

The function ```lodepng_convert``` does the color conversion. It is available in the
interface but normally isn't needed since the encoder and decoder already call
it.

## 6.3. padding bits

In the PNG file format, if a less than 8-bit per pixel color type is used and the scanlines
have a bit amount that isn't a multiple of 8, then padding bits are used so that each
scanline starts at a fresh byte. But that is NOT true for the LodePNG raw input and output.
The raw input image you give to the encoder, and the raw output image you get from the decoder
will NOT have these padding bits, e.g. in the case of a 1-bit image with a width
of 7 pixels, the first pixel of the second scanline will the the 8th bit of the first byte,
not the first bit of a new byte.

## 6.4. A note about 16-bits per channel and endianness

LodePNG uses ```unsigned char``` arrays for 16-bit per channel colors too, just like
for any other color format. The 16-bit values are stored in big endian (most
significant byte first) in these arrays. This is the opposite order of the
little endian used by x86 CPU's.

LodePNG always uses big endian because the PNG file format does so internally.
Conversions to other formats than PNG uses internally are not supported by
LodePNG on purpose, there are myriads of formats, including endianness of 16-bit
colors, the order in which you store R, G, B and A, and so on. Supporting and
converting to/from all that is outside the scope of LodePNG.

This may mean that, depending on your use case, you may want to convert the big
endian output of LodePNG to little endian with a for loop. This is certainly not
always needed, many applications and libraries support big endian 16-bit colors
anyway, but it means you cannot simply cast the ```unsigned char*``` buffer to an
```unsigned short*``` buffer on x86 CPUs.


# 7. error values

All functions in LodePNG that return an error code, return 0 if everything went
OK, or a non-zero code if there was an error.

The meaning of the LodePNG error values can be retrieved with the function
```lodepng_error_text```: given the numerical error code, it returns a description
of the error in English as a string.

Check the implementation of ```lodepng_error_text``` to see the meaning of each code.


# 8. chunks and PNG editing

If you want to add extra chunks to a PNG you encode, or use LodePNG for a PNG
editor that should follow the rules about handling of unknown chunks, or if your
program is able to read other types of chunks than the ones handled by LodePNG,
then that's possible with the chunk functions of LodePNG.

A PNG chunk has the following layout:

```
4 bytes length
4 bytes type name
length bytes data
4 bytes CRC
```

## 8.1. iterating through chunks

If you have a buffer containing the PNG image data, then the first chunk (the
IHDR chunk) starts at byte number 8 of that buffer. The first 8 bytes are the
signature of the PNG and are not part of a chunk. But if you start at byte 8
then you have a chunk, and can check the following things of it.

NOTE: none of these functions check for memory buffer boundaries. To avoid
exploits, always make sure the buffer contains all the data of the chunks.
When using ```lodepng_chunk_next```, make sure the returned value is within the
allocated memory.

```
unsigned lodepng_chunk_length(const unsigned char* chunk)
```

Get the length of the chunk's data. The total chunk length is this length + 12.

```
void lodepng_chunk_type(char type[5], const unsigned char* chunk)
unsigned char lodepng_chunk_type_equals(const unsigned char* chunk, const char* type)
```

Get the type of the chunk or compare if it's a certain type

```
unsigned char lodepng_chunk_critical(const unsigned char* chunk)
unsigned char lodepng_chunk_private(const unsigned char* chunk)
unsigned char lodepng_chunk_safetocopy(const unsigned char* chunk)
```

Check if the chunk is critical in the PNG standard (only IHDR, PLTE, IDAT and IEND are).
Check if the chunk is private (public chunks are part of the standard, private ones not).
Check if the chunk is safe to copy. If it's not, then, when modifying data in a critical
chunk, unsafe to copy chunks of the old image may NOT be saved in the new one if your
program doesn't handle that type of unknown chunk.


```
unsigned char* lodepng_chunk_data(unsigned char* chunk)
const unsigned char* lodepng_chunk_data_const(const unsigned char* chunk)
```

Get a pointer to the start of the data of the chunk.

```
unsigned lodepng_chunk_check_crc(const unsigned char* chunk)
void lodepng_chunk_generate_crc(unsigned char* chunk)
```

Check if the crc is correct or generate a correct one.

```
unsigned char* lodepng_chunk_next(unsigned char* chunk)
const unsigned char* lodepng_chunk_next_const(const unsigned char* chunk)
```

Iterate to the next chunk. This works if you have a buffer with consecutive chunks. Note that these
functions do no boundary checking of the allocated data whatsoever, so make sure there is enough
data available in the buffer to be able to go to the next chunk.

```
unsigned lodepng_chunk_append(unsigned char** out, size_t* outlength, const unsigned char* chunk)
unsigned lodepng_chunk_create(unsigned char** out, size_t* outlength, unsigned length,
                              const char* type, const unsigned char* data)
```

These functions are used to create new chunks that are appended to the data in *out that has
length *outlength. The append function appends an existing chunk to the new data. The create
function creates a new chunk with the given parameters and appends it. Type is the 4-letter
name of the chunk.

## 8.2. chunks in info_png

The LodePNGInfo struct contains fields with the unknown chunk in it. It has 3
buffers (each with size) to contain 3 types of unknown chunks:
the ones that come before the PLTE chunk, the ones that come between the PLTE
and the IDAT chunks, and the ones that come after the IDAT chunks.
It's necessary to make the distionction between these 3 cases because the PNG
standard forces to keep the ordering of unknown chunks compared to the critical
chunks, but does not force any other ordering rules.

* ```info_png.unknown_chunks_data[0]``` is the chunks before PLTE
* ```info_png.unknown_chunks_data[1]``` is the chunks after PLTE, before IDAT
* ```info_png.unknown_chunks_data[2]``` is the chunks after IDAT

The chunks in these 3 buffers can be iterated through and read by using the same
way described in the previous subchapter.

When using the decoder to decode a PNG, you can make it store all unknown chunks
if you set the option ```settings.remember_unknown_chunks``` to 1. By default, this
option is off (0).

The encoder will always encode unknown chunks that are stored in the ```info_png```.
If you need it to add a particular chunk that isn't known by LodePNG, you can
use ```lodepng_chunk_append``` or ```lodepng_chunk_create``` to the chunk data in
```info_png.unknown_chunks_data[x]``` .

Chunks that are known by LodePNG should not be added in that way. E.g. to make
LodePNG add a bKGD chunk, set ```background_defined``` to true and add the correct
parameters there instead.


# 9. compiler support

No libraries other than the current standard C library are needed to compile
LodePNG. For the C++ version, only the standard C++ library is needed on top.
Add the files lodepng.c(pp) and lodepng.h to your project, include
lodepng.h where needed, and your program can read/write PNG files.

It is compatible with C90 and up, and C++03 and up.

If performance is important, use optimization when compiling! For both the
encoder and decoder, this makes a large difference.

Make sure that LodePNG is compiled with the same compiler of the same version
and with the same settings as the rest of the program, or the interfaces with
```std::vectors``` and ```std::strings``` in C++ can be incompatible.

```CHAR_BITS``` must be 8 or higher, because LodePNG uses unsigned chars for octets.

## gcc and g++

LodePNG is developed in gcc so this compiler is natively supported. It gives no
warnings with compiler options "-Wall -Wextra -pedantic -ansi", with gcc and g++
version 4.7.1 on Linux, 32-bit and 64-bit.

## Clang

Fully supported and warning-free.

## Mingw

The Mingw compiler (a port of gcc for Windows) should be fully supported by
LodePNG.

## Visual Studio and Visual C++ Express Edition

LodePNG should be warning-free with warning level W4. Two warnings were disabled
with pragmas though: warning 4244 about implicit conversions, and warning 4996
where it wants to use a non-standard function fopen_s instead of the standard C
fopen.

Visual Studio may want "stdafx.h" files to be included in each source file and
give an error "unexpected end of file while looking for precompiled header".
This is not standard C++ and will not be added to the stock LodePNG. You can
disable it for lodepng.cpp only by right clicking it, Properties, C/C++,
Precompiled Headers, and set it to Not Using Precompiled Headers there.

NOTE: Modern versions of VS should be fully supported, but old versions, e.g.
VS6, are not guaranteed to work.

## Compilers on Macintosh

LodePNG has been reported to work both with gcc and LLVM for Macintosh, both for
C and C++.

## Other Compilers

If you encounter problems on any compilers, feel free to let me know and I may
try to fix it if the compiler is modern and standards complient.


# 10. examples

This decoder example shows the most basic usage of LodePNG. More complex
examples can be found on the LodePNG website.

## 10.1. decoder C++ example


```C
#include "lodepng.h"
#include <iostream>

int main(int argc, char *argv[])
{
  const char* filename = argc > 1 ? argv[1] : "test.png";

  //load and decode
  std::vector<unsigned char> image;
  unsigned width, height;
  unsigned error = lodepng::decode(image, width, height, filename);

  //if there's an error, display it
  if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

  //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}
```

## 10.2. decoder C example

```C
#include "lodepng.h"

int main(int argc, char *argv[])
{
  unsigned error;
  unsigned char* image;
  size_t width, height;
  const char* filename = argc > 1 ? argv[1] : "test.png";

  error = lodepng_decode32_file(&image, &width, &height, filename);

  if(error) printf("decoder error %u: %s\n", error, lodepng_error_text(error));

  / * use image here * /

  free(image);
  return 0;
}
```

# 11. changes

See lodepng.h

# 12. contact information

Feel free to contact me with suggestions, problems, comments, ... concerning
LodePNG. If you encounter a PNG image that doesn't work properly with this
decoder, feel free to send it and I'll use it to find and fix the problem.

My email address is (puzzle the account and domain together with an @ symbol):
Domain: gmail dot com.
Account: lode dot vandevenne.

---
Copyright (c) 2005-2014 Lode Vandevenne