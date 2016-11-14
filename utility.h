#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <map>

typedef std::map<std::string, unsigned int> portMapping;
typedef std::tuple<std::string, portMapping> blockPortMapping;
typedef std::tuple<std::string, std::string, int> srcInfo;

class TimeInterval
{
public:
  TimeInterval(std::string l, std::string li, std::string r, std::string ri) :
    start(l),
    end(r)
  {
    if (li == "(")
      startClosed = "off";
    else
      startClosed = "on";

    if (ri == ")")
      endClosed = "off";
    else
      endClosed = "on";
  }

  TimeInterval() {}

  std::string start;
  std::string end;
  std::string startClosed;
  std::string endClosed;
};

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj);

enum DriverStatus { HEADER, BODY, FOOTER };
enum ComparisonOperator { GEQ, LEQ, GREATER, SMALLER, EQUAL, NEQUAL };
enum LogicalOperator { AND, OR, NOT, COMPARISON, ISSTEP };
enum MathOperator { SUM, SUB, MUL, DIV, CONST, PORT };
//enum TemporalOperator { ALWAYS, EVENTUALLY };

struct MathOperation {
  MathOperator op;
  MathOperation *a;
  MathOperation *b;
  std::string value;
};

struct ComparisonOperation {
  ComparisonOperator op;
  MathOperation *a;
  MathOperation *b;
};

struct LogicalOperation {
  LogicalOperator op;
  LogicalOperation *a;
  LogicalOperation *b;
  ComparisonOperation *value;

  // For boolean functions
  MathOperation *arg1;
  MathOperation *arg2;
};

class TreeNode {

protected:
  TreeNode *left;
  TreeNode *right;

public:
  TreeNode() : left(nullptr), right(nullptr) {}
  ~TreeNode() {
    delete left;
    left = nullptr;

    delete right;
    right = nullptr;
  }
  virtual blockPortMapping generate() {
    std::cout << "ERROR: code generation not yet implemented in the child node" << std::endl;
  }
};

class STLFormula : public TreeNode {
};

class BoolExpr : public STLFormula {
};

class STLFormulaNOT : public STLFormula {
public:
  STLFormulaNOT(STLFormula *f) {
    std::cout << "--|--|--|--|--|--) Found  STLFormulaNOT" << std::endl;
    left = f;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    std::cout << "TODO - Generating STLFormulaNOT" << std::endl;

    return bpm;
  }
};

class STLFormulaAND : public STLFormula {
public:
  STLFormulaAND(STLFormula *f1, STLFormula *f2) {
    std::cout << "--|--|--|--|--|--) Found  STLFormulaAND" << std::endl;
    left = f1;
    right = f2;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    right->generate();
    std::cout << "TODO - Generating STLFormulaAND" << std::endl;

    return bpm;
  }
};

class STLAlways : public STLFormula {
  TimeInterval _t;
  bool _tSet;
public:
  STLAlways(const TimeInterval &t, STLFormula *f)
    : _t(t), _tSet(true) {
    std::cout << "--|--|--|--|--|--) Found Timed STLAlways" << std::endl;
    left = f;
  }
  STLAlways(STLFormula *f) : _tSet(false){
    std::cout << "--|--|--|--|--|--|--) Found STLAlways ";
    left = f;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    if (_tSet)
      std::cout << "TODO - Generating Timed STLAlways" << std::endl;
    else
      std::cout << "TODO - Generating STLAlways" << std::endl;

    return bpm;
  }
};

class STLEventually : public STLFormula {
  TimeInterval _t;
  bool _tSet;
public:
  STLEventually(const TimeInterval &t, STLFormula *f)
    : _t(t), _tSet(true) {
    std::cout << "--|--|--|--|--|--) Found Timed STLEventually" << std::endl;
    left = f;
  }
  STLEventually(STLFormula *f) : _tSet(false){
    std::cout << "--|--|--|--|--|--|--) Found STLEventually ";
    left = f;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    if (_tSet)
      std::cout << "TODO - Generating Timed STLEventually" << std::endl;
    else
      std::cout << "TODO - Generating STLEventually" << std::endl;

    return bpm;
  }
};

class STLFormulaUNTIL : public STLFormula {
  TimeInterval _t;
  bool _tSet;
public:
  STLFormulaUNTIL(const TimeInterval &t, STLFormula *f1, STLFormula *f2)
    : _t(t), _tSet(true) {
    std::cout << "--|--|--|--|--|--) Found Timed STLFormulaUNTIL" << std::endl;
    left = f1;
    right = f2;
  }
  STLFormulaUNTIL(STLFormula *f1, STLFormula *f2) : _tSet(false){
    std::cout << "--|--|--|--|--|--|--) Found STLFormulaUNTIL " << std::endl;
    left = f1;
    right = f2;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    right->generate();
    if (_tSet)
      std::cout << "TODO - Generating Timed STLFormulaUNTIL" << std::endl;
    else
      std::cout << "TODO - Generating STLFormulaUNTIL" << std::endl;

    return bpm;
  }
};

class Expression : public TreeNode {
  MathOperator _op;
  std::string _value;
public:
  Expression(MathOperator op, Expression *e1, Expression *e2)
    : _op(op) {
    std::cout << "--|--|--|--|--|--) Found mathematical expression" << std::endl;
    left = e1;
    right = e2;
  }
  Expression(MathOperator op, std::string value) : _op(op), _value(value) {
    std::cout << "--|--|--|--|--|--|--) Found mathematical value ";
    if (op == CONST)
      std::cout << "[CONST -> ";
    else if (op == PORT)
      std::cout << "[PORT -> ";
    std::cout << value << "]" << std::endl;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    if (_op == CONST || _op == PORT) {
      // TODO
    } else {
      left->generate();
      right->generate();
    }

    std::cout << "TODO - Generating Expression" << std::endl;

    return bpm;
  }
};

/********* Boolean Expressions *********/

class BooleanExpression : public STLFormula {
};

class BooleanOperation : public BooleanExpression {
public:
  BooleanOperation(LogicalOperator op, BooleanExpression *b1, BooleanExpression *b2) {
    std::cout << "--|--|--|--) Found boolean operation" << std::endl;
    left = b1;
    right = b2;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    right->generate();
    std::cout << "TODO - Generating BooleanOperation" << std::endl;

    return bpm;
  }
};

class ComparisonExpression : public BooleanExpression {
  ComparisonOperator _op;
public:
  ComparisonExpression(ComparisonOperator op, Expression *e1, Expression *e2) : _op(op) {
    std::cout << "--|--|--|--|--) Found comparison expression" << std::endl;
    left = e1;
    right = e2;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    right->generate();
    std::cout << "TODO - Generating ComparisonExpression" << std::endl;

    return bpm;
  }
};

class BooleanFunction : public BooleanExpression {};

class isStepFunction : public BooleanFunction {
public:
  isStepFunction(Expression *e1, Expression *e2) {
    std::cout << "--|--|--|--|--|--|--) Found isStep" << std::endl;
    left = e1;
    right = e2;
  }
  blockPortMapping generate() {
    blockPortMapping bpm;

    left->generate();
    right->generate();
    std::cout << "TODO - Generating isStepFunction" << std::endl;

    return bpm;
  }
};

class BooleanValue : public BooleanExpression {
  bool _v;
public:
  BooleanValue(bool v);
  blockPortMapping generate();
};

#endif // UTILITY_H

