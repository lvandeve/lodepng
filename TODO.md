TODO:
[.] test if there are no memory leaks or security exploits - done a lot but needs to be checked often
[.] check compatibility with various compilers  - done but needs to be redone for every newer version
[X] converting color to 16-bit per channel types
[X] support color profile chunk types (but never let them touch RGB values by default)
[ ] support all second edition public PNG chunk types (almost done except sPLT and hIST)
[X] support non-animation third edition public PNG chunk types: eXIf, cICP, mDCV, cLLI
[ ] make sure encoder generates no chunks with size > (2^31)-1
[ ] partial decoding (stream processing)
[X] let the "isFullyOpaque" function check color keys and transparent palettes too
[X] better name for the variables "codes", "codesD", "codelengthcodes", "clcl" and "lldl"
[ ] allow treating some errors like warnings, when image is recoverable (e.g. 69, 57, 58)
[ ] make warnings like: oob palette, checksum fail, data after iend, wrong/unknown crit chunk, no null terminator in text, ...
[ ] error messages with line numbers (and version)
[ ] errors in state instead of as return code?
[ ] new errors/warnings like suspiciously big decompressed ztxt or iccp chunk
[ ] let the C++ wrapper catch exceptions coming from the standard library and return LodePNG error codes
[ ] allow user to provide custom color conversion functions, e.g. for premultiplied alpha, padding bits or not, ...
[ ] allow user to give data (void*) to custom allocator
[X] provide alternatives for C library functions not present on some platforms (memcpy, ...)

