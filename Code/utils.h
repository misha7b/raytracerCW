#include <random>

static float randomFloat() {
    static std::mt19937 generator(1337); 
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(generator);
}