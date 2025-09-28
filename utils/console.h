#ifndef CONSOLE_H
#define CONSOLE_H
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
class Console{
public:
    inline static std::streambuf *buffer, *backup;

    inline static std::ofstream file_stream;

    inline static
    void EnableLoggingFile(){
        Console::file_stream.open("engine_log.txt");

        Console::backup = std::cout.rdbuf();
        buffer = file_stream.rdbuf();

        std::cout.rdbuf(Console::buffer);
    }

    inline static void CloseLoggingFile(){
        Console::file_stream.close();
    }

    template<typename Arg, typename ...Args>
    static void Out(Arg&& arg, Args&& ...args){
        std::cout << arg;
        ((std::cout << " " << args), ...);
        std::cout << std::endl;
    }

    //Same as Out
    template<typename Arg, typename ...Args>
    static void PrintLine(Arg&& arg, Args&& ...args){
        std::cout << arg;
        ((std::cout << " " << args), ...);
        std::cout << std::endl;
    }

    //Same as PrintLine except it doesn't flush for you
    template<typename Arg, typename ...Args>
    static void Print(Arg&& arg, Args&& ...args){
        std::cout << arg;
        ((std::cout << " " << args), ...);
    }
    
    template<typename T>
    static void Out(std::vector<T> _vec){
        for(int i = 0; i < _vec.size(); i++){
            if(i != 0) std::cout << ", " << _vec[i];
            else std::cout << _vec[i];
        }
        std::cout << std::endl;
    }
    static
    std::string In(std::string _s){
        std::cout << _s;

        std::string v_in;
        getline(std::cin, v_in, '\n');
        return v_in;
    }
};

#endif