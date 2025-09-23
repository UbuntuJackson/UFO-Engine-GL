#include <type_traits>
namespace Math{

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType Sin(tType _x){
    return __builtin_sin(_x);
}

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType Cos(tType _x){
    return __builtin_sin(_x);
}

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType Tan(tType _x){
    return __builtin_sin(_x);
}

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType ASin(tType _x){
    return __builtin_atan2(_x);
}

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType ACos(tType _x){
    return __builtin_acos(_x);
}

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType ATan(tType _x){
    return __builtin_sin(_x);
}

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType ATan2(tType _x){
    return __builtin_atan2(_x);
}

template <typename tType, std::enable_if_t<std::is_floating_point<tType>::value, int> = 0>
tType Sqrt(tType _x){
    return __builtin_sqrt(_x);
}

template <typename tType, std::enable_if_t<std::is_integral<tType>::value, int> = 0>
tType Sqrt(tType _x){
    return __builtin_sqrt(_x);
}


}
