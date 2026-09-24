<h1 align="center">Command Line RayCasting Visualization</h1>

Terminal-based [raycaster](https://en.wikipedia.org/wiki/Ray_casting) demo written in pure C. It renders a pseudo-3D first-person maze in the style of [Wolfenstein 3D](https://en.wikipedia.org/wiki/Wolfenstein_3D) directly in the console using `curses` library and ASCII characters.

<p align="center">
  <img src="images/game.gif" alt="Demo Gif" width="600">
</p>

# Controls

```
↑, w               Walk forward
↓, s               Walk backward
←, a               Turn left
→, d               Turn right
Esc                Quit
```

# Dependencies

- C compiler
- CMake
- `curses` library

# Build

```bash
mkdir build && cd build
cmake ..
make
./RayCasting
```
