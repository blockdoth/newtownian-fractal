#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <chrono>
#include <iterator>
#include "fractal.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;
const Color roots[] = { RED, GREEN, BLUE, YELLOW, ORANGE, PURPLE, GRAY, PINK, DARKGREEN, DARKBLUE};

using namespace std;
using namespace chrono;
using namespace ispc;

int main() {
    // Fractal computation
    int n = 3;
    int max_n = sizeof(roots) / sizeof(roots[0]);
    int max_iter_step = 20;
    int max_iter = n * max_iter_step;
    double tolerance = 1e-7;
    
    // Positioning
    double x_pos = 0.0f;
    double y_pos = 0.0f;
    double zoom = 1.0f;
    
    // Color banding
    double k = 20.0f; 
    double min_brightness = 0.2f;
    double log_base = logf(1.0f + k);

    bool changed = true;
    bool use_ispc = false;
     
    vector<Color> pixels(SCREEN_WIDTH * SCREEN_HEIGHT);

    int mem_size = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Point);
    Point* grid = (Point*) std::aligned_alloc(32, mem_size);
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Newton Fractal");
    SetTargetFPS(60);

    Texture2D texture = LoadTextureFromImage({
      pixels.data(),
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      1,
      PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    });    


    while (!WindowShouldClose()) {
        double step = 100.0f / zoom;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  { 
          x_pos -= step; 
          changed = true; 
        }

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { 
          x_pos += step; 
          changed = true; 
        }

        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    { 
          y_pos -= step; 
          changed = true; 
        }

        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  { 
          y_pos += step; 
          changed = true; 
        }

        if (IsKeyDown(KEY_Q) && zoom > 1.0f)  { 
          zoom /= 1.5f; 
          changed = true; 
        }        

        if (IsKeyDown(KEY_E))  { 
          zoom *= 1.5f; 
          changed = true; 
        } 

        if (IsKeyDown(KEY_EQUAL) && n < max_n)  { 
          n += 1; 
          max_iter = n * max_iter_step;
          changed = true; 
        }   

        if (IsKeyDown(KEY_MINUS) && n > 0)  { 
          n -= 1; 
          max_iter = n * max_iter_step;
          changed = true; 
        } 

        if (IsKeyDown(KEY_C))  { 
          max_iter += 10;
          changed = true; 
        }   

        if (IsKeyDown(KEY_Z) && max_iter > 0)  { 
          max_iter -= 10;
          changed = true; 
        }         

        if (IsKeyDown(KEY_SPACE) )  { 
          use_ispc = !use_ispc;
          changed = true; 
        }                                      

        if (changed) {
            auto compute_before = steady_clock::now();
            
            if (use_ispc){
              fractal_ispc(grid, SCREEN_WIDTH, SCREEN_HEIGHT, x_pos, y_pos, n, max_iter, tolerance,zoom);
            }else {
              fractal_cpp(grid, SCREEN_WIDTH, SCREEN_HEIGHT, x_pos, y_pos, n, max_iter, tolerance,zoom);
            }
            
            auto duration = duration_cast<chrono::duration<double>>(steady_clock::now() - compute_before);
            
            printf("Frame (%dx%d) recomputed in %f secs at (%.2f, %.2f) using ispc = %d at %.2fx zoom with n=%d and max_iter=%d\n", SCREEN_WIDTH, SCREEN_HEIGHT,  duration.count(), x_pos, y_pos, use_ispc, zoom,n, max_iter);
        }    
        
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                int idx = y * SCREEN_WIDTH + x;
            
                Color color = roots[grid[idx].nearest_root % n];
            
                double normalized = static_cast<double>(grid[idx].depth) / max_iter;                
                double brightness = logf(1.0f + k * normalized) / log_base;

                double brightness_adjusted = min_brightness + (1.0f - brightness) * brightness;
                // printf("%d %d\n", grid[idx].depth, grid[idx].nearest_root);
                pixels[idx] = {
                    static_cast<unsigned char>(color.r * brightness_adjusted),
                    static_cast<unsigned char>(color.g * brightness_adjusted),
                    static_cast<unsigned char>(color.b * brightness_adjusted),
                    255
                };
            }
        }
      

        UpdateTexture(texture, pixels.data());
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        EndDrawing();
        changed = false;
    }
    std::free(grid);
    UnloadTexture(texture);
    CloseWindow();
}
