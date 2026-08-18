# Grimar Engine

Grimar is a lightweight **2D game engine** written in **C++20**.

This repository contains the engine core and a small sandbox application used
to test and develop engine features.

---

## Build

- **Language:** C++20
- **Build System:** CMake
- **Dependencies:** SDL2, SDL2_image and nlohmann-json
- **Platform:** Windows, Linux and macOS (MSVC, GCC or Clang)

Dependencies are managed using **vcpkg (manifest mode)**.

## Build on Windows or Linux

The same CMake workflow is used on every platform. From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

With CMake 3.25+, the equivalent preset commands are:

```sh
cmake --preset vcpkg-debug
cmake --build --preset vcpkg-debug
ctest --preset vcpkg-debug
```

In Windows PowerShell, use `$env:VCPKG_ROOT` or pass the full path to
`vcpkg.cmake`. Visual Studio, Ninja, GCC and Clang are supported CMake
generators. The sample assets are copied beside each executable at build time,
so IDE and command-line launches use the same paths.

Native Linux packages are supported when they provide CMake or pkg-config
metadata; vcpkg remains the most reproducible option.

---

## Project Structure
