#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

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
  virtual void generate() {
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
  void generate() {
    left->generate();
    std::cout << "TODO - Generating STLFormulaNOT" << std::endl;
  }
};

class STLFormulaAND : public STLFormula {
public:
  STLFormulaAND(STLFormula *f1, STLFormula *f2) {
    std::cout << "--|--|--|--|--|--) Found  STLFormulaAND" << std::endl;
    left = f1;
    right = f2;
  }
  void generate() {
    left->generate();
    right->generate();
    std::cout << "TODO - Generating STLFormulaAND" << std::endl;
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
  void generate() {
    left->generate();
    if (_tSet)
      std::cout << "TODO - Generating Timed STLAlways" << std::endl;
    else
      std::cout << "TODO - Generating STLAlways" << std::endl;
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
  void generate() {
    left->generate();
    if (_tSet)
      std::cout << "TODO - Generating Timed STLEventually" << std::endl;
    else
      std::cout << "TODO - Generating STLEventually" << std::endl;
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
  void generate() {
    left->generate();
    right->generate();
    if (_tSet)
      std::cout << "TODO - Generating Timed STLFormulaUNTIL" << std::endl;
    else
      std::cout << "TODO - Generating STLFormulaUNTIL" << std::endl;
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
  void generate() {
    if (_op == CONST || _op == PORT) {
      // TODO
    } else {
      left->generate();
      right->generate();
    }

    std::cout << "TODO - Generating Expression" << std::endl;
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
  void generate() {
    left->generate();
    right->generate();
    std::cout << "TODO - Generating BooleanOperation" << std::endl;
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
  void generate() {
    left->generate();
    right->generate();
    std::cout << "TODO - Generating ComparisonExpression" << std::endl;
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
  void generate() {
    left->generate();
    right->generate();
    std::cout << "TODO - Generating isStepFunction" << std::endl;
  }
};

class BooleanValue : public BooleanExpression {
  bool _v;
public:
  BooleanValue(bool v) : _v(v) {
    std::cout << "--|--|--|--|--|--|--) Found boolean value [";
    if (v)
      std::cout << "TRUE";
    else
      std::cout << "FALSE";
    std::cout << "]" << std::endl;
  }
  void generate() {
    std::cout << "TODO - Generating BooleanValue [";
    if (_v)
      std::cout << "TRUE";
    else
      std::cout << "FALSE";
    std::cout << "]" << std::endl;
  }
};

void foundUntil();
void foundConstantBlock(std::string v);
void foundPortBlock(std::string v);
void foundMainTimeRange(TimeInterval t);
void foundComparisonExpression(LogicalOperator op, std::string v1, std::string v2 = "");

#endif // UTILITY_H

