# -*- coding: utf-8 -*-

#include <iostream>

#include <antlr4-runtime.h>
#include <pybind11/embed.h>
#include "JSONLexer.h"
#include "JSONParser.h"
#include "JSONBaseListener.h"

namespace py = pybind11;
using namespace py::literals;
using namespace antlr4mw;
using namespace antlr4;

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
    try {
        py::scoped_interpreter guard{};

        std::ifstream stream;

        // This imports example.py from app/example.py
        // The app folder is the root folder so you don't need to specify app.example.
        // The app/example script that is being imported is from the actual build folder!
        // Cmake will copy the python scripts after you have compiled the source code.
        std::cout << "Importing module..." << std::endl;
        auto example = py::module::import("app.example");

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

