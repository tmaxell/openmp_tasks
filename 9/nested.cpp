#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <climits>

void generateMatrix(std::vector<std::vector<int>>& matrix, int minValue, int maxValue) {
    for (auto& row : matrix) {
        for (auto& elem : row) {
            elem = minValue + std::rand() % (maxValue - minValue + 1);
        }
    }
}

// Поиск максимума среди минимумов (без вложенного параллелизма)
int findMaxOfMinsNoNested(const std::vector<std::vector<int>>& matrix) {
    int maxOfMins = INT_MIN;

#pragma omp parallel for reduction(max:maxOfMins)
    for (size_t i = 0; i < matrix.size(); ++i) {
        int rowMin = INT_MAX;
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            if (matrix[i][j] < rowMin) {
                rowMin = matrix[i][j];
            }
        }
        if (rowMin > maxOfMins) {
            maxOfMins = rowMin;
        }
    }

    return maxOfMins;
}

// Поиск максимума среди минимумов (с вложенным параллелизмом)
int findMaxOfMinsNested(const std::vector<std::vector<int>>& matrix) {
    int maxOfMins = INT_MIN;

#pragma omp parallel for reduction(max:maxOfMins)
    for (size_t i = 0; i < matrix.size(); ++i) {
        int rowMin = INT_MAX;

#pragma omp parallel for reduction(min:rowMin)
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            if (matrix[i][j] < rowMin) {
                rowMin = matrix[i][j];
            }
        }

        if (rowMin > maxOfMins) {
            maxOfMins = rowMin;
        }
    }

    return maxOfMins;
}

int main() {
    int numRows, numCols;

    // Ввод размеров матрицы
    std::cout << "Enter the number of rows: ";
    std::cin >> numRows;
    std::cout << "Enter the number of columns: ";
    std::cin >> numCols;

    if (numRows <= 0 || numCols <= 0) {
        std::cerr << "Matrix dimensions must be positive integers.\n";
        return 1;
    }

    std::vector<std::vector<int>> matrix(numRows, std::vector<int>(numCols));

    std::srand(static_cast<unsigned int>(std::time(0)));
    generateMatrix(matrix, 1, 100);

    // Включение вложенного параллелизма
    omp_set_nested(1);

    // Поиск максимума среди минимумов без вложенного параллелизма
    double startTime = omp_get_wtime();
    int maxNoNested = findMaxOfMinsNoNested(matrix);
    double endTime = omp_get_wtime();
    double timeNoNested = endTime - startTime;
    std::cout << "Max (no nested): " << maxNoNested << " in " << timeNoNested << " seconds.\n";

    // Поиск максимума среди минимумов с вложенным параллелизмом
    startTime = omp_get_wtime();
    int maxNested = findMaxOfMinsNested(matrix);
    endTime = omp_get_wtime();
    double timeNested = endTime - startTime;
    std::cout << "Max (nested): " << maxNested << " in " << timeNested << " seconds.\n";

    std::ofstream logFile("matrix_max_of_mins.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file.\n";
        return 1;
    }

    logFile << "Matrix Dimensions: " << numRows << "x" << numCols << "\n";
    logFile << "Max (no nested): " << maxNoNested << " in " << timeNoNested << " seconds.\n";
    logFile << "Max (nested): " << maxNested << " in " << timeNested << " seconds.\n";
    logFile << "------------------------------------------\n";

    logFile.close();

    return 0;
}
