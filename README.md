# The Ultimate Golf

A 2D mini-golf game made in C with raylib for the BUET CSE 102 game project.

## Requirements

- C compiler
- raylib 6.0
- All files inside `the_ultimate_golf/assets/`

## Build & Run

Install raylib 6.0, then from the project root:

```bash
cd the_ultimate_golf
gcc -std=c11 the_ultimate_golf.c -I../raylib/include $(pkg-config --cflags --libs raylib) -lm -o the_ultimate_golf
./the_ultimate_golf
```

Run the executable from `the_ultimate_golf/` so the asset paths work correctly.

## Controls

- **Left mouse:** Aim and shoot
- **Mouse:** Menus and level selection
- **Escape:** Exit / go back

## Project Structure

```text
raylib/include/          raylib headers
the_ultimate_golf/
├── the_ultimate_golf.c  source code
└── assets/              game assets
```

## Credits

- **2505093 — Abdullah Al Nafi**
- **2505114 — Syed Abdul Fahim**

BUET CSE — CSE 102 Game Project
