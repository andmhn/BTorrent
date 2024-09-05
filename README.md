## Author

👋 Hi, I'm Mohan!

## System Requirements

To get started with this project, ensure you have the following tools and libraries:

- **CMake (Version 3.14 or later):** Essential for building and managing the project files.
- **C++ Compiler:** Compatible with Clang, GCC, or MSVC. Choose one based on your development environment.
- **GTK3 Development Libraries (for Linux users):** Necessary for GUI development on Linux platforms.

## Building the Project

# Debug

Follow these steps to build the project:

1. **Create a build directory & configure the build:**
   ```bash
   cmake -S. -Bbuild
   ```

2. **Build the project:**
   ```bash
   cmake --build build -j
   ```

This will create a `build` directory and compile all necessary artifacts there. The main executable will be located in `build/`.

# Release

For release build use `--config Release` on Windows:

```bash
cmake -S. -Bbuild
cmake --build build -j --config Release
```

Artifacts for both configurations will be generated in the `build` directory.

On Mac or Linux you'll need to maintain two build trees:

```bash
cmake -S. -Bbuild/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug -j

cmake -S. -Bbuild/Release -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release -j
```

# Testing just the core

If you only wanted to test there is no need to build the whole UI.

```
cmake -S. -Bbuild/Test -DCMAKE_BUILD_TYPE=Test
cmake --build build/Test -j

./bin/Test/bt_test
```

## License

MIT License. Can be used in closed-source commercial products.
