#ifndef CONSOLE_H
#define CONSOLE_H
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
class Console{
public:
    inline static std::streambuf *cout_buffer, *cout_backup;
    inline static std::streambuf *cerr_buffer, *cerr_backup;

    inline static std::ofstream file_stream;

    inline static std::stringstream string_stream;

    inline static
    void EnableLoggingFile(){
        Console::file_stream.open("engine_log.txt");

        Console::cout_backup = std::cout.rdbuf();
        cout_buffer = string_stream.rdbuf();
        std::cout.rdbuf(Console::cout_buffer);

        Console::cerr_backup = std::cerr.rdbuf();
        cerr_buffer = string_stream.rdbuf();
        std::cerr.rdbuf(Console::cerr_buffer);
    }

    inline static void CloseLoggingFile(){
        Console::file_stream << string_stream.str();
        Console::file_stream.close();
        std::cout.rdbuf(cout_backup);

        //Console::file_stream << string_stream.str();
        //Console::file_stream.close();
        std::cerr.rdbuf(cerr_backup);
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
    std::string Input(const std::string& _s){
        std::cout << _s;

        std::string v_in;
        getline(std::cin, v_in, '\n');
        return v_in;
    }
};

#endif
