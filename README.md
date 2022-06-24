# CWCC Compiler

CWCC is a lightweight C compiler designed for [audr32](https://github.com/RaidTheWeb/audr32) following [acwj](https://github.com/DoctorWkt/acwj). CWCC supports a reasonable subset of C and can compile most programs for audr32.

## Building

Running `make` in the root of the repo will build audr32-cwcc and install to your `~/.local/bin/` directory, whilst also installing the standard headers to `/tmp/include/` (will definitely change in the future, probably take the GCC approach by embedding headers into the `~/.local/audr32-cwcc/include/` directory or something) so take caution to rebuild whenever `/tmp/` will end up cleared (reboots, etc.).

### Neovim Clangd

Run `make clean; bear -- make` to build the `compile_commands.json` for clangd in the root of the repo.

## License

<a href="https://opensource.org/licenses/MIT">
  <img align="right" height="96" alt="MIT License" src="mit-license.png" />
</a>

CWCC is licensed under the [MIT](https://opensource.org/licenses/MIT) license. As such the code is free to use in commercial, private and public use as long as credit to the contributors and the original MIT license is preserved.
