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
  std::ifstream systemPortsFile;

  std::string path;

  std::map<std::string, std::string> variablesValues;
  std::list<std::string> ports;

  std::list<TreeNode *> nodes;

  bool REF_input;
  bool SIG_input;

  void cleanTree();

public:
  STLdriver(const std::string &path);
  virtual ~STLdriver();

  bool variableExists(std::string v);
  bool portExists(std::string v);
  void setVariable(std::string name, std::string value);
  std::string getVariable(std::string name);

  void printConstantValues();

  // Run the parser on file F.
  // Return 0 on success.
  int parse(const std::string& f);
  void parsePorts();

  // Appends to the AUTOGEN file the given string
  void fileAppend(const std::string &s, std::ofstream &f);
  void testBlockAppendLn(const std::string &fileName,
                         const std::string &functionName,
                         int lineNumber,
                         const std::string &s);

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

  void addSTLFormula(TreeNode *f, const std::string &name = "");
  blockPortMapping createSTLFormulas();
  void linkSTLFormulas(const blockPortMapping &bpm);

  std::string createTimeInterval(const TimeInterval &time,
                                 const std::string &parent,
                                 bool withDelay = false);

  std::string createSTLFormulaTemporalOperator(TemporalOperator op,
                                        std::string parent);

  std::string createSTLFormulaUntil(const std::string &parent);
  void createDiffBlock(std::string v);

  std::string createIsStepBlock(srcInfo code,
                                const std::string &parent,
                                unsigned int x1,
                                unsigned int x2,
                                unsigned int y1,
                                unsigned int y2);
  std::string createEmptyBlock(srcInfo code,
                               const std::string &parent,
                               unsigned int x1,
                               unsigned int x2,
                               unsigned int y1,
                               unsigned int y2);
  void createLine(srcInfo code,
                  const std::string &src,
                  const std::string &dst,
                  const std::string &root,
                  unsigned int src_p = 1,
                  unsigned int dst_p = 1);
};

#endif
