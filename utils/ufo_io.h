#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

namespace ufo{

template<typename Arg, typename ...Args>
void PrintLine(Arg&& arg, Args&& ...args){
    std::cout << arg;
    ((std::cout << " " << args), ...);
    std::cout << std::endl;
}

//Same as PrintLine except it doesn't flush for you
template<typename Arg, typename ...Args>
void Print(Arg&& arg, Args&& ...args){
    std::cout << arg;
    ((std::cout << " " << args), ...);
}

std::string Input(const std::string& _s){
    std::cout << _s;

    std::string v_in;
    getline(std::cin, v_in, '\n');
    return v_in;
}

}
