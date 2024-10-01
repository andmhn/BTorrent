## Author

👋 Hi, I'm Anand Mohan!

## System Requirements

To get started with this project, ensure you have the following tools and libraries:

- **CMake (Version 3.14 or later):** Essential for building and managing the project files.
- **C++ Compiler:** Compatible with Clang, GCC, or MSVC. Choose one based on your development environment.
- **GTK3 Development Libraries (for Linux users):** Necessary for GUI development on Linux platforms.

## Building the Project

# Debug

Build as a normal cmake project

```
cmake -S. -Bbuild
cmake --build build -j
```

This will create a `build` directory and compile all necessary artifacts there.
The executables will be located in `build/bin`.

# Release

For release build use `--config Release` on Windows:

```
cmake -S. -Bbuild
cmake --build build -j --config Release
```

## License

MIT License. Can be used in closed-source commercial products.
