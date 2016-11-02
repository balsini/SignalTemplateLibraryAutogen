#ifndef STL_DRIVER_H
#define STL_DRIVER_H

#include <string>
#include <fstream>
#include <map>

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

    std::map<std::string, bool> variablesSet;

public:
    STLdriver (char *filename);
    virtual ~STLdriver();

    bool variableExists(std::string v) {
      return (variablesSet.find(v) != variablesSet.end());
    }
    void setVariable(std::string v) {
      variablesSet[v] = true;
    }

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

    bool REF_input;
    bool SIG_input;

    void createMainTemporalOperator();
    void createIsStepBlock(std::string v1, std::string v2);
    void createDiffBlock(std::string v);
    void createFunctionBlock();
};
#endif
