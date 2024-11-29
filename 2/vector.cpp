#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

int main() {
    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Размер векторов
    int n = 10; // Вы можете изменить размер на любое значение

    // Генерация случайных векторов A и B
    std::vector<int> A(n);
    std::vector<int> B(n);

    for (int i = 0; i < n; ++i) {
        A[i] = std::rand() % 100; // Значения от 0 до 99
        B[i] = std::rand() % 100; // Значения от 0 до 99
    }

    // Вывод сгенерированных векторов
    std::cout << "Vector A: ";
    for (int val : A) std::cout << val << " ";
    std::cout << "\n";

    std::cout << "Vector B: ";
    for (int val : B) std::cout << val << " ";
    std::cout << "\n";

    // Скалярное произведение с использованием редукции
    int dot_product_reduction = 0;

#pragma omp parallel for reduction(+:dot_product_reduction)
    for (int i = 0; i < n; ++i) {
        dot_product_reduction += A[i] * B[i];
    }

    std::cout << "Dot product using reduction: " << dot_product_reduction << "\n";

    // Скалярное произведение без использования редукции
    int dot_product_no_reduction = 0;

#pragma omp parallel
    {
        int local_sum = 0;

#pragma omp for
        for (int i = 0; i < n; ++i) {
            local_sum += A[i] * B[i];
        }

#pragma omp critical
        {
            dot_product_no_reduction += local_sum;
        }
    }

    std::cout << "Dot product without reduction: " << dot_product_no_reduction << "\n";

    return 0;
}
