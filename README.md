# Cell Wars

A small real-time strategy prototype where fleets travel between connected bases, reinforce allies, and capture enemy territory.

Built in C with SDL3’s GPU API and Metal shaders.

## Gameplay

- Drag from one of your bases to a connected base to send roughly half its units.
- Send units to friendly bases to reinforce them.
- Attack neutral or enemy bases to reduce their health and capture them.
- Fleets from opposing factions clash when traveling along the same connection.
- Non-neutral bases regenerate one unit per second.

## Tech

- C17
- SDL3
- SDL GPU API
- Metal Shading Language (MSL)
- CMake
- Conan 2

## Requirements

- macOS with Metal support
- CMake 3.25+
- Conan 2.x
- A C compiler with C17 support

## Build and run

From the repository root:

```sh
conan install . --output-folder=build --build=missing

cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build

./build/out
```

> The game loads its `.metal` shader files relative to the repository root, so run the executable from there.

## Project layout

```text
src/
├── engine/       # Windowing, input, camera, render loop, and GPU renderer
├── game/         # Bases, factions, graph connections, fleets, and game logic
└── main.c        # Map setup and application entry point

*.metal           # Metal shaders for bases, edges, fleets, health, and drag lines
```

## Status

Cell Wars is an in-progress prototype focused on core RTS interactions, fleet movement, combat, and base capture.
