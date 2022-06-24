# CWCC Compiler

CWCC is a lightweight C compiler designed for [audr32](https://github.com/RaidTheWeb/audr32) following [acwj](https://github.com/DoctorWkt/acwj). CWCC supports a reasonable subset of C and can compile most programs for audr32.

## Building

Running `make` in the root of the repo will build audr32-cwcc and install to your `~/.local/bin/` directory, whilst also installing the standard headers to `/tmp/include/` (will definitely change in the future, probably take the GCC approach by embedding headers into the `~/.local/audr32-cwcc/include/` directory or something) so take caution to rebuild whenever `/tmp/` will end up cleared (reboots, etc.).

### Neovim Clangd

Run `make clean; bear -- make` to build the `compile_commands.json` for clangd in the root of the repo.

## Using

### CLI Arguments

- `-v` toggle verbose
- `-S` generate assembly output only
- `-T` dump AST tree
- `-M` dump symbols
- `-o out` select the output file for executable
- `-b offset` select the base offset (relocating)
- `-s size` base size for output binary
- `-a file` add an assembly file to the list of files to compile against

### Kernels

Kernels and other binaries that require offsetting as they're loaded at different locations other than the default `0x40330000` (ROM) require compilation whilst passing in the `-b` option with a required base offset argument in hexadecimal.

Example: `audr32-cwcc -b 330000 -o ramstart.bin ramstart.c` (Compile C file so that it will act like everything is loaded at 0x00330000 (RAMSTART))

## License

<a href="https://opensource.org/licenses/MIT">
  <img align="right" height="96" alt="MIT License" src="mit-license.png" />
</a>

CWCC is licensed under the [MIT](https://opensource.org/licenses/MIT) license. As such the code is free to use in commercial, private and public use as long as credit to the contributors and the original MIT license is preserved.
