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

#include "libjsonnet++.h"

namespace py = pybind11;
using namespace py::literals;

using namespace antlr4mw;
using namespace antlr4;

using namespace llvm;

using namespace jsonnet;

std::string readFile(const std::string& filename)
{
    std::ifstream in(filename);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}


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
        // py::module sys = py::module::import("sys");
        // py::print(sys.attr("path"));

        std::ifstream stream;

        py::object main_python = py::module::import("__main__");
        py::dict scope = main_python.attr("__dict__");
        if (argc != 2)
        {
            std::cerr << "usage: $ ./mw python_generator.py" << std::endl;
            return EXIT_SUCCESS;
        }
    
        auto ev = py::eval_file(argv[1], scope);
        auto generatorClass = "";
        auto prefix_class = std::string("<class '__main__.");
        for (auto item : scope)
        {
            py::print("key: {}, value={}"_s.format(item.first, item.second));
            if (!py::repr(item.second).cast<std::string>().compare(0, prefix_class.size(), prefix_class))
            {
                generatorClass = item.first.cast<std::string>().c_str();
            }
        }
        auto generator = main_python.attr(generatorClass);
        auto myExampleInstance = generator();
        const auto msg = myExampleInstance.attr("codegen")(); // Calls the getMsg
        const auto msg_jso = myExampleInstance.attr("codegen_jsonnet")(); // Calls the getMsg
        std::cout << "Got msg back on C++ side: " << msg.cast<std::string>() << std::endl;

#ifdef THIS_IS_THE_VERY_LONG_DEFINITION
        stream.open(argv[1]);
        ANTLRInputStream input(stream);
#else
        const std::string jsonnet_input = msg.cast<std::string>();
        Jsonnet jsonnet;
        jsonnet.init();
        std::string output;
        jsonnet.evaluateSnippet("snippet", jsonnet_input, &output);
        std::cout << "jsonnet errors:" << jsonnet.lastError() << std::endl;

        ANTLRInputStream input(output);
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
