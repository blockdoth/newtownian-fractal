
# Newton's fractal using ISPC

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


In both cases once you have a shell containing all required dependencies proceed to build and run the program like this

```bash
mkdir build
cd build
cmake ..
make && /newton-fractal
```
All configuration is done using keybinds while the program is running.

## Visualization
I used [raylib](https://github.com/raysan5/raylib) for managing window lifetime and drawing, this amazing C library abstracts over a lot of the verbose low level rendering api normally required, yet allows still allows for a lot of fine control. 

I compiled Raylib with the SDL backend because of minor graphical issues with the GLFW backend and hyprland (my window manager), but both backends should work fine on pretty much any other system. 


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


## Notes 
On my system the performance difference between Serial and SIMD was less than I expected, only about 1.5x, I put some time in improving this but concluded that with my current knowledge of SIMD best principles, this is it for now. 

I thought about moving from an array of structs to a struct of arrays for better vectorized memory access patterns, but that would require significant changes propagating to basically all code of the program.  

Another performance warning the ispc compiler kept giving me was related to the modulus operation I used to find the nearest root. I could have looked into using a more classical distance enumeration based calculation, however I found the modulus based trick really cool, so its staying in.

The cpu I tested this on has 8 cores / 16 threads, so at first I picked 16 as the task count for multithreaded usage. However even while not entirely sure how the provided runtime worked I figured that at least doubling that could improve performance somewhat, to make use of an idle time occurring from uneven iteration depth between tasks. Having more tasks should therefore provide better overall cpu utilization at the cost of some overhead. Picking the optimal value would require setting up a benchmark.