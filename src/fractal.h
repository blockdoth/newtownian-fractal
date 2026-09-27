#include <complex>
#include <raylib.h>
#include "fractal_ispc.h"

typedef std::complex<double> Complex;

void fractal_cpp(
  ispc::Point* grid, 
  int screen_height, 
  int screen_width,     
  double x_pos, 
  double y_pos, 
  int n, 
  int max_iter, 
  double tol, 
  double zoom,
  float mouse_x,
  float mouse_y
);