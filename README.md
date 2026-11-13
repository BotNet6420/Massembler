# Massembler

**Massembler** is a two-pass assembler for the [Basic Computer](https://en.wikipedia.org/wiki/Mano_machine)
described by M. Morris Mano in *Computer System Architecture*.

It translates assembly source code into machine code for Mano's Basic Computer.

Massembler provides both:
- A command-line interface for assembling source files.
- A shared C library that can be integrated into other projects.

## Features

- Two-pass assembly.
- Symbol table for label resolution.
- Support for memory-reference instructions.
- Support for register-reference and input-output instructions.
- Support for assembler directives (`ORG`, `END`, `DEC`, `HEX`).
- Binary output generation.
- Diagnostic reporting for assembly errors.
- Shared library for integration with other C projects.

## Building

Massembler uses [CMake](https://cmake.org/) as its build system.

### Requirements

- A C compiler supporting C99.
- CMake 3.25.1 or newer.
- A compatible build tool (such as Ninja or Make).

### Build using the command line

Clone the repository:

```bash
git clone https://github.com/BotNet6420/Massembler
cd Massembler
```

Configure the project:

```bash
cmake -S . -B build
```

Build the project:

```bash
cmake --build build
```

The build produces two main targets:

| Target            | Description                              |
|-------------------|------------------------------------------|
| `massembler`      | Command-line assembler executable        |
| `massembler_core` | Shared library for use in other projects |

The exact output locations and library filenames depend on the platform and CMake generator.

## Command-Line Usage

```text
massembler <INPUT> [OUTPUT]
```

### Arguments

| Argument   | Description                                      |
|------------|--------------------------------------------------|
| `<INPUT>`  | Assembly source file to assemble.                |
| `[OUTPUT]` | Optional output file. Defaults to `program.bin`. |

### Examples

Assemble a source file using the default output filename:

```bash
./massembler program.asm
```

Specify an output filename:

```bash
./massembler program.asm program.bin
```

Display help:

```bash
./massembler --help
```

Display version information:

```bash
./massembler --version
```

If an output file already exists, it will be overwritten.

## Library Usage

Massembler also builds a shared library, `massembler_core`, which can be linked against other C projects.

The library exposes the assembler functionality independently of the command-line interface,
as well as functions to evaluate assembly symbols.

The public headers are available in the `include/` directory.

### Linking

Build Massembler using CMake, then link your project against the generated `massembler_core` library.

When using CMake in your own project, you can link against the library target if Massembler is included as a subdirectory:

```cmake
add_subdirectory(path/to/Massembler)

target_link_libraries(your_target
    PRIVATE massembler_core
)
```

You will also need to make sure the library can be found at runtime when using the shared-library build.

For usage details, refer to the public headers in `include/`.

## Technical Details

Massembler uses a two-pass assembly process:

1. **First pass:** Processes labels, tracks their memory locations, and builds the symbol table.
2. **Second pass:** Translates instructions and resolves symbol references into machine code.

The assembler targets Mano's Basic Computer, which uses 16-bit instructions and a 12-bit address space (4096 memory locations).

## Contributing

Contributions, bug reports, and suggestions are welcome.

If you find a bug or have an idea for an improvement, feel free to open an issue or submit a pull request.

## License

This project is licensed under the GNU General Public License v3.0.
See the [LICENSE](LICENSE) file for details.