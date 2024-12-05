#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <fstream>

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

    std::ofstream logFile("matrix_log.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    // Последовательный поиск минимальных значений строк
    double start_time_seq = omp_get_wtime();
    for (int i = 0; i < rows; ++i) {
        int minVal = matrix[i][0];
        for (int j = 1; j < cols; ++j) {
            if (matrix[i][j] < minVal) {
                minVal = matrix[i][j];
            }
        }
        minValues[i] = minVal;
    }
    int maxOfMins_seq = minValues[0];
    for (int i = 1; i < rows; ++i) {
        if (minValues[i] > maxOfMins_seq) {
            maxOfMins_seq = minValues[i];
        }
    }
    double end_time_seq = omp_get_wtime();
    double execution_time_seq = end_time_seq - start_time_seq;

    // Поиск минимальных значений строк с использованием OpenMP
    double start_time_parallel = omp_get_wtime();
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

    // Поиск максимального значения среди минимальных с использованием OpenMP
    int maxOfMins_parallel = minValues[0];
#pragma omp parallel for reduction(max:maxOfMins_parallel)
    for (int i = 1; i < rows; ++i) {
        if (minValues[i] > maxOfMins_parallel) {
            maxOfMins_parallel = minValues[i];
        }
    }
    double end_time_parallel = omp_get_wtime();
    double execution_time_parallel = end_time_parallel - start_time_parallel;

    std::cout << "Sequential Maximum value among the minimums of each row: " << maxOfMins_seq << "\n";
    std::cout << "Parallel Maximum value among the minimums of each row: " << maxOfMins_parallel << "\n";
    std::cout << "Sequential Execution time: " << execution_time_seq << " seconds\n";
    std::cout << "Parallel Execution time: " << execution_time_parallel << " seconds\n";

    double speedup = execution_time_seq / execution_time_parallel;
    std::cout << "Speedup (sequential / parallel): " << speedup << "\n";

    logFile << "Matrix size: " << rows << "x" << cols << "\n";
    logFile << "Sequential Maximum value: " << maxOfMins_seq << "\n";
    logFile << "Parallel Maximum value: " << maxOfMins_parallel << "\n";
    logFile << "Sequential Execution time: " << execution_time_seq << " seconds\n";
    logFile << "Parallel Execution time: " << execution_time_parallel << " seconds\n";
    logFile << "Speedup (sequential / parallel): " << speedup << "\n";
    logFile << "------------------------------------------\n";

    logFile.close();

    return 0;
}
