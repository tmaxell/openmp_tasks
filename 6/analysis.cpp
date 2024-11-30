#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Симуляция тяжелых вычислений
void simulateWork(int load) {
    double result = 0.0;
    for (int i = 0; i < load; ++i) {
        result += std::sin(i) * std::cos(i); // Тут операция
    }
}


