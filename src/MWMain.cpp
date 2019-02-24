// -*- coding: utf-8 -*-

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <antlr4-runtime.h>
#include <pybind11/embed.h>

#include "JSONLexer.h"
#include "JSONParser.h"
#include "JSONBaseListener.h"

namespace py = pybind11;
using namespace py::literals;

using namespace antlr4mw;
using namespace antlr4;

using namespace llvm;

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

#if 0
        // This imports example.py from app/example.py
        // The app folder is the root folder so you don't need to specify app.example.
        // The app/example script that is being imported is from the actual build folder!
        // Cmake will copy the python scripts after you have compiled the source code.
        std::cout << "Importing module..." << std::endl;
        auto example = py::module::import("app.gen");

        std::cout << "Initializing class..." << std::endl;
        const auto myExampleClass = example.attr("Generator");
        auto myExampleInstance = myExampleClass(); // Calls the constructor
        // Will print in the terminal window:
        // Example constructor with msg: Hello World

        const auto msg = myExampleInstance.attr("gen")(); // Calls the getMsg
        std::cout << "Got msg back on C++ side: " << msg.cast<std::string>() << std::endl;
#endif

    py::object main_python = py::module::import("__main__");
    py::dict scope = main_python.attr("__dict__");
    if (argc != 2)
    {
        std::cerr << "usage: $ ./mw python_generator.py" << std::endl;
        return EXIT_SUCCESS;
    }
    auto ev = py::eval_file(argv[1], scope);
    auto generatorClass = "";
    auto prefix_class = "<class '__main__.";
    for (auto item : scope)
    {
        py::print("key: {}, value={}"_s.format(item.first, item.second));
        if (!py::repr(item.second).compare(0, prefix_class.size(), prefix_class))
        {
            generatorClass = item.first.cast<std::string>();
        }
        if (py::repr(item.second)
    }
    auto generator = main_python.attr(generatorClass);
    auto myExampleInstance = generator();
    const auto msg = myExampleInstance.attr("gen")(); // Calls the getMsg
    std::cout << "Got msg back on C++ side: " << msg.cast<std::string>() << std::endl;

#ifdef THIS_IS_THE_VERY_LONG_DEFINITION
        stream.open(argv[1]);
        ANTLRInputStream input(stream);
#else
        ANTLRInputStream input(msg.cast<std::string>());
#endif

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
