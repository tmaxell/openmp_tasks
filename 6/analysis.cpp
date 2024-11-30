#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Симуляция тяжелых вычислений
void simulateWork(int load) {
    double result = 0.0;
    for (int i = 0; i < load; ++i) {
        result += std::sin(i) * std::cos(i); // Тут операция
    }
}

// Функция для тестирования распределения итераций
double testScheduling(int numIterations, const std::string& scheduleType, int chunkSize = 0) {
    double startTime, endTime;
    std::vector<int> loads(numIterations);

    // Генерация случайных нагрузок для итераций
    for (int i = 0; i < numIterations; ++i) {
        loads[i] = rand() % 1000 + 100; 
    }

    startTime = omp_get_wtime();

    if (scheduleType == "static") {
#pragma omp parallel for schedule(static)
        for (int i = 0; i < numIterations; ++i) {
            simulateWork(loads[i]);
        }
    } else if (scheduleType == "dynamic") {
#pragma omp parallel for schedule(dynamic, chunkSize)
        for (int i = 0; i < numIterations; ++i) {
            simulateWork(loads[i]);
        }
    } else if (scheduleType == "guided") {
#pragma omp parallel for schedule(guided, chunkSize)
        for (int i = 0; i < numIterations; ++i) {
            simulateWork(loads[i]);
        }
    }

    endTime = omp_get_wtime();

    return endTime - startTime;
}

int main() {

    std::srand(static_cast<unsigned int>(std::time(0)));

    int numIterations = 10000; 
    int chunkSize = 10;        

    double timeStatic = testScheduling(numIterations, "static");
    double timeDynamic = testScheduling(numIterations, "dynamic", chunkSize);
    double timeGuided = testScheduling(numIterations, "guided", chunkSize);

    std::cout << "Execution time with static scheduling: " << timeStatic << " seconds\n";
    std::cout << "Execution time with dynamic scheduling: " << timeDynamic << " seconds\n";
    std::cout << "Execution time with guided scheduling: " << timeGuided << " seconds\n";

    return 0;
}
