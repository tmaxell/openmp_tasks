#include <iostream>
#include <fstream>
#include <omp.h>
#include <cmath>

double f(double x) {
    return std::sin(x); 
}

int main() {
    // Границы интегрирования
    double a = 0.0; // Начало отрезка
    double b = M_PI; // Конец отрезка (например, интеграл sin(x) от 0 до π)

    // Количество разбиений
    int N = 1000000;
    double h = (b - a) / N;
    double integral = 0.0;

    std::ofstream logFile("integral_log.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    double start_time = omp_get_wtime();

#pragma omp parallel for reduction(+:integral)
    for (int i = 0; i < N; ++i) {
        double x_i = a + i * h; 
        integral += f(x_i);    
    }
    integral *= h;

    double end_time = omp_get_wtime();
    double execution_time = end_time - start_time;

    std::cout << "Integral value: " << integral << "\n";
    std::cout << "Execution time: " << execution_time << " seconds\n";

    logFile << "Integration bounds: [" << a << ", " << b << "]\n";
    logFile << "Number of subdivisions (N): " << N << "\n";
    logFile << "Integral value: " << integral << "\n";
    logFile << "Execution time: " << execution_time << " seconds\n";
    logFile << "------------------------------------------\n";

    logFile.close();
    return 0;
}
