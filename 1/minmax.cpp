#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>

int main() {
    // Инициализация вектора с произвольными значениями
    std::vector<int> vec = {45, 22, 78, 11, 90, 3, 67, 50, 19};
    int n = vec.size();

    // Подход с использованием редукции
    int min_val = std::numeric_limits<int>::max();
    int max_val = std::numeric_limits<int>::min();

#pragma omp parallel for reduction(min:min_val) reduction(max:max_val)
    for (int i = 0; i < n; ++i) {
        if (vec[i] < min_val) min_val = vec[i];
        if (vec[i] > max_val) max_val = vec[i];
    }

    std::cout << "Using reduction:\n";
    std::cout << "Minimum value: " << min_val << "\n";
    std::cout << "Maximum value: " << max_val << "\n";

    // Подход без использования редукции
    min_val = std::numeric_limits<int>::max();
    max_val = std::numeric_limits<int>::min();

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

    std::cout << "Without reduction:\n";
    std::cout << "Minimum value: " << min_val << "\n";
    std::cout << "Maximum value: " << max_val << "\n";

    return 0;
}
