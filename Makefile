# This makefile only makes the unit test, benchmark and pngdetail and showpng
# utilities. It does not make the PNG codec itself as shared or static library.
# That is because:
# LodePNG itself has only 1 source file (lodepng.cpp, can be renamed to
# lodepng.c) and is intended to be included as source file in other projects and
# their build system directly.


CC ?= gcc
CXX ?= g++

override CFLAGS := -W -Wall -Wextra -ansi -pedantic -O3 -Wno-unused-function $(CFLAGS)
override CXXFLAGS := -W -Wall -Wextra -ansi -pedantic -O3 $(CXXFLAGS)

BIN       = unittest benchmark pngdetail showpng
STATICLIB = $(PREFIX)liblodepng.a
SHAREDLIB = $(PREFIX)liblodepng.so.0
SHAREDBIN  = pngdetail-shared pngreencode-shared pnglossy-shared pngshow-shared
LIBOBJS = lodepng.o lodepng_util.o

all: $(BIN)

%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) -I ./ $(CXXFLAGS) -c $< -o $@

unittest: lodepng.o lodepng_util.o lodepng_unittest.o
	$(CXX) $^ $(CXXFLAGS) -o $@

benchmark: lodepng.o lodepng_benchmark.o
	$(CXX) $^ $(CXXFLAGS) -lSDL -o $@

pngdetail: lodepng.o lodepng_util.o pngdetail.o
	$(CXX) $^ $(CXXFLAGS) -o $@

showpng: lodepng.o examples/example_sdl.o
	$(CXX) -I ./ $^ $(CXXFLAGS) -lSDL -o $@

shared: $(STATICLIB) $(SHAREDLIB) $(SHAREDBIN)

$(STATICLIB): $(LIBOBJS)
	$(AR) rcs $@ $^
	$(if $(RANLIB), $(RANLIB) $@)

$(SHAREDLIB): $(LIBOBJS)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(CFLAGS) $(LDFLAGS)

pngdetail-shared: $(SHAREDLIB) pngdetail.o
	$(CXX) $^ $(CXXFLAGS) -o $@

pngreencode-shared: $(SHAREDLIB) examples/example_reencode.o
	$(CXX) -I ./ $^ $(CXXFLAGS) -o $@

pnglossy-shared: $(SHAREDLIB) examples/example_lossy.o
	$(CXX) -I ./ $^ $(CXXFLAGS) -o $@

pngshow-shared: $(SHAREDLIB) examples/example_sdl.o
	$(CXX) -I ./ $^ $(CXXFLAGS) -lSDL -o $@

clean:
	rm -f $(BIN) $(STATICLIB) $(SHAREDLIB) $(SHAREDBIN) *.o examples/*.o
