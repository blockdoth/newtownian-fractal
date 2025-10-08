#include "raylib.h"
#include <complex>
#include <vector>
#include <cstdlib>

#include <chrono>


struct Point {
    int depth;
    int nearest_point;
};

const int screenWidth = 1000;
const int screenHeight = 1000;

typedef std::complex<float> Complex;

std::vector<Point> random_grid(){
  std::vector<Point> grid(screenWidth * screenHeight);
  for (int y = 0; y < screenHeight; y++) {
    for (int x = 0; x < screenWidth; x++) {
        grid[y * screenWidth + x].nearest_point = rand() % 3;
    }
  }
  return grid;
}



std::vector<Point> fractal_grid(float start_x, float start_y, int n, int max_iter, float tol, float zoom){
  std::vector<Point> grid(screenWidth * screenHeight);
  

  float plane_width = screenWidth / zoom;
  float plane_height = screenHeight / zoom; 
  Complex cf = Complex(1,0);
  Complex cfprime = Complex(n,0);
  
  for (int y = 0; y < screenHeight; y++) {
    for (int x = 0; x < screenWidth; x++) {
      
      int depth = 0;
      int nearest_root = 0;

      float real = (x / (float)screenWidth) * plane_width + start_x;
      float imag = (y / (float)screenHeight) * plane_height + start_y;
      Complex z(real, imag);
      for (; depth < max_iter; depth++) {

        Complex zpow = std::pow(z, n-1);
        Complex f = z * zpow - cf;
        Complex fprime = cfprime * zpow;

        Complex dz =  f / fprime;

        if (abs(dz) < tol) {
          break;
        }
        z -= dz;
      }
      nearest_root = (int)((std::arg(z) + M_PI) / (2*M_PI/n))  % n; 
      grid[y * screenWidth + x] = {depth, nearest_root};
    }
  }
  return grid;
}

int main() {
    int n = 5;
    int max_iter = 50;
    float tolerance = 1e-3;

    std::vector<Color> pixels(screenWidth * screenHeight);

    InitWindow(screenWidth, screenHeight, "Newton Fractal");
    SetTargetFPS(60);

    Texture2D texture = LoadTextureFromImage({
      pixels.data(),
      screenWidth,
      screenHeight,
      1,
      PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    });
    
    float start_x = -500.0f;
    float start_y = -500.0f;
    float zoom = 1.0f;

    float k = 20.0f; 
    float min_brightness = 0.2f;
    float log_base = logf(1.0f + k);


    bool moved = true;
    std::vector<Point> grid;
    const Color roots[] = { RED, GREEN, BLUE, YELLOW, ORANGE };

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_A))  { 
          start_x -= 100.0f; 
          moved = true; 
        }
        if (IsKeyPressed(KEY_D)) { 
          start_x += 100.0f; 
          moved = true; 
        }
        if (IsKeyPressed(KEY_W))    { 
          start_y -= 100.0f; 
          moved = true; 
        }
        if (IsKeyPressed(KEY_S))  { 
          start_y += 100.0f; 
          moved = true; 
        }

        if (IsKeyPressed(KEY_Q))  { 
          zoom *= 1.3f; 
          moved = true; 
        }        
        if (IsKeyPressed(KEY_E))  { 
          zoom /= 1.3f; 
          moved = true; 
        }       

        if (moved) {
            auto compute_before = std::chrono::steady_clock::now();
            grid = fractal_grid(start_x, start_y, n, max_iter, tolerance,zoom);
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_before);
            printf("Frame recomputed at (%.2f, %.2f) %.2fx in %.3f s\n", start_x, start_y,zoom, duration.count());
          
            moved = false;
        }    

        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                int idx = y * screenWidth + x;

                Color color = roots[grid[idx].nearest_point % n];

                float normalized = static_cast<float>(grid[idx].depth) / max_iter;                
                float brightness = logf(1.0f + k * normalized) / log_base;
                
                float brightness_adjusted = min_brightness + (1.0f - brightness) * brightness;

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
    }

    UnloadTexture(texture);
    CloseWindow();
}
