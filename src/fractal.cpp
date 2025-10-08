#include "fractal.h"

typedef std::complex<double> Complex;
using namespace std;

void fractal_cpp(
    ispc::Point* grid, 
    int screen_height, 
    int screen_width,     
    double x_pos, 
    double y_pos, 
    int n, 
    int max_iter, 
    double tol, 
    double zoom
  ){

  double plane_width = screen_width / zoom;
  double plane_height = screen_height / zoom; 
  
  Complex cf = Complex(1,0);
  Complex cfprime = Complex(n,0);
  
  for (int y = 0; y < screen_height; y++) {
    for (int x = 0; x < screen_width; x++) {
      
      int depth = 0;
      int nearest_root = 0;

      double real = ((double)x / screen_width - 0.5) * plane_width + x_pos;
      double imag = ((double)y / screen_height - 0.5) * plane_height + y_pos;
      Complex z(real, imag);
      for (; depth < max_iter; depth++) {

        Complex zpow = pow(z, n-1);
        Complex f = z * zpow - cf;
        Complex fprime = cfprime * zpow;

        Complex dz =  f / fprime;

        if (abs(dz) < tol) {
          break;
        }
        z -= dz;
      }
      nearest_root = (int)((arg(z) + M_PI) / (2*M_PI/n))  % n; 
      grid[y * screen_width + x] = {depth, nearest_root};
    }
  }
}