#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <fstream>

// Функция для заполнения массива случайными числами
void fillArray(std::vector<int>& array, int minValue, int maxValue) {
    for (size_t i = 0; i < array.size(); ++i) {
        array[i] = minValue + std::rand() % (maxValue - minValue + 1);
    }
}

// Сумма с использованием атомарных операций
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

// Сумма с использованием критической секции
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

// Сумма с использованием блокировок
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

// Сумма с использованием OpenMP редукции
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

    size_t arraySize;
    std::cout << "Enter the size of the array: ";
    std::cin >> arraySize;

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

    std::ofstream logFile("reduction_methods_log.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    logFile << "Array size: " << arraySize << "\n";
    logFile << "Execution times:\n";
    logFile << "Atomic: " << timeAtomic << " seconds\n";
    logFile << "Critical: " << timeCritical << " seconds\n";
    logFile << "Lock: " << timeLock << " seconds\n";
    logFile << "OpenMP reduction: " << timeOpenMP << " seconds\n";
    logFile << "------------------------------------------\n";

    logFile.close();

    return 0;
}
