
// Generated from JSON.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "JSONParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by JSONParser.
 */
class  JSONVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by JSONParser.
   */
    virtual antlrcpp::Any visitJson(JSONParser::JsonContext *context) = 0;

    virtual antlrcpp::Any visitObj(JSONParser::ObjContext *context) = 0;

    virtual antlrcpp::Any visitPair(JSONParser::PairContext *context) = 0;

    virtual antlrcpp::Any visitArray(JSONParser::ArrayContext *context) = 0;

    virtual antlrcpp::Any visitValue(JSONParser::ValueContext *context) = 0;


};

