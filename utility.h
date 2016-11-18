#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <map>

#define SRC_INFO_TEMP __FILE__, __func__, __LINE__
#define SRC_INFO (std::make_tuple(__FILE__, __func__, __LINE__))

typedef std::map<std::string, unsigned int> portMapping;
typedef std::tuple<std::string, portMapping> blockPortMapping;
typedef std::tuple<std::string, std::string, int> srcInfo;

class STLdriver;

class TimeInterval
{
public:
  TimeInterval(std::string li, std::string l, std::string r, std::string ri) :
    start(l),
    end(r)
  {
    if ( (li != "[" && li != "(") || (ri != "]" && ri != ")") )
      throw "Wrong interval delimiter";

    startClosed = (li == "[");
    endClosed = (ri == "]");
  }

  TimeInterval() {}

  std::string start;
  std::string end;
  bool startClosed;
  bool endClosed;
};

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj);

enum ComparisonOperator { GEQ, LEQ, GREATER, SMALLER, EQUAL, NEQUAL };
enum LogicalOperator { AND, OR, NOT, COMPARISON, ISSTEP };
enum MathOperator { SUM, SUB, MUL, DIV, ABS, DIFF, CONST, PORT };
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
    left = nullptr;

    delete right;
    right = nullptr;
  }
  virtual blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos) {
    std::cout << "ERROR: code generation not yet implemented in the child node" << std::endl;
  }
};

class STLFormula : public TreeNode {
};

class BoolExpr : public STLFormula {
};

class STLFormulaNOT : public STLFormula {
public:
  STLFormulaNOT(STLFormula *f);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class STLFormulaAND : public STLFormula {
public:
  STLFormulaAND(STLFormula *f1, STLFormula *f2);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class STLAlways : public STLFormula {
  TimeInterval _t;
  bool _tSet;
public:
  STLAlways(const TimeInterval &t, STLFormula *f);
  STLAlways(STLFormula *f);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class STLEventually : public STLFormula {
  TimeInterval _t;
  bool _tSet;
public:
  STLEventually(const TimeInterval &t, STLFormula *f);
  STLEventually(STLFormula *f);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class STLFormulaUNTIL : public STLFormula {
  TimeInterval _t;
  bool _tSet;
public:
  STLFormulaUNTIL(const TimeInterval &t, STLFormula *f1, STLFormula *f2);
  STLFormulaUNTIL(STLFormula *f1, STLFormula *f2);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class Expression : public TreeNode {
  std::string _value;
protected:
  MathOperator _op;
public:
  Expression() {}
  Expression(MathOperator op, Expression *e1, Expression *e2);
  Expression(MathOperator op, std::string value);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class ExpressionFunction : public Expression {
public:
  ExpressionFunction(MathOperator op, Expression *e);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

/********* Boolean Expressions *********/

class BooleanExpression : public STLFormula {
};

class BooleanOperation : public BooleanExpression {
  LogicalOperator _op;
public:
  BooleanOperation(LogicalOperator op, BooleanExpression *b1, BooleanExpression *b2);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class ComparisonExpression : public BooleanExpression {
  ComparisonOperator _op;
public:
  ComparisonExpression(ComparisonOperator op, Expression *e1, Expression *e2);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class BooleanFunction : public BooleanExpression {};

class isStepFunction : public BooleanFunction {
public:
  isStepFunction(Expression *e1, Expression *e2);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

class BooleanValue : public BooleanExpression {
  bool _v;
public:
  BooleanValue(bool v);
  blockPortMapping generate(STLdriver *d, const std::string &parent, int vpos);
};

void updateRequiredPorts(STLdriver *d,
                         const std::string &name,
                         portMapping &bpm,
                         const blockPortMapping &A,
                         unsigned int &portId);

#endif // UTILITY_H

