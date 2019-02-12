#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <Windows.h> // Needed by GetModuleFileNameW
#else
#include <libgen.h> // Needed by readlink
#endif

#include <iostream>

#include <antlr4-runtime.h>
#include "JSONLexer.h"
#include "JSONParser.h"
#include "JSONBaseListener.h"


#include <Python.h>
#ifndef IS_WINDOWS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/embed.h>
#ifndef IS_WINDOWS
#pragma GCC diagnostic pop
#endif

namespace py = pybind11;
using namespace py::literals;
using namespace antlr4mw;
using namespace antlr4;

///=============================================================================
#ifdef IS_WINDOWS
std::wstring getExecutableDir() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    const auto executableDir = std::wstring(exePath);
    const auto pos = executableDir.find_last_of('\\');
    if (pos != std::string::npos) return executableDir.substr(0, pos);
    return L"\\";
}
#else
std::wstring getExecutableDir() {
    return L"/Users/kimrin/MW/bin";
}
#endif


class TreeShapeListener : public JSONBaseListener
{
  public:
    void enterPair(JSONParser::PairContext *ctx) override
    {
        // Do something when entering the key rule.
        std::cout << ctx << std::endl;
    }
};

int main(int argc, const char *argv[])
{
    // Get executable dir and build python PATH variable
    const auto exeDir = getExecutableDir();
    std::wcout << exeDir << std::endl;


#ifdef IS_WINDOWS
    const auto pythonHome = exeDir + L"\\lib";
    const auto pythonPath = exeDir + L"\\lib;" + exeDir + L"\\app;";
#else
    const auto pythonHome = exeDir;
    const auto pythonPath = exeDir + L"/Lib:" + exeDir + L"/lib-dylib:" + exeDir + L"/app";
#endif

    // Initialize python
    Py_OptimizeFlag = 1;
    std::cout << 1 << std::endl;
    Py_SetProgramName(L"mw");
    std::cout << 2 << std::endl;
    Py_SetPath(pythonPath.c_str());
    std::cout << 3 << std::endl;
    Py_SetPythonHome(pythonHome.c_str());
    std::cout << 4 << std::endl;

    std::wcout << "Python PATH set to: " << pythonPath << std::endl;
    std::wcout << "Python HOME set to: " << pythonHome << std::endl;

    try {
        py::scoped_interpreter guard{};

        std::ifstream stream;

        // Disable build of __pycache__ folders
        py::exec(R"(
            import sys
            sys.dont_write_bytecode = True
        )");

        // This imports example.py from app/example.py
        // The app folder is the root folder so you don't need to specify app.example.
        // The app/example script that is being imported is from the actual build folder!
        // Cmake will copy the python scripts after you have compiled the source code.
        std::cout << "Importing module..." << std::endl;
        auto example = py::module::import("example");

        std::cout << "Initializing class..." << std::endl;
        const auto myExampleClass = example.attr("Example");
        auto myExampleInstance = myExampleClass("Hello World"); // Calls the constructor
        // Will print in the terminal window:
        // Example constructor with msg: Hello World

        const auto msg = myExampleInstance.attr("getMsg")(); // Calls the getMsg
        std::cout << "Got msg back on C++ side: " << msg.cast<std::string>() << std::endl;

        stream.open(argv[1]);
        ANTLRInputStream input(stream);
        JSONLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        tokens.fill();
        for (auto token : tokens.getTokens())
        {
            std::cout << token->toString() << std::endl;
        }
        JSONParser parser(&tokens);

        tree::ParseTree *tree = parser.json();
        std::cout << tree->toStringTree(&parser) << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Something went wrong: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

