#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <iomanip>

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

    // Ветвление для выбора типа планирования итераций
    if (rule == 1) {
#pragma omp parallel for schedule(static)
        for (int i = 0; i < size; ++i) {
            int minVal = matrix[i][0];
            for (int j = 1; j < size; ++j) {
                if (matrix[i][j] != 0 && matrix[i][j] < minVal) {
                    minVal = matrix[i][j];
                }
            }
            minValues[i] = minVal;
        }
    } else if (rule == 2) {
#pragma omp parallel for schedule(dynamic, 10)
        for (int i = 0; i < size; ++i) {
            int minVal = matrix[i][0];
            for (int j = 1; j < size; ++j) {
                if (matrix[i][j] != 0 && matrix[i][j] < minVal) {
                    minVal = matrix[i][j];
                }
            }
            minValues[i] = minVal;
        }
    } else if (rule == 3) {
#pragma omp parallel for schedule(guided)
        for (int i = 0; i < size; ++i) {
            int minVal = matrix[i][0];
            for (int j = 1; j < size; ++j) {
                if (matrix[i][j] != 0 && matrix[i][j] < minVal) {
                    minVal = matrix[i][j];
                }
            }
            minValues[i] = minVal;
        }
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

    // Ввод параметров
    int size, bandwidth, minValue, maxValue;
    std::cout << "Enter the size of the matrix: ";
    std::cin >> size;
    std::cout << "Enter the bandwidth of the matrix: ";
    std::cin >> bandwidth;
    std::cout << "Enter the minimum value for matrix elements: ";
    std::cin >> minValue;
    std::cout << "Enter the maximum value for matrix elements: ";
    std::cin >> maxValue;

    // Генерация ленточной матрицы
    auto matrix = generateBandMatrix(size, bandwidth, minValue, maxValue);

    // Вывод части матрицы для проверки (если размер небольшой)
    if (size <= 20) {
        std::cout << "Band Matrix:\n";
        printMatrix(matrix);
    }

    // Открытие лог-файла
    std::ofstream logFile("band_matrix_log.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    // Измерение времени выполнения для разных правил распределения
    double timeStatic = measureExecutionTime(matrix, 1);
    double timeDynamic = measureExecutionTime(matrix, 2);
    double timeGuided = measureExecutionTime(matrix, 3);

    // Вывод времени выполнения
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Execution time with static scheduling: " << timeStatic << " seconds\n";
    std::cout << "Execution time with dynamic scheduling: " << timeDynamic << " seconds\n";
    std::cout << "Execution time with guided scheduling: " << timeGuided << " seconds\n";

    logFile << "Matrix size: " << size << "x" << size << "\n";
    logFile << "Bandwidth: " << bandwidth << "\n";
    logFile << "Execution time with static scheduling: " << timeStatic << " seconds\n";
    logFile << "Execution time with dynamic scheduling: " << timeDynamic << " seconds\n";
    logFile << "Execution time with guided scheduling: " << timeGuided << " seconds\n";
    logFile << "------------------------------------------\n";

    logFile.close();

    return 0;
}
