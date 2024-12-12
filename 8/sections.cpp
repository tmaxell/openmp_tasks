#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <sstream>

// Функция для генерации файла с векторами
void generateVectors(const std::string& filename, int size) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Ошибка открытия файла для записи." << std::endl;
        exit(1);
    }

    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < size; ++i) {
        outFile << rand() % 100 << " ";
    }
    outFile << std::endl;

    for (int i = 0; i < size; ++i) {
        outFile << rand() % 100 << " ";
    }

    outFile.close();
}

// Функция для чтения векторов из файла
void readVectors(const std::string& filename, std::vector<int>& vec1, std::vector<int>& vec2) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Ошибка открытия файла для чтения." << std::endl;
        exit(1);
    }

    std::string line;

    // Чтение первой строки вектора
    if (std::getline(inFile, line)) {
        std::istringstream iss(line);
        int value;
        while (iss >> value) {
            vec1.push_back(value);
        }
    }

    // Чтение второй строки вектора
    if (std::getline(inFile, line)) {
        std::istringstream iss(line);
        int value;
        while (iss >> value) {
            vec2.push_back(value);
        }
    }

    if (vec1.size() != vec2.size()) {
        std::cerr << "Ошибка: вектора имеют разную размерность." << std::endl;
        exit(1);
    }

    inFile.close();
}

// Функция для вычисления скалярного произведения
int computeDotProduct(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    int dotProduct = 0;

    #pragma omp parallel for reduction(+:dotProduct)
    for (size_t i = 0; i < vec1.size(); ++i) {
        dotProduct += vec1[i] * vec2[i];
    }

    return dotProduct;
}

int main() {
    std::string filename = "vectors.txt";
    int size;

    std::cout << "Введите размерность векторов: ";
    std::cin >> size;

    if (size <= 0) {
        std::cerr << "Ошибка: размерность векторов должна быть положительным числом." << std::endl;
        return 1;
    }

    // Генерация файла с векторами
    generateVectors(filename, size);

    std::vector<int> vec1, vec2;

    double readTime = 0.0, computeTime = 0.0;

    // Секция нумеро уно для чтения векторов из файла
    #pragma omp sections
    {
        #pragma omp section
        {
            double start = omp_get_wtime();
            readVectors(filename, vec1, vec2);
            readTime = omp_get_wtime() - start;
        }
    }

    // Секция драй для вычисления скалярного произведения
    int result = 0;
    #pragma omp sections
    {
        #pragma omp section
        {
            double start = omp_get_wtime();
            result = computeDotProduct(vec1, vec2);
            computeTime = omp_get_wtime() - start;
        }
    }

    // Вывод результатов
    std::cout << "Скалярное произведение: " << result << std::endl;
    std::cout << "Время чтения данных: " << readTime << " секунд" << std::endl;
    std::cout << "Время вычисления: " << computeTime << " секунд" << std::endl;

    // Запись в лог
    std::ofstream logFile("log.txt", std::ios::app);
    if (logFile) {
        logFile << "Размерность: " << size << "\n"
                << "Скалярное произведение: " << result << "\n"
                << "Время чтения данных: " << readTime << " секунд\n"
                << "Время вычисления: " << computeTime << " секунд\n";
        logFile.close();
    } else {
        std::cerr << "Ошибка записи в лог-файл." << std::endl;
    }

    return 0;
}
