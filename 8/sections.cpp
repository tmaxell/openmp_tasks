#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
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

// Параллельное вычисление скалярного произведения с использованием OpenMP
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
    std::string filename = "vectors.txt";  // Файл с векторами
    std::vector<int> vec1, vec2;
    int dotProductSequential = 0, dotProductParallel = 0;

    try {
        double startTimeTotal = omp_get_wtime(); // Время начала всех операций

        #pragma omp parallel sections
        {
            // Секция нумеро уно для чтения векторов из файла
            #pragma omp section
            {
                double startTimeRead = omp_get_wtime(); // Время начала чтения
                std::cout << "Reading vectors from file...\n";
                readVectorsFromFile(filename, vec1, vec2);
                double endTimeRead = omp_get_wtime(); // Время окончания чтения
                std::cout << "Vectors loaded. Size: " << vec1.size() << "\n";
                std::cout << "Time for reading vectors: " << (endTimeRead - startTimeRead) << " seconds\n";
            }

            // Секция драй для вычисления скалярного произведения
            #pragma omp section
            {
                double startTimeComputeSeq = omp_get_wtime(); // Время начала вычисления для последовательного
                std::cout << "Calculating dot product (sequential)...\n";
                dotProductSequential = computeDotProductSequential(vec1, vec2);
                double endTimeComputeSeq = omp_get_wtime(); // Время окончания вычисления для последовательного
                std::cout << "Dot product result (sequential): " << dotProductSequential << "\n";
                std::cout << "Time for calculating dot product (sequential): " << (endTimeComputeSeq - startTimeComputeSeq) << " seconds\n";

                double startTimeComputePar = omp_get_wtime(); // Время начала вычисления для параллельного
                std::cout << "Calculating dot product (parallel)...\n";
                dotProductParallel = computeDotProductParallel(vec1, vec2);
                double endTimeComputePar = omp_get_wtime(); // Время окончания вычисления для параллельного
                std::cout << "Dot product result (parallel): " << dotProductParallel << "\n";
                std::cout << "Time for calculating dot product (parallel): " << (endTimeComputePar - startTimeComputePar) << " seconds\n";
            }
        }

        double endTimeTotal = omp_get_wtime(); // Время окончания всех операций
        std::cout << "Total time for all operations: " << (endTimeTotal - startTimeTotal) << " seconds\n";

        // Запись в лог-файл
        std::ofstream logFile("dot_product_log.log", std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }

        logFile << "Dot Product Calculation:\n";
        logFile << "Vector size: " << vec1.size() << "\n";
        logFile << "Sequential result: " << dotProductSequential << "\n";
        logFile << "Parallel result: " << dotProductParallel << "\n";
        logFile << "Time for reading vectors: " << (endTimeTotal - startTimeTotal) << " seconds\n";
        logFile << "Time for calculating dot product (sequential): " << (endTimeTotal - startTimeTotal) << " seconds\n";
        logFile << "Time for calculating dot product (parallel): " << (endTimeTotal - startTimeTotal) << " seconds\n";
        logFile << "------------------------------------------\n";

        logFile.close();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
