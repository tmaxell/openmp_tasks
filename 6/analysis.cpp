#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>

// Симуляция тяжелых вычислений
void simulateWork(int load) {
    double result = 0.0;
    for (int i = 0; i < load; ++i) {
        result += std::sin(i) * std::cos(i); // Имитация вычислительной нагрузки
    }
}

// Функция для тестирования распределения итераций
double testScheduling(int numIterations, const std::string& scheduleType, int chunkSize = 0) {
    double startTime, endTime;
    std::vector<int> loads(numIterations);

    // Генерация случайных нагрузок для итераций
    for (int i = 0; i < numIterations; ++i) {
        loads[i] = rand() % 1000 + 100; // Случайная нагрузка от 100 до 1099
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

    int numIterations = 10000; // Количество итераций
    int chunkSize = 10;        // Размер чанка для dynamic и guided

    std::ofstream logFile("scheduling_log.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    // Измерение времени выполнения для разных типов планирования
    double timeStatic = testScheduling(numIterations, "static");
    double timeDynamic = testScheduling(numIterations, "dynamic", chunkSize);
    double timeGuided = testScheduling(numIterations, "guided", chunkSize);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Execution time with static scheduling: " << timeStatic << " seconds\n";
    std::cout << "Execution time with dynamic scheduling: " << timeDynamic << " seconds\n";
    std::cout << "Execution time with guided scheduling: " << timeGuided << " seconds\n";

    logFile << "Number of iterations: " << numIterations << "\n";
    logFile << "Chunk size: " << chunkSize << "\n";
    logFile << "Execution time with static scheduling: " << timeStatic << " seconds\n";
    logFile << "Execution time with dynamic scheduling: " << timeDynamic << " seconds\n";
    logFile << "Execution time with guided scheduling: " << timeGuided << " seconds\n";
    logFile << "------------------------------------------\n";

    logFile.close();

    return 0;
}
