#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>
#include <fstream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));
    int n;
    std::cout << "Enter the size of the vector: ";
    std::cin >> n;

    // Диапазон случайных чисел
    int min_range, max_range;
    std::cout << "Enter the minimum range value: ";
    std::cin >> min_range;
    std::cout << "Enter the maximum range value: ";
    std::cin >> max_range;

    // Проверка корректности диапазона
    if (min_range > max_range) {
        std::cerr << "Error: Minimum range value cannot be greater than maximum range value.\n";
        return 1;
    }

    // Генерация случайного вектора
    std::vector<int> vec(n);
    for (int i = 0; i < n; ++i) {
        vec[i] = min_range + std::rand() % (max_range - min_range + 1);
    }

    double start_time, end_time;

    std::ofstream log_file("performance_log.txt", std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Error: Unable to open log file.\n";
        return 1;
    }

    // Подход с использованием редукции
    int min_val = std::numeric_limits<int>::max();
    int max_val = std::numeric_limits<int>::min();

    start_time = omp_get_wtime();
#pragma omp parallel for reduction(min:min_val) reduction(max:max_val) schedule(static)
    for (int i = 0; i < n; ++i) {
        if (vec[i] < min_val) min_val = vec[i];
        if (vec[i] > max_val) max_val = vec[i];
    }
    end_time = omp_get_wtime();
    double time_with_reduction = end_time - start_time;

    std::cout << "Using reduction:\n";
    std::cout << "Minimum value: " << min_val << "\n";
    std::cout << "Maximum value: " << max_val << "\n";
    std::cout << "Execution time with reduction: " << time_with_reduction << " seconds\n";

    // Подход без использования редукции
    min_val = std::numeric_limits<int>::max();
    max_val = std::numeric_limits<int>::min();

    start_time = omp_get_wtime();
#pragma omp parallel
    {
        // Общие переменные обновляются в критических секциях
#pragma omp for
        for (int i = 0; i < n; ++i) {
#pragma omp critical
            {
                if (vec[i] < min_val) min_val = vec[i];
                if (vec[i] > max_val) max_val = vec[i];
            }
        }
    }
    end_time = omp_get_wtime();
    double time_without_reduction = end_time - start_time;

    std::cout << "Without reduction:\n";
    std::cout << "Minimum value: " << min_val << "\n";
    std::cout << "Maximum value: " << max_val << "\n";
    std::cout << "Execution time without reduction: " << time_without_reduction << " seconds\n";

    log_file << "Vector size: " << n << "\n";
    log_file << "Range: [" << min_range << ", " << max_range << "]\n";
    log_file << "Time with reduction: " << time_with_reduction << " seconds\n";
    log_file << "Time without reduction: " << time_without_reduction << " seconds\n";
    log_file << "---------------------------------------\n";

    log_file.close();
    return 0;
}
