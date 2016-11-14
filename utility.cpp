#include "utility.h"

#include "STLdriver.h"

const unsigned int position_X_IN[2] = {20, 40};
const unsigned int position_X_EXP[2] = {80, 150};
const unsigned int position_X_OP[2] = {190, 210};
const unsigned int position_X_OUT[2] = {240, 260};

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj)
{
  os << obj.start << obj.end;
  return os;
}

BooleanValue::BooleanValue(bool v) :
  _v(v)
{
  std::cout << "--|--|--|--|--|--|--) Found boolean value [";
  if (v)
    std::cout << "TRUE";
  else
    std::cout << "FALSE";
  std::cout << "]" << std::endl;
}

blockPortMapping BooleanValue::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cerr << "TODO - Generating BooleanValue [";
  if (_v)
    std::cerr << "TRUE";
  else
    std::cerr << "FALSE";
  std::cerr << "]" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_B = add_block('simulink/Sources/Constant', [" + name + " '/TRUE']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'OutDataTypeStr', 'boolean');");

  if (_v) {
    d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'Value', '1');");
  } else {
    d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'Value', '0');");
  }

  d->createLine(SRC_INFO, name + "_B", name + "_OUT", name);

  return bpm;
}

isStepFunction::isStepFunction(Expression *e1, Expression *e2)
{
  std::cout << "--|--|--|--|--|--|--) Found isStep" << std::endl;
  left = e1;
  right = e2;
}

blockPortMapping isStepFunction::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  right->generate(d, parent, 1);
  std::cerr << "TODO - Generating isStepFunction" << std::endl;

  return bpm;
}

ComparisonExpression::ComparisonExpression(ComparisonOperator op, Expression *e1, Expression *e2) :
  _op(op)
{
  std::cout << "--|--|--|--|--) Found comparison expression" << std::endl;
  left = e1;
  right = e2;
}

blockPortMapping ComparisonExpression::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  right->generate(d, parent, 1);
  std::cerr << "TODO - Generating ComparisonExpression" << std::endl;

  return bpm;
}

BooleanOperation::BooleanOperation(LogicalOperator op, BooleanExpression *b1, BooleanExpression *b2)
{
  std::cout << "--|--|--|--) Found boolean operation" << std::endl;
  left = b1;
  right = b2;
}

blockPortMapping BooleanOperation::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  right->generate(d, parent, 1);
  std::cerr << "TODO - Generating BooleanOperation" << std::endl;

  return bpm;
}

Expression::Expression(MathOperator op, Expression *e1, Expression *e2) :
  _op(op)
{
  std::cout << "--|--|--|--|--|--) Found mathematical expression" << std::endl;
  left = e1;
  right = e2;
}

Expression::Expression(MathOperator op, std::string value) :
  _op(op),
  _value(value)
{
  std::cout << "--|--|--|--|--|--|--) Found mathematical value ";
  if (op == CONST)
    std::cout << "[CONST -> ";
  else if (op == PORT)
    std::cout << "[PORT -> ";
  std::cout << value << "]" << std::endl;
}

blockPortMapping Expression::generate(STLdriver *d, const std::string &parent, int vpos) {
  blockPortMapping bpm;

  if (_op == CONST || _op == PORT) {
    // TODO
  } else {
    left->generate(d, parent, 0);
    right->generate(d, parent, 1);
  }

  std::cerr << "TODO - Generating Expression" << std::endl;

  return bpm;
}

STLFormulaUNTIL::STLFormulaUNTIL(const TimeInterval &t, STLFormula *f1, STLFormula *f2) :
  _t(t),
  _tSet(true)
{
  std::cout << "--|--|--|--|--|--) Found Timed STLFormulaUNTIL" << std::endl;
  left = f1;
  right = f2;
}

STLFormulaUNTIL::STLFormulaUNTIL(STLFormula *f1, STLFormula *f2) :
  _tSet(false)
{
  std::cout << "--|--|--|--|--|--|--) Found STLFormulaUNTIL " << std::endl;
  left = f1;
  right = f2;
}

blockPortMapping STLFormulaUNTIL::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  right->generate(d, parent, 1);
  if (_tSet)
    std::cerr << "TODO - Generating Timed STLFormulaUNTIL" << std::endl;
  else
    std::cerr << "TODO - Generating STLFormulaUNTIL" << std::endl;

  return bpm;
}

STLEventually::STLEventually(const TimeInterval &t, STLFormula *f) :
  _t(t),
  _tSet(true)
{
  std::cout << "--|--|--|--|--|--) Found Timed STLEventually" << std::endl;
  left = f;
}

STLEventually::STLEventually(STLFormula *f) :
  _tSet(false)
{
  std::cout << "--|--|--|--|--|--|--) Found STLEventually ";
  left = f;
}

blockPortMapping STLEventually::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  if (_tSet)
    std::cerr << "TODO - Generating Timed STLEventually" << std::endl;
  else
    std::cerr << "TODO - Generating STLEventually" << std::endl;

  return bpm;
}

STLAlways::STLAlways(const TimeInterval &t, STLFormula *f) :
  _t(t),
  _tSet(true)
{
  std::cout << "--|--|--|--|--|--) Found Timed STLAlways" << std::endl;
  left = f;
}

STLAlways::STLAlways(STLFormula *f) :
  _tSet(false)
{
  std::cout << "--|--|--|--|--|--|--) Found STLAlways ";
  left = f;
}

blockPortMapping STLAlways::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  if (_tSet)
    std::cerr << "TODO - Generating Timed STLAlways" << std::endl;
  else
    std::cerr << "TODO - Generating STLAlways" << std::endl;

  return bpm;
}

STLFormulaAND::STLFormulaAND(STLFormula *f1, STLFormula *f2)
{
  std::cout << "--|--|--|--|--|--) Found  STLFormulaAND" << std::endl;
  left = f1;
  right = f2;
}

blockPortMapping STLFormulaAND::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  right->generate(d, parent, 1);
  std::cerr << "TODO - Generating STLFormulaAND" << std::endl;

  return bpm;
}

STLFormulaNOT::STLFormulaNOT(STLFormula *f)
{
  std::cout << "--|--|--|--|--|--) Found  STLFormulaNOT" << std::endl;
  left = f;
}

blockPortMapping STLFormulaNOT::generate(STLdriver *d, const std::string &parent, int vpos)
{
  blockPortMapping bpm;

  left->generate(d, parent, 0);
  std::cerr << "TODO - Generating STLFormulaNOT" << std::endl;

  return bpm;
}
