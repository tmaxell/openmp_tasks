#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>

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

// Функция для последовательного поиска минимальных значений в строках
int sequentialMaxOfMins(const std::vector<std::vector<int>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    std::vector<int> minValues(rows);

    for (int i = 0; i < rows; ++i) {
        int minVal = matrix[i][0];
        for (int j = 1; j < cols; ++j) {
            if (matrix[i][j] < minVal) {
                minVal = matrix[i][j];
            }
        }
        minValues[i] = minVal;
    }

    int maxOfMins = minValues[0];
    for (int i = 1; i < rows; ++i) {
        if (minValues[i] > maxOfMins) {
            maxOfMins = minValues[i];
        }
    }

    return maxOfMins;
}

// Функция для параллельного поиска минимальных значений в строках
int parallelMaxOfMins(const std::vector<std::vector<int>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    std::vector<int> minValues(rows);

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

    int maxOfMins = minValues[0];
#pragma omp parallel for reduction(max:maxOfMins)
    for (int i = 1; i < rows; ++i) {
        if (minValues[i] > maxOfMins) {
            maxOfMins = minValues[i];
        }
    }

    return maxOfMins;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Размеры матрицы
    int rows = 1000;
    int cols = 1000;

    auto matrix = generateMatrix(rows, cols, 1, 100);

    std::ofstream logFile("matrix_log.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Cannot open log file.\n";
        return 1;
    }

    // Последовательное выполнение
    double start_time_seq = omp_get_wtime();
    int seqResult = sequentialMaxOfMins(matrix);
    double end_time_seq = omp_get_wtime();
    double execution_time_seq = end_time_seq - start_time_seq;

    // Параллельное выполнение
    double start_time_parallel = omp_get_wtime();
    int parallelResult = parallelMaxOfMins(matrix);
    double end_time_parallel = omp_get_wtime();
    double execution_time_parallel = end_time_parallel - start_time_parallel;

    if (seqResult != parallelResult) {
        std::cerr << "Error: Results do not match! Sequential: " << seqResult
                  << ", Parallel: " << parallelResult << "\n";
        return 1;
    }

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Sequential Maximum value among the minimums of each row: " << seqResult << "\n";
    std::cout << "Parallel Maximum value among the minimums of each row: " << parallelResult << "\n";
    std::cout << "Sequential Execution time: " << execution_time_seq << " seconds\n";
    std::cout << "Parallel Execution time: " << execution_time_parallel << " seconds\n";

    double speedup = execution_time_seq / execution_time_parallel;
    std::cout << "Speedup (sequential / parallel): " << speedup << "\n";

    logFile << "Matrix size: " << rows << "x" << cols << "\n";
    logFile << "Sequential Maximum value: " << seqResult << "\n";
    logFile << "Parallel Maximum value: " << parallelResult << "\n";
    logFile << "Sequential Execution time: " << execution_time_seq << " seconds\n";
    logFile << "Parallel Execution time: " << execution_time_parallel << " seconds\n";
    logFile << "Speedup (sequential / parallel): " << speedup << "\n";
    logFile << "------------------------------------------\n";

    logFile.close();

    return 0;
}
