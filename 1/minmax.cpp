#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>
#include <fstream>

int main() {
    // Инициализация вектора с произвольными значениями
    std::vector<int> vec = {45, 22, 78, 11, 90, 3, 67, 50, 19};
    int n = vec.size();
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
#pragma omp parallel for reduction(min:min_val) reduction(max:max_val)
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
        int local_min = std::numeric_limits<int>::max();
        int local_max = std::numeric_limits<int>::min();

#pragma omp for
        for (int i = 0; i < n; ++i) {
            if (vec[i] < local_min) local_min = vec[i];
            if (vec[i] > local_max) local_max = vec[i];
        }

#pragma omp critical
        {
            if (local_min < min_val) min_val = local_min;
            if (local_max > max_val) max_val = local_max;
        }
    }
    end_time = omp_get_wtime();
    double time_without_reduction = end_time - start_time;

    std::cout << "Without reduction:\n";
    std::cout << "Minimum value: " << min_val << "\n";
    std::cout << "Maximum value: " << max_val << "\n";
    std::cout << "Execution time without reduction: " << time_without_reduction << " seconds\n";

    log_file << "Vector size: " << n << "\n";
    log_file << "Time with reduction: " << time_with_reduction << " seconds\n";
    log_file << "Time without reduction: " << time_without_reduction << " seconds\n";
    log_file << "---------------------------------------\n";

    log_file.close();
    return 0;
}
