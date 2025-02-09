To run:

Install `cmake`

Run `cmake . --preset default`

TODO:
Either:
- Make macros compatible with MSVC
- Get CMake working with Ninja and a C compiler (maybe install it for Windows, use that first?)
	- Either this or make sure to use a version of ALL of these on the right platform.
	- I'm thinking prioritize MSYS
- Transfer over to Meson to put this all behind us