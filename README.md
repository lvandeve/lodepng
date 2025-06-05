LodePNG
-------

PNG encoder and decoder in C and C++, without dependencies

Home page: http://lodev.org/lodepng/

### Documentation

Detailed documentation is included in a large comment in the second half of the
header file `lodepng.h`.

Source code examples using LodePNG can be found in the examples directory.

An FAQ can be found on http://lodev.org/lodepng/

### Building

Only two files are needed to encode and decode PNGs:

* `lodepng.cpp` (or renamed to `lodepng.c`)
* `lodepng.h`

All other files are just source code examples, tests, misc utilities, etc...,
which are normally not needed in projects using this.

You can include the files directly in your project's source tree and its
makefile, IDE project file, or other build system. No library is necessary.

In addition to C++, LodePNG also supports ANSI C (C89), with all the same
functionality: C++ only adds extra convenience API.

For C, rename `lodepng.cpp` to `lodepng.c`.

Consider using git submodules to include LodePNG in your project.

### Compiling in C++

If you have a hypothetical `your_program.cpp` that #includes and uses `lodepng.h`,
you can build as follows:

`g++ your_program.cpp lodepng.cpp -Wall -Wextra -pedantic -ansi -O3`

or:

`clang++ your_program.cpp lodepng.cpp -Wall -Wextra -pedantic -ansi -O3`

This shows compiler flags it was designed for, but normally one would use the
compiler or build system of their project instead of those commands, and other
C++ compilers are supported.

### Compiling in C

Rename `lodepng.cpp` to `lodepng.c` for this.

If you have a hypothetical your_program.c that #includes and uses lodepng.h,
you can build as follows:

`gcc your_program.c lodepng.c -ansi -pedantic -Wall -Wextra -O3`

or

`clang your_program.c lodepng.c -ansi -pedantic -Wall -Wextra -O3`

This shows compiler flags it was designed for, but normally one would use the
compiler or build system of their project instead of those commands, and other
C compilers are supported.

### Makefile

There is a Makefile, but this is not intended for using LodePNG itself since the
way to use that one is to include its source files in your program. The Makefile
only builds development and testing utilities. It can be used as follows:

`make -j`

### CMakeLists.txt

There is a CMakeLists.txt but, like the Makefile is not intended for using LodePNG itself and builds only development and testing utilities. It can be used as follows

```bash
cmake -B build -D LODEPNG_WERROR=ON
cmake --build build [--target <benchmark|pngdetail|showpng|unittest>]
```

If SDL2 is not installed the `benchmark` and `showpng` targets will not be configured into the build. Use your system package manager to install the `sdl2` package or you can install [vcpkg](https://github.com/microsoft/vcpkg) and let it automatically build and install SDL2. To use vcpkg add the following to the first `cmake` command above:

```bash
-D CMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```
