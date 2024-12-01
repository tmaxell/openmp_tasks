#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>

// Функция для генерации случайной матрицы
std::vector<std::vector<int>> generateMatrix(int rows, int cols, int minValue, int maxValue) {
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
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

int main() {
    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Размеры матрицы
    int rows = 5; // Количество строк
    int cols = 6; // Количество столбцов

    // Генерация случайной матрицы
    auto matrix = generateMatrix(rows, cols, 1, 100);

    // Вывод матрицы
    std::cout << "Generated Matrix:\n";
    printMatrix(matrix);

    // Вектор минимальных значений строк
    std::vector<int> minValues(rows);

    // Поиск минимальных значений в каждой строке с использованием OpenMP
#pragma omp parallel for
    for (int i = 0; i < rows; ++i) {
        int minVal = matrix[i][0];
        for (int j = 1; j < cols; ++j) {
            if (matrix[i][j] < minVal) {
                minVal = matrix[i][j];
            }
        }
        minValues[i] = minVal;
    }

    // Поиск максимального значения среди минимальных
    int maxOfMins = minValues[0];
#pragma omp parallel for reduction(max:maxOfMins)
    for (int i = 1; i < rows; ++i) {
        if (minValues[i] > maxOfMins) {
            maxOfMins = minValues[i];
        }
    }

    // Вывод результата
    std::cout << "Maximum value among the minimums of each row: " << maxOfMins << "\n";

    return 0;
}
