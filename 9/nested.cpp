#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>

void generateMatrix(std::vector<std::vector<int>>& matrix, int minValue, int maxValue) {
    for (auto& row : matrix) {
        for (auto& elem : row) {
            elem = minValue + std::rand() % (maxValue - minValue + 1);
        }
    }
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
    const int numRows = 1000;
    const int numCols = 1000;
    std::vector<std::vector<int>> matrix(numRows, std::vector<int>(numCols));

    std::srand(static_cast<unsigned int>(std::time(0)));
    generateMatrix(matrix, 1, 100);

    omp_set_nested(1);



    // Поиск максимума среди минимумов с вложенным параллелизмом
    startTime = omp_get_wtime();
    int maxNested = findMaxOfMinsNested(matrix);
    endTime = omp_get_wtime();
    std::cout << "Max (nested): " << maxNested << " in " << (endTime - startTime) << " seconds.\n";

    return 0;
}
