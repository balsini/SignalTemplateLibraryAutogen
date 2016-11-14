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
enum TemporalOperator { ALWAYS, EVENTUALLY };

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
    delete right;
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
};

class STLFormulaAND : public STLFormula {
};

class STLAlways : public STLFormula {
  TemporalOperator tOp;
  TimeInterval t;
};

class STLEventually : public STLFormula {
  TemporalOperator tOp;
  TimeInterval t;
};

class STLFormulaUNTIL : public STLFormula {
};

class Expression : public TreeNode {
  MathOperator _op;
  std::string _value;
public:
  Expression(MathOperator op, Expression *e1 = nullptr, Expression *e2 = nullptr)
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
    std::cout << "TODO - Generating Expression" << std::endl;

    if (_op == CONST || _op == PORT) {
      // TODO
    } else {
      left->generate();
      right->generate();
    }
  }
};

/********* Boolean Expressions *********/

class BooleanExpression : public STLFormula {
};

class BooleanOperation : public BooleanExpression {
public:
  BooleanOperation(LogicalOperator op, BooleanExpression *b1 = nullptr, BooleanExpression *b2 = nullptr) {
    std::cout << "--|--|--|--) Found boolean operation" << std::endl;
    left = b1;
    right = b2;
  }
  void generate() {
    std::cout << "TODO - Generating BooleanOperation" << std::endl;
    left->generate();
    right->generate();
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
    std::cout << "TODO - Generating ComparisonExpression" << std::endl;
    left->generate();
    right->generate();
  }
};

class BooleanFunction : public BooleanExpression {
public:
  BooleanFunction() {}
  BooleanFunction(const std::string &name) {
    std::cout << "--|--|--|--|--|--|--) Found boolean function [";
    std::cout << name;
    std::cout << "]" << std::endl;
  }
};

class isStepFunction : public BooleanFunction {
public:
  isStepFunction(Expression *e1, Expression *e2) {
    std::cout << "--|--|--|--|--|--|--) Found isStep" << std::endl;
  }
  void generate() {
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

