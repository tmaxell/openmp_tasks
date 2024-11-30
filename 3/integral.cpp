#include <iostream>
#include <omp.h>
#include <cmath> 

double f(double x) {
    return std::sin(x); 
}

int main() {
    // Границы интегрирования
    double a = 0.0; // Начало отрезка
    double b = M_PI; // Конец отрезка (например, интеграл sin(x) от 0 до π)

    int N = 1000000;
    double h = (b - a) / N;
    double integral = 0.0;

#pragma omp parallel for reduction(+:integral)
    for (int i = 0; i < N; ++i) {
        double x_i = a + i * h; 
        integral += f(x_i);    
    }
    integral *= h;
    std::cout << "Integral value: " << integral << "\n";
    return 0;
}
