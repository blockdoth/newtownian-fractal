#include "raylib.h"
#include <complex>
#include <functional>
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



std::vector<Point> fractal_grid(float start_x, float start_y, int n, int max_iter, float tol, std::function<Complex(Complex)> f, std::function<Complex(Complex)> fprime){
  std::vector<Point> grid(screenWidth * screenHeight);
  
  for (int y = 0; y < screenHeight; y++) {
    for (int x = 0; x < screenWidth; x++) {
      
      int depth = 0;
      int nearest_root = 0;
      Complex z = Complex(x + start_x, y + start_y);
      for (; depth < max_iter; depth++) {

        Complex dz = f(z) / fprime(z);

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
    float tolerance = 1e-2;

    std::vector<Color> pixels(screenWidth * screenHeight);
    
    auto f      = [n](Complex z) { return std::pow(z, n) - Complex(1,0); };
    auto fprime = [n](Complex z) { return Complex(n  , 0) * std::pow(z, n - 1); };
    
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

    bool moved = true;
    std::vector<Point> grid;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_LEFT))  { 
          start_x -= 100.0f; 
          moved = true; 
        }
        if (IsKeyPressed(KEY_RIGHT)) { 
          start_x += 100.0f; 
          moved = true; 
        }
        if (IsKeyPressed(KEY_UP))    { 
          start_y -= 100.0f; 
          moved = true; 
        }
        if (IsKeyPressed(KEY_DOWN))  { 
          start_y += 100.0f; 
          moved = true; 
        }

        if (moved) {
            auto before = std::chrono::steady_clock::now();
            grid = fractal_grid(start_x, start_y, n, max_iter, tolerance, f, fprime);
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - before);
            printf("Frame recomputed at (%.2f, %.2f) in %.3f s\n", start_x, start_y, duration.count());
          
            moved = false;
        }    

        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                int idx = y * screenWidth + x;
                Color color;
                switch (grid[idx].nearest_point) {
                    case 0: color = RED; break;
                    case 1: color = GREEN; break;
                    case 2: color = BLUE; break;
                    case 3: color = YELLOW; break;
                    case 4: color = ORANGE; break;
                }

                float k = 20.0f; 
                float min_brightness = 0.2f;

                float normalized = static_cast<float>(grid[idx].depth) / max_iter;                
                float brightness = logf(1.0f + k * normalized) / logf(1.0f + k);
                
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
