![Logo](images/logo.png)
# RayCasting Engine
---

## Introduction
RayCasting is a technique in computer graphics that allows you to create 3D projections based on a 2D plane. This technique was particularly valued in an era when the processing power of computers was limited. It allowed games, the first of which was Wolfenstein 3D, to achieve an impressive illusion of three-dimensionality.

![Showcase](images/game.gif)

## Programming language
The C programming language, known for its high performance and ability for low-level resource management, was chosen to realize the project. Graphics are displayed using the ncurses library, which allows efficient drawing in the console, creating complex text interfaces and visualizations. The Windows API is used to read keystrokes, providing reliable and responsive user interaction with the program. These technologies provide a strong set of tools for creating raycasting-based games. The project is compatible with Windows OC only.

## How to run the project
1. Unpack and install the project
2. Open it using the console line
3. To compile:
   > gcc -o main main.c –lpdcurses
4. To run:
   > main

## Control
* W (up) -	walk forwards
* S (down) - walk backward
* A (left)	- turn left
* D (right) - turn right
* Esc - Exit
