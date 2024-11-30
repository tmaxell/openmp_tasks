#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>

void fillArray(std::vector<int>& array, int minValue, int maxValue) {
    for (size_t i = 0; i < array.size(); ++i) {
        array[i] = minValue + std::rand() % (maxValue - minValue + 1);
    }
}

double reductionAtomic(const std::vector<int>& array) {
    int sum = 0;
    double startTime = omp_get_wtime();
#pragma omp parallel for
    for (size_t i = 0; i < array.size(); ++i) {
#pragma omp atomic
        sum += array[i];
    }
    double endTime = omp_get_wtime();
    std::cout << "Sum (atomic): " << sum << "\n";
    return endTime - startTime;
}

double reductionCritical(const std::vector<int>& array) {
    int sum = 0;
    double startTime = omp_get_wtime();
#pragma omp parallel for
    for (size_t i = 0; i < array.size(); ++i) {
#pragma omp critical
        sum += array[i];
    }
    double endTime = omp_get_wtime();
    std::cout << "Sum (critical): " << sum << "\n";
    return endTime - startTime;
}

double reductionLock(const std::vector<int>& array) {
    int sum = 0;
    omp_lock_t lock;
    omp_init_lock(&lock);

    double startTime = omp_get_wtime();
#pragma omp parallel for
    for (size_t i = 0; i < array.size(); ++i) {
        omp_set_lock(&lock);
        sum += array[i];
        omp_unset_lock(&lock);
    }
    double endTime = omp_get_wtime();
    omp_destroy_lock(&lock);
    std::cout << "Sum (lock): " << sum << "\n";
    return endTime - startTime;
}

double reductionOpenMP(const std::vector<int>& array) {
    int sum = 0;
    double startTime = omp_get_wtime();
#pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < array.size(); ++i) {
        sum += array[i];
    }
    double endTime = omp_get_wtime();
    std::cout << "Sum (OpenMP reduction): " << sum << "\n";
    return endTime - startTime;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));
    const size_t arraySize = 1'000'000;
    std::vector<int> array(arraySize);

    fillArray(array, 1, 100);

    double timeAtomic = reductionAtomic(array);
    double timeCritical = reductionCritical(array);
    double timeLock = reductionLock(array);
    double timeOpenMP = reductionOpenMP(array);

    std::cout << "\nExecution times:\n";
    std::cout << "Atomic: " << timeAtomic << " seconds\n";
    std::cout << "Critical: " << timeCritical << " seconds\n";
    std::cout << "Lock: " << timeLock << " seconds\n";
    std::cout << "OpenMP reduction: " << timeOpenMP << " seconds\n";

    return 0;
}

