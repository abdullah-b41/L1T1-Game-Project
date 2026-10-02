# The Ultimate Golf

A 2D mini-golf game developed in C with [raylib](https://www.raylib.com/) for the BUET CSE 102 game project.

The game contains multiple themed golf levels with obstacles, animations, sound effects, textures, and a level-selection/gameplay flow. The complete source code and required game assets are included in this repository.

## Project Structure

```text
L1T1-Game-Project/
├── raylib/
│   └── include/              # raylib header files used by the project
├── the_ultimate_golf/
│   ├── assets/                # Images, textures, fonts and audio assets
│   └── the_ultimate_golf.c    # Complete game source code
├── .gitignore
└── README.md
```

## Requirements

- A C compiler supporting C11 or later
- raylib **6.0** (the project uses the raylib 6.0 API)
- A working OpenGL-capable desktop environment
- All assets included in `the_ultimate_golf/assets/`

The repository contains the raylib header files used by the project, but does **not** include compiled raylib libraries or executable files. A raylib installation is therefore required on the machine used to build the game.

## Dependencies / Libraries

### raylib

The game is built using raylib for:

- Window creation and rendering
- Mouse and keyboard input
- Textures and images
- Audio and sound effects
- Fonts
- 2D drawing and animation
- Vector and collision-related utilities

The repository includes the required raylib header files under `raylib/include/`, while the raylib library itself must be installed separately.

## Building and Running

### macOS

The simplest setup is to install raylib using Homebrew.

```bash
brew install raylib pkg-config
```

Then build the game:

```bash
cd the_ultimate_golf
clang -std=c11 the_ultimate_golf.c -I../raylib/include $(pkg-config --cflags --libs raylib) -lm -o the_ultimate_golf
```

Run it from the same directory:

```bash
./the_ultimate_golf
```

**Important:** Run the executable from inside the `the_ultimate_golf` directory. The game loads its assets using paths such as `assets/...`, so changing the working directory can cause assets to fail to load.

### Linux

Install a C compiler, pkg-config, and raylib using your distribution's package manager or by building raylib 6.0 from source.

For a system installation that provides `pkg-config`, build from the game directory with:

```bash
cd the_ultimate_golf
gcc -std=c11 the_ultimate_golf.c -I../raylib/include $(pkg-config --cflags --libs raylib) -lm -o the_ultimate_golf
```

Then run:

```bash
./the_ultimate_golf
```

If your distribution provides an older raylib release, use raylib 6.0 to match the bundled headers and the version used during development.

### Windows

Install raylib 6.0 together with a compatible GCC/MinGW toolchain. Make sure the raylib include and library directories are available to the compiler.

From the `the_ultimate_golf` directory, compile `the_ultimate_golf.c` while linking against raylib and the required system libraries. The exact linker flags depend on the raylib/MinGW distribution being used.

After compilation, run the generated executable **from the `the_ultimate_golf` directory** so that the relative asset paths resolve correctly.

## Controls

- **Left Mouse Button:** Click and drag from the golf ball to aim and control shot power; release to hit the ball.
- **Mouse:** Used for menus, level selection, aiming, and other interactive controls.
- **Escape:** Exit the current screen/game or return where supported.

## Gameplay

The objective is to get the golf ball into the hole using as few strokes as possible.

The levels contain different environments and obstacles that affect how the ball moves. The game includes collision handling, moving obstacles, hazards, visual effects, animations, and audio feedback.

## Assets

All assets required by the game are included in the repository under:

```text
the_ultimate_golf/assets/
```

This includes the game's image/texture assets, character or decorative images, themed level assets, fonts, and sound effects/music. No additional project asset download is required.

## Special Configuration / Setup Notes

1. **Working directory matters.** The executable should be launched from `the_ultimate_golf/` because asset paths are relative to that directory.
2. **raylib must be installed separately.** Compiled raylib libraries are intentionally not included in the submission.
3. **Use raylib 6.0.** The project includes raylib 6.0 headers, so using the matching raylib library avoids API/version mismatches.
4. **Do not commit generated binaries.** Executables and compiled object files are excluded through `.gitignore`.
5. The game requires a normal desktop graphics environment because raylib creates a graphical window and uses GPU/OpenGL functionality.

## Submission Contents

This repository is prepared for submission according to the project requirements:

- Complete C source code
- All required game assets
- raylib header files used by the source
- This detailed README with build, dependency, and setup instructions
- No executable or compiled object files

## Author

BUET CSE — CSE 102 Game Project
