#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <chrono>
#include "fractal.h"

using namespace std;
using namespace chrono;
using namespace ispc;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;
const Color roots[] = { RED, GREEN, BLUE, YELLOW, ORANGE, PURPLE, GRAY, PINK, DARKGREEN, DARKBLUE};

enum Mode {
  SERIAL,
  SIMD,
  SIMD_THREADED
};

const char* MODE_STRING[] = {
  "Serial",
  "SIMD",
  "SIMD Threaded"
};

const string asset_path = "../output/";

int main() {
    // Fractal computation
    int n = 3;
    int max_n = sizeof(roots) / sizeof(roots[0]);
    int max_iter_step = 25;
    int max_iter = n * max_iter_step;
    double iter_delta_factor = 0.4;
    
    double tolerance = 1e-7;
    
    // Positioning
    double x_pos = 0.0f;
    double y_pos = 0.0f;
    double zoom = 1.0f;
    double zoom_factor = 1.2;
    double base_step_size = 20.0f;
    
    // Color banding
    double k = 5.0f; 
    double min_brightness = 0.4f;
    double log_base = 1 / logf(1.0f + k);

    // UI 
    bool changed = true;
    Mode mode = SERIAL;
    bool save = false;
     
    // recording
    int frame_idx = 0;
    
    vector<Color> pixels(SCREEN_WIDTH * SCREEN_HEIGHT);
    Point* grid = (Point*) std::aligned_alloc(32,  SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Point));
    
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
        double step = base_step_size / zoom;
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

        if (IsKeyDown(KEY_LEFT_SHIFT) && zoom > 1.0f)  { 
          zoom /= zoom_factor; 
          int delta_max_iter = log(zoom) * iter_delta_factor;
          if (max_iter > delta_max_iter){
            max_iter -= delta_max_iter;
          }
          changed = true; 
        }        

        if (IsKeyDown(KEY_SPACE))  { 
          zoom *= zoom_factor; 
          max_iter += log(zoom) * iter_delta_factor;
          changed = true; 
        } 

        if (IsKeyPressed(KEY_EQUAL) && n < max_n)  { 
          n += 1; 
          max_iter = n * max_iter_step + log(zoom) * iter_delta_factor;
          changed = true; 
        }   

        if (IsKeyPressed(KEY_MINUS) && n > 0)  { 
          n -= 1; 
          max_iter = n * max_iter_step + log(zoom) * iter_delta_factor;
          changed = true; 
        } 

        if (IsKeyDown(KEY_Q))  { 
          max_iter += 10;
          changed = true; 
        }   

        if (IsKeyDown(KEY_E) && max_iter > 10)  { 
          max_iter -= 10;
          changed = true; 
        }         

        if (IsKeyPressed(KEY_ONE) )  { 
          mode = SERIAL;
          changed = true; 
        }    
        
        if (IsKeyPressed(KEY_TWO) )  { 
          mode = SIMD;
          changed = true; 
        }                                         
        
        if (IsKeyPressed(KEY_THREE) )  { 
          mode = SIMD_THREADED;
          changed = true; 
        }    
        if (IsKeyPressed(KEY_R) )  { 
          save = !save; 
          if (save){
            printf("Started recording frames\n");
          }else {
            printf("Stoped recording frames\n");
          }
        }    

        if (changed) {
            auto compute_before = steady_clock::now();
            
            switch (mode) {
              case SERIAL:
                fractal_cpp(grid, SCREEN_WIDTH, SCREEN_HEIGHT, x_pos, y_pos, n, max_iter, tolerance,zoom);
                break;
              case SIMD:
                fractal_ispc(grid, SCREEN_WIDTH, SCREEN_HEIGHT, x_pos, y_pos, n, max_iter, tolerance,zoom, 1);
                break;
              case SIMD_THREADED:
                fractal_ispc(grid, SCREEN_WIDTH, SCREEN_HEIGHT, x_pos, y_pos, n, max_iter, tolerance,zoom, 64);
                break;
            }

            
            auto duration = duration_cast<chrono::duration<double>>(steady_clock::now() - compute_before);
            
            printf("Frame (%dx%d) recomputed in %f secs at (%.2f, %.2f) mode %s at %fx zoom with n=%d and max_iter=%d\n", SCREEN_WIDTH, SCREEN_HEIGHT,  duration.count(), x_pos, y_pos, MODE_STRING[mode], zoom,n, max_iter);
            
            if (save){
              Image image = LoadImageFromTexture(texture); 
              std::string path = std::format("{}frame_{:03}.png", asset_path, frame_idx);
              printf("Exporting frame to %s\n", path.c_str());
              ExportImage(image, path.c_str());
              frame_idx++;
            }
        }    
        
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                int idx = y * SCREEN_WIDTH + x;
            
                Color color = roots[grid[idx].nearest_root % n];
          
                double normalized = static_cast<double>(grid[idx].depth) / max_iter;                
                double brightness = logf(1.0f + k * normalized) * log_base;

                double brightness_adjusted = min_brightness + (1.0f - brightness) * brightness;

                color.r *= brightness_adjusted; 
                color.g *= brightness_adjusted; 
                color.b *= brightness_adjusted; 
                pixels[idx] = color;
            }
        }
      
        UpdateTexture(texture, pixels.data());
        BeginDrawing();
        
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        DrawFPS(10,10);
        
        EndDrawing();
        changed = false;
    }
    std::free(grid);
    UnloadTexture(texture);
    CloseWindow();
}
