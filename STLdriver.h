#ifndef STL_DRIVER_H
#define STL_DRIVER_H

#include <string>
#include <fstream>

#include <STLparser.hh>

// Tell Flex the lexer's prototype ...
# define YY_DECL \
    yy::STLparser::symbol_type yylex (STLdriver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

class STLdriver
{
    std::ofstream outputFile;

public:
    STLdriver (char *filename);
    virtual ~STLdriver();

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
};
#endif
