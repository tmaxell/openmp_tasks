#include <iostream>
#include <fstream>
#include <omp.h>
#include <cmath>

double f(double x) {
    return std::sin(x); 
}

int main() {
    // Границы интегрирования
    double a = 0.0;
    double b = M_PI;

    // Количество разбиений
    int N = 1000000;
    double h = (b - a) / N;
    double integral_parallel = 0.0;
    double integral_sequential = 0.0;

    std::ofstream logFile("integral_log.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    double start_time_sequential = omp_get_wtime();
    for (int i = 0; i < N; ++i) {
        double x_i = a + i * h; 
        integral_sequential += f(x_i);
    }
    integral_sequential *= h;
    double end_time_sequential = omp_get_wtime();
    double execution_time_sequential = end_time_sequential - start_time_sequential;

    double start_time_parallel = omp_get_wtime();
#pragma omp parallel for reduction(+:integral_parallel)
    for (int i = 0; i < N; ++i) {
        double x_i = a + i * h; 
        integral_parallel += f(x_i);    
    }
    integral_parallel *= h;
    double end_time_parallel = omp_get_wtime();
    double execution_time_parallel = end_time_parallel - start_time_parallel;

    std::cout << "Sequential Integral value: " << integral_sequential << "\n";
    std::cout << "Parallel Integral value: " << integral_parallel << "\n";
    std::cout << "Sequential Execution time: " << execution_time_sequential << " seconds\n";
    std::cout << "Parallel Execution time: " << execution_time_parallel << " seconds\n";

    // Вычисление ускорения
    double speedup = execution_time_sequential / execution_time_parallel;
    std::cout << "Speedup (sequential / parallel): " << speedup << "\n";

    logFile << "Integration bounds: [" << a << ", " << b << "]\n";
    logFile << "Number of subdivisions (N): " << N << "\n";
    logFile << "Sequential Integral value: " << integral_sequential << "\n";
    logFile << "Parallel Integral value: " << integral_parallel << "\n";
    logFile << "Sequential Execution time: " << execution_time_sequential << " seconds\n";
    logFile << "Parallel Execution time: " << execution_time_parallel << " seconds\n";
    logFile << "Speedup (sequential / parallel): " << speedup << "\n";
    logFile << "------------------------------------------\n";

    logFile.close();
    return 0;
}
