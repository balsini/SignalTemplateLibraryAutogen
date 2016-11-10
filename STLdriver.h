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

typedef std::map<std::string, unsigned int> portMapping;
typedef std::tuple<std::string, portMapping> blockPortMapping;

class STLdriver
{
  std::ofstream testBlockAppendFile;
  std::ofstream testBlockRoutingAppendFile;
  std::ifstream systemPortsFile;

  std::string path;

  std::map<std::string, std::string> variablesValues;
  std::list<std::string> ports;

  std::list<std::tuple<TemporalOperator, TimeInterval, LogicalOperation *> > STLFormulas;

  bool REF_input;
  bool SIG_input;

  DriverStatus status;

public:
  STLdriver(const std::string &path);
  virtual ~STLdriver();

  bool variableExists(std::string v);
  bool portExists(std::string v);
  void setVariable(std::string name, std::string value);
  std::string getVariable(std::string name);

  void setStatus(DriverStatus s);

  void printConstantValues();

  // Run the parser on file F.
  // Return 0 on success.
  int parse(const std::string& f);
  void parsePorts();

  // Appends to the AUTOGEN file the given string
  void fileAppend(const std::string &s, std::ofstream &f);
  void testBlockAppendLn(const std::string fileName, const std::string &functionName, int lineNumber, const std::string &s);
  void testBlockRoutingAppendLn(const std::string fileName, const std::string &functionName, int lineNumber, const std::string &s);

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

  void addSTLFormula(const TemporalOperator &tOp,
                     const TimeInterval &tIn,
                     LogicalOperation *l);
  void createSTLFormulas();

  blockPortMapping createSTLFormulaBody(LogicalOperation *l,
                                        std::string parent = "",
                                        unsigned int y = 0,
                                        std::string BLOCK_ROOT = "");
  void createSTLFormulaTimeInterval(const TimeInterval &time,
                                    std::string parent,
                                    std::string BLOCK_ROOT);
  void createSTLFormulaTemporalOperator(TemporalOperator op,
                                        std::string parent,
                                        std::string BLOCK_ROOT);
#if 0
  blockPortMapping createSTLFormula(const TemporalOperator &tOp,
                                    const TimeInterval &tIn,
                                    LogicalOperation *l);
#endif
  void createIsStepBlock(std::string v1, std::string v2);
  void createDiffBlock(std::string v);
  blockPortMapping createExpression(MathOperation * e,
                                    std::string parent = "",
                                    unsigned int y = 0,
                                    std::string BLOCK_ROOT = "");
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
