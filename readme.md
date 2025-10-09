
# Newton's fractal

An implementation of Newtowns fractal making use of the [ispc](https://github.com/ispc/ispc) compiler to compile a C dialect optimized for SIMD code generation. Using raylib for visualization.

![](assets/reversed.gif "")

## Building
I used nix to manage dependencies, but on any other system make sure the following packages are installed to replicate my environment

```bash
# Required
gcc
cmake
ispc
raylib

# Utils
gdb
clang-tools
perf 
flamegraph

# Recording
gifsicle
ffmpeg
```
If you have nix installed you can make use of my flake and enter a shell with all dependencies installed using  `nix develop`


In both cases proceed to build the program like this

```bash
mkdir build
cd build
cmake ..
make && /
```

## Visualization
I used [raylib](https://github.com/raysan5/raylib) for managing window lifetime and drawing, this amazing C library abstracts over a lot of the verbose low level rendering api normally required, yet allows still allows for a lot of fine control. 

Raylib is compiled with the SDL backend because of issues with hyprland (my window manager) using the GLFW backed, but both backends should work fine on pretty much any other configuration. 


## Threading runtime
The ISPC threading runtime was left as an interface for the user by default. Based on the documentation I included [a default implementation](https://github.com/ispc/ispc/blob/main/examples/common/tasksys.cpp) found in the examples folder of the github repo in my project. An alternative linking based approach is also possible given that the `ispc` package comes both with a shared and static library available.

## Recording
You can toggle recording by pressing `r` this saves each frame rendered to an (hardcoded) output folder . You can then use the `make_gif.sh` script to turn the individual frames into a nice gif. 

## Controls
There are quite a few controls, here a quick overview

```
=== Navigation ===
W / ↑ - Move up
S / ↓ - Move down
A / ← - Move left
D / → - Move right

SPACE      - Zoom in
Left Shift - Zoom out

=== Fractal Parameters ===

= (Equal key) - Increase polynomial degree n
- (Minus key) - Decrease polynomial degree n

Q - Increase max iteration depth
E - Decrease max iteration depth

=== Computation Mode ===

1 - Serial 
2 - SIMD 
3 - SIMD Threaded 

=== Recording ===

R - Toggle frame recording on/off

```


