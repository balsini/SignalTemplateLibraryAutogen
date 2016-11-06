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
  std::ofstream testBlockAppendFile;
  std::ofstream testBlockRoutingAppendFile;

  std::map<std::string, std::string> variablesValues;

  bool REF_input;
  bool SIG_input;

  DriverStatus status;

public:
  STLdriver(const std::string &path);
  virtual ~STLdriver();

  bool variableExists(std::string v);
  void setVariable(std::string name, std::string value);
  std::string getVariable(std::string name);

  void setStatus(DriverStatus s);

  void printConstantValues();

  // Run the parser on file F.
  // Return 0 on success.
  int parse(const std::string& f);

  // Appends to the AUTOGEN file the given string
  void fileAppend(const std::string &s, std::ofstream &f);
  void testBlockAppendLn(const std::string fileName, int lineNumber, const std::string &s);
  void testBlockRoutingAppendLn(const std::string fileName, int lineNumber, const std::string &s);

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

  std::tuple<std::string, unsigned int> createAssertionBody(LogicalOperation *l,
                                                            std::string parent = "",
                                                            unsigned int x = 0,
                                                            unsigned int y = 0);
  void connectAssertions(std::list<std::tuple<std::string, unsigned int> > l);
  void createIsStepBlock(std::string v1, std::string v2);
  void createDiffBlock(std::string v);
  void createSignalBlock();
  std::tuple<std::string, unsigned int> createExpression(MathOperation * e,
                                                         std::string parent = "",
                                                         unsigned int x = 0,
                                                         unsigned int y = 0);
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

#endif
