#include <iostream>

#include <antlr4-runtime.h>
#include "JSONLexer.h"
#include "JSONParser.h"
#include "JSONBaseListener.h"

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
    std::ifstream stream;
    stream.open(argv[1]);
    ANTLRInputStream input(stream);
    JSONLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();
		for (auto token : tokens.getTokens()) {
			std::cout << token->toString() << std::endl;
		}
    JSONParser parser(&tokens);

	tree::ParseTree *tree = parser.json();
	std::cout << tree->toStringTree(&parser) << std::endl;
	
    return 0;
}