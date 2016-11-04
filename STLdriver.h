#ifndef STL_DRIVER_H
#define STL_DRIVER_H

#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <list>

#include <STLparser.hh>
#include <utility.h>

// Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::STLparser::symbol_type yylex (STLdriver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

class STLdriver
{
    std::ofstream outputFile;

    std::map<std::string, std::string> variablesValues;

    bool REF_input;
    bool SIG_input;

    DriverStatus status;

  public:
    STLdriver (char *filename);
    virtual ~STLdriver();

    bool variableExists(std::string v);
    void setVariable(std::string name, std::string value);
    std::string getVariable(std::string name);

    void setStatus(DriverStatus s);

    void printConstantValues();
    void printAssertions();

    // Run the parser on file F.
    // Return 0 on success.
    int parse(const std::string& f);

    // Appends to the file and stdout
    // the given string
    void append(const std::string &s);
    void appendln(const std::string &s);

    // Handling the scanner.
    void scan_begin();
    void scan_end();
    bool trace_scanning;

    // The name of the file being parsed.
    // Used later to pass the file name to the location tracker.
    std::string file;

    // Whether parser traces should be generated.
    bool trace_parsing;

    // Error handling.
    void error(const yy::location& l, const std::string& m);
    void error(const std::string& m);

    std::string createAssertionBody(LogicalOperation *l, std::string parent = "");
    void createIsStepBlock(std::string v1, std::string v2);
    void createDiffBlock(std::string v);
    void createExpressionBlock();
    void createSignalBlock();
    std::string createExpression(MathOperation * e);
    void createConstantBlock(std::string v);
    void createReferenceBlock();
    ComparisonOperation * createComparisonBlock(ComparisonOperator op,
                                                MathOperation *a,
                                                MathOperation *b = nullptr);
    LogicalOperation * createLogicalBlock(LogicalOperator op,
                                          LogicalOperation *a = nullptr,
                                          LogicalOperation *b = nullptr);
    MathOperation * createMathBlock(MathOperator op,
                                    MathOperation *a = nullptr,
                                    MathOperation *b = nullptr);

};

void foundConstantBlock(std::string v);
void foundMainTimeRange(TimeInterval t);
void foundComparisonExpression(LogicalOperator op, std::string v1, std::string v2 = "");

#endif
