struct Point {
  int depth;
  int nearest_root;
};

typedef struct Complex {
  double real;
  double imag;
} Complex;

inline Complex add(Complex a, Complex b) {
  Complex r;
  r.real = a.real + b.real;
  r.imag = a.imag + b.imag;
  return r;
}

inline Complex subtract(Complex a, Complex b) {
  Complex r;
  r.real = a.real - b.real;
  r.imag = a.imag - b.imag;
  return r;
}

inline Complex multiply(Complex a, Complex b) {
  Complex r;
  r.real = a.real * b.real - a.imag * b.imag;
  r.imag = a.real * b.imag + a.imag * b.real ;
  return r;
}

inline Complex divide(Complex a, Complex b) {
  double denom = b.real * b.real + b.imag * b.imag;
  Complex r;
  r.real = (a.real * b.real + a.imag * b.imag) / denom;
  r.imag = (a.imag * b.real - a.real * b.imag) / denom;
  return r;
}

inline double arg(Complex z) {
  return atan2(z.imag, z.real);
}

inline double mag(Complex z) {
  return sqrt(z.real*z.real + z.imag*z.imag);
}

inline Complex pow(Complex z, int n) {
  Complex r;
  r.real = 1.0;
  r.imag = 0.0;

  for (int i = 0; i < n; i++) {
      r = multiply(r, z);
  }
  return r;
}
 


task void fractal_ispc_task(
    uniform Point grid[], 
    uniform int screen_height, 
    uniform int screen_width,     
    uniform double x_pos, 
    uniform double y_pos, 
    uniform int n, 
    uniform int max_iter, 
    uniform double tol, 
    uniform double zoom,
    uniform int core_count
  ){

  double plane_width = screen_width / zoom;
  double plane_height = screen_height / zoom; 

  uniform double inv_width = 1.0 / screen_width;
  uniform double inv_height = 1.0 / screen_height;
  uniform double region_divider = 2 * PI / n;
  Complex cf;
  cf.real = 1.0;
  cf.imag = 0.0;

  Complex cfprime;
  cfprime.real = (double) n;
  cfprime.imag = 0.0;  
  
  
  uniform int stroke_height = (screen_height + core_count - 1) / core_count;

  int y_start = taskIndex * stroke_height;
  int y_end = (taskIndex + 1) * stroke_height;

  int depth = 0;
  int nearest_root = 0;
  for (int y = y_start; y < y_end; y++) {
    foreach (x = 0 ... screen_width) { 
      depth = 0;
      nearest_root = 0;
      
      Complex z;
      z.real = ((double)x * inv_width - 0.5) * plane_width + x_pos;
      z.imag = ((double)y * inv_height - 0.5) * plane_height + y_pos;

      for (; depth < max_iter; depth++) {
  
        Complex zpow = pow(z, n-1);
        Complex f = subtract(multiply(z, zpow), cf);
        Complex fprime = multiply(cfprime, zpow);
  
        Complex dz = divide(f, fprime);
  
        if (mag(dz) < tol) {
          break;
        }
        z = subtract(z, dz);
      }
      nearest_root = (int)((arg(z) + PI) / region_divider)  % n; 
      
      grid[y * screen_width + x].depth = depth;
      grid[y * screen_width + x].nearest_root = nearest_root;
      
    }    
  }
}

export void fractal_ispc(
  uniform Point grid[], 
  uniform int screen_height, 
  uniform int screen_width,     
  uniform double x_pos, 
  uniform double y_pos, 
  uniform int n, 
  uniform int max_iter, 
  uniform double tol, 
  uniform double zoom,
  uniform core_count
){
  launch [core_count] fractal_ispc_task(
    grid, 
    screen_height, 
    screen_width, 
    x_pos, 
    y_pos, 
    n, 
    max_iter, 
    tol, 
    zoom,
    core_count
  );
}

