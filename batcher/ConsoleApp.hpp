/**
     * Class Name:		ConsoleApp.hpp
     * Purpose:         header file for the console app 
     * Coder:			Dylan Mahyuddin
     * Date:			2022-10-13
*/
#pragma once
#if defined(_WIN32)
#include <crtdbg.h>
#include <Windows.h>
#endif
#include <deque>
#include <string>

class ConsoleApp {
    static ConsoleApp* thisApp_sm;
    friend int main(int, char* []);

    using CommandLineArgs = std::deque<std::string>;
    CommandLineArgs args_m;
protected:
    ConsoleApp();
    virtual int execute() = 0;
    CommandLineArgs const& get_args() const { return args_m; }
};

#define MAKEAPP(name)\
    class name : public ConsoleApp {\
    int execute() override;\
}name ## _g;\
int name::execute()