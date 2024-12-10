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

    std::vector<int> vec(n);
    for (int i = 0; i < n; ++i) {
        vec[i] = std::rand() % 1000; // Случайные числа от 0 до 999
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

    // Подход без использования редукции (с минимизацией синхронизации)
    min_val = std::numeric_limits<int>::max();
    max_val = std::numeric_limits<int>::min();

    start_time = omp_get_wtime();
    int num_threads;
    std::vector<int> local_min, local_max;

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int local_min_val = std::numeric_limits<int>::max();
        int local_max_val = std::numeric_limits<int>::min();

#pragma omp for nowait
        for (int i = 0; i < n; ++i) {
            if (vec[i] < local_min_val) local_min_val = vec[i];
            if (vec[i] > local_max_val) local_max_val = vec[i];
        }

#pragma omp critical
        {
            if (local_min_val < min_val) min_val = local_min_val;
            if (local_max_val > max_val) max_val = local_max_val;
        }

#pragma omp single
        num_threads = omp_get_num_threads();
    }
    end_time = omp_get_wtime();
    double time_without_reduction = end_time - start_time;

    std::cout << "Without reduction (optimized):\n";
    std::cout << "Minimum value: " << min_val << "\n";
    std::cout << "Maximum value: " << max_val << "\n";
    std::cout << "Execution time without reduction: " << time_without_reduction << " seconds\n";

    log_file << "Vector size: " << n << "\n";
    log_file << "Threads used: " << num_threads << "\n";
    log_file << "Time with reduction: " << time_with_reduction << " seconds\n";
    log_file << "Time without reduction: " << time_without_reduction << " seconds\n";
    log_file << "---------------------------------------\n";

    log_file.close();
    return 0;
}
