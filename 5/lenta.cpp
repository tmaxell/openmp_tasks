#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Функция для генерации ленточной матрицы
std::vector<std::vector<int>> generateBandMatrix(int size, int bandwidth, int minValue, int maxValue) {
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size, 0));
    for (int i = 0; i < size; ++i) {
        for (int j = std::max(0, i - bandwidth); j <= std::min(size - 1, i + bandwidth); ++j) {
            matrix[i][j] = minValue + std::rand() % (maxValue - minValue + 1);
        }
    }
    return matrix;
}

// Функция для вывода матрицы
void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

// Функция для измерения времени выполнения
double measureExecutionTime(const std::vector<std::vector<int>>& matrix, int rule) {
    int size = matrix.size();
    std::vector<int> minValues(size);

    double startTime = omp_get_wtime();

    // Один блок с ветвлением внутри
#pragma omp parallel for schedule(static) if (rule == 1) \
                                 schedule(dynamic, 10) if (rule == 2) \
                                 schedule(guided) if (rule == 3)
    for (int i = 0; i < size; ++i) {
        int minVal = matrix[i][0];
        for (int j = 1; j < size; ++j) {
            if (matrix[i][j] != 0 && matrix[i][j] < minVal) {
                minVal = matrix[i][j];
            }
        }
        minValues[i] = minVal;
    }

    int maxOfMins = minValues[0];
#pragma omp parallel for reduction(max:maxOfMins)
    for (int i = 1; i < size; ++i) {
        if (minValues[i] > maxOfMins) {
            maxOfMins = minValues[i];
        }
    }

    double endTime = omp_get_wtime();
    return endTime - startTime;
}


int main() {
    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Параметры матрицы
    int size = 1000;       // Размер матрицы
    int bandwidth = 10;    // Ширина ленты
    int minValue = 1;      // Минимальное значение элементов
    int maxValue = 100;    // Максимальное значение элементов

    // Генерация ленточной матрицы
    auto matrix = generateBandMatrix(size, bandwidth, minValue, maxValue);

    // Вывод части матрицы для проверки (если размер небольшой)
    if (size <= 20) {
        std::cout << "Band Matrix:\n";
        printMatrix(matrix);
    }

    // Измерение времени выполнения для разных правил распределения
    double timeStatic = measureExecutionTime(matrix, 1);
    double timeDynamic = measureExecutionTime(matrix, 2);
    double timeGuided = measureExecutionTime(matrix, 3);

    // Вывод времени выполнения
    std::cout << "Execution time with static scheduling: " << timeStatic << " seconds\n";
    std::cout << "Execution time with dynamic scheduling: " << timeDynamic << " seconds\n";
    std::cout << "Execution time with guided scheduling: " << timeGuided << " seconds\n";

    return 0;
}
