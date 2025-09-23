#include <random>
#include "random_number_generator.h"

RandomNumberGenerator::RandomNumberGenerator(){
    random_engine = std::default_random_engine(device());
}

RandomNumberGenerator& RandomNumberGenerator::Get(){
    static RandomNumberGenerator rng;
    return rng;
}

int RandomNumberGenerator::RandomInt(int _min, int _max){
    std::uniform_int_distribution<int> uniform_dist(_min,_max);
    return uniform_dist(random_engine);
}
float RandomNumberGenerator::RandomFloat(float _min, float _max){
    std::uniform_real_distribution<float> uniform_dist(_min,_max);
    return uniform_dist(random_engine);
}