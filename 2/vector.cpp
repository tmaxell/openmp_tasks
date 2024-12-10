#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <omp.h>

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));

    int n;
    std::cout << "Enter the size of the vectors: ";
    std::cin >> n;

    std::vector<int> A(n);
    std::vector<int> B(n);

    for (int i = 0; i < n; ++i) {
        A[i] = std::rand() % 100;
        B[i] = std::rand() % 100;
    }

    // Лог-файл
    std::ofstream logFile("results.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    // Скалярное произведение с использованием редукции
    int dot_product_reduction = 0;
    double start_time_reduction = omp_get_wtime();

    #pragma omp parallel for reduction(+:dot_product_reduction) schedule(static)
    for (int i = 0; i < n; ++i) {
        dot_product_reduction += A[i] * B[i];
    }

    double end_time_reduction = omp_get_wtime();
    double time_reduction = end_time_reduction - start_time_reduction;

    std::cout << "Dot product using reduction: " << dot_product_reduction << "\n";
    logFile << "Using reduction: Dot product = " << dot_product_reduction
            << ", Time = " << time_reduction << " seconds\n";

    // Скалярное произведение без использования редукции
    int dot_product_no_reduction = 0;
    double start_time_no_reduction = omp_get_wtime();

    #pragma omp parallel
    {
        int local_sum = 0;

        #pragma omp for schedule(static)
        for (int i = 0; i < n; ++i) {
            local_sum += A[i] * B[i];
        }

        #pragma omp critical
        {
            dot_product_no_reduction += local_sum;
        }
    }

    double end_time_no_reduction = omp_get_wtime();
    double time_no_reduction = end_time_no_reduction - start_time_no_reduction;

    std::cout << "Dot product without reduction: " << dot_product_no_reduction << "\n";
    logFile << "Without reduction: Dot product = " << dot_product_no_reduction
            << ", Time = " << time_no_reduction << " seconds\n";

    logFile.close();

    std::cout << "\nPerformance Comparison:\n";
    std::cout << "Reduction time: " << time_reduction << " seconds\n";
    std::cout << "No reduction time: " << time_no_reduction << " seconds\n";

    return 0;
}
