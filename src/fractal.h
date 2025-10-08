#include <complex>
#include "fractal_ispc.h"

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
);