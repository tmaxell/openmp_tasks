#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <string>
#include <stdexcept>

// Функция для чтения векторов из файла
void readVectorsFromFile(const std::string& filename, std::vector<int>& vec1, std::vector<int>& vec2) {
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    int value;
    bool toggle = true;

    while (inputFile >> value) {
        if (toggle) {
            vec1.push_back(value);
        } else {
            vec2.push_back(value);
        }
        toggle = !toggle;
    }

    inputFile.close();
}

// Последовательное вычисление скалярного произведения
int computeDotProductSequential(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same size for dot product calculation.");
    }

    int dotProduct = 0;
    for (size_t i = 0; i < vec1.size(); ++i) {
        dotProduct += vec1[i] * vec2[i];
    }

    return dotProduct;
}

// Параллельное вычисление скалярного произведения
int computeDotProductParallel(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same size for dot product calculation.");
    }

    int dotProduct = 0;
#pragma omp parallel for reduction(+:dotProduct)
    for (size_t i = 0; i < vec1.size(); ++i) {
        dotProduct += vec1[i] * vec2[i];
    }

    return dotProduct;
}

int main() {
    std::string filename = "vectors.txt";
    std::vector<int> vec1, vec2;
    int dotProductSequential = 0, dotProductParallel = 0;

    try {
        // Чтение векторов из файла
        std::cout << "Reading vectors from file...\n";
        readVectorsFromFile(filename, vec1, vec2);
        std::cout << "Vectors loaded. Size: " << vec1.size() << "\n";

        // Последовательное вычисление
        double startTimeSeq = omp_get_wtime();
        dotProductSequential = computeDotProductSequential(vec1, vec2);
        double endTimeSeq = omp_get_wtime();

        // Параллельное вычисление
        double startTimePar = omp_get_wtime();
        dotProductParallel = computeDotProductParallel(vec1, vec2);
        double endTimePar = omp_get_wtime();

        std::cout << "Dot product result (sequential): " << dotProductSequential << "\n";
        std::cout << "Dot product result (parallel): " << dotProductParallel << "\n";
        std::cout << "Sequential execution time: " << (endTimeSeq - startTimeSeq) << " seconds\n";
        std::cout << "Parallel execution time: " << (endTimePar - startTimePar) << " seconds\n";

        std::ofstream logFile("dot_product_log.log", std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }

        logFile << "Dot Product Calculation:\n";
        logFile << "Vector size: " << vec1.size() << "\n";
        logFile << "Sequential result: " << dotProductSequential << "\n";
        logFile << "Parallel result: " << dotProductParallel << "\n";
        logFile << "Sequential execution time: " << (endTimeSeq - startTimeSeq) << " seconds\n";
        logFile << "Parallel execution time: " << (endTimePar - startTimePar) << " seconds\n";
        logFile << "------------------------------------------\n";

        logFile.close();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
