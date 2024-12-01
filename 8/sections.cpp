#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <string>

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

int computeDotProduct(const std::vector<int>& vec1, const std::vector<int>& vec2) {
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
    int dotProductResult = 0;

    try {
        // Распараллеливание задач с помощью sections
#pragma omp parallel sections
        {
#pragma omp section
            {
                std::cout << "Reading vectors from file...\n";
                readVectorsFromFile(filename, vec1, vec2);
                std::cout << "Vectors loaded. Size: " << vec1.size() << "\n";
            }

#pragma omp section
            {
                std::cout << "Waiting for vectors to calculate dot product...\n";
#pragma omp barrier
                if (!vec1.empty() && !vec2.empty()) {
                    dotProductResult = computeDotProduct(vec1, vec2);
                }
            }
        }

        std::cout << "Dot product result: " << dotProductResult << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
