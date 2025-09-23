#ifndef RANDOM_NUMBER_GENERATOR
#define RANDOM_NUMBER_GENERATOR

#include <random>

class RandomNumberGenerator{
public:
    std::random_device device;
    std::default_random_engine random_engine;
    RandomNumberGenerator();
    static RandomNumberGenerator& Get();
    int RandomInt(int _min, int _max);
    float RandomFloat(float _min, float _max);
};

#endif