// batcher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <memory>
#include <thread>
#include <vector>
/**
     * Class Name:	batcher.cpp
     * Purpose:     run the console app as a singleton
     * Coder:		Dylan Mahyuddin
     * Date:		2022-10-13
*/
#include <Windows.h>
#include <iostream>
#include <string>
#include "ConsoleApp.hpp"
using namespace std;

//singleton the console app
ConsoleApp* ConsoleApp::thisApp_sm = nullptr;
ConsoleApp::ConsoleApp() {
    if (thisApp_sm) {
        cerr << "Error: more than one instance of ConsoleApp.\n";
        exit(EXIT_FAILURE);
    }
    thisApp_sm = this;
}

//Debug flags
int main(int argc, char* argv[]) try {
#if defined(_DEBUG) && defined(_WIN32)
    int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlags |= _CRTDBG_CHECK_ALWAYS_DF;
    dbgFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
    dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlags);

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#endif
    std::set_terminate([]() {
        std::cerr << "Error (main): Unhandled exception\n";
        std::exit(EXIT_FAILURE);
        });

    ConsoleApp::thisApp_sm->args_m.assign(argv, argv + argc);

    auto exitCode = ConsoleApp::thisApp_sm->execute();
    return exitCode;
}
catch (char const* msg) {
    cerr << "Error (main) caught exception string: " << msg << endl;
    return EXIT_FAILURE;
}
catch (std::logic_error& e) {
    cerr << "Error (main) caught logic error: " << e.what() << endl;
    return EXIT_FAILURE;
}
catch (std::runtime_error& e) {
    cerr << "Error (main) caught runtime error: " << e.what() << endl;
    return EXIT_FAILURE;
}
catch (std::exception& e) {
    cerr << "Error (main) caught std::exception: " << e.what() << endl;
    return EXIT_FAILURE;
}

