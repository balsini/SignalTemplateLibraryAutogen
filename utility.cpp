#include "utility.h"

#include "STLdriver.h"

const unsigned int position_X_IN[2] = {20, 40};
const unsigned int position_X_EXP[2] = {80, 150};
const unsigned int position_X_OP[2] = {190, 210};
const unsigned int position_X_OUT[2] = {240, 260};

const unsigned int portOffset = 40;

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj)
{
  os << obj.start << obj.end;
  return os;
}

void updateRequiredPorts(STLdriver *d, const std::string &name, portMapping &bpm, const blockPortMapping &A, unsigned int &portId)
{
  for (auto pm : std::get<1>(A)) {
    portMapping::iterator it = bpm.find(pm.first);
    if (it == bpm.end()) {
      // Port needs to be created

      d->testBlockAppendLn(SRC_INFO_TEMP, name + pm.first + " = add_block('simulink/Sources/In1', [" + name + " '/" + pm.first + "']);");
      d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      bpm[pm.first] = portId;
      portId++;
    }

    // And line generated
    d->createLine(SRC_INFO, name + pm.first, std::get<0>(A), name, 1, pm.second);
  }
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
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating BooleanValue [";
  if (_v)
    std::cout << "TRUE";
  else
    std::cout << "FALSE";
  std::cout << "]" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  // Create boolean value
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_B = add_block('simulink/Sources/Constant', [" + name + " '/B']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'OutDataTypeStr', 'boolean');");
  if (_v) {
    d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'Value', '1');");
  } else {
    d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_B, 'Value', '0');");
  }

  // Link blocks
  d->createLine(SRC_INFO, name + "_B", name + "_OUT", name);

  return std::make_tuple(name, bpm);
}

isStepFunction::isStepFunction(Expression *e1, Expression *e2)
{
  std::cout << "--|--|--|--|--|--|--) Found isStep" << std::endl;
  left = e1;
  right = e2;
}

blockPortMapping isStepFunction::generate(STLdriver *d, const std::string &parent, int vpos)
{
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating isStepFunction" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  blockPortMapping A = left->generate(d, name, 0);
  blockPortMapping B = right->generate(d, name, 1);

  std::string is = d->createIsStepBlock(SRC_INFO, name, position_X_OP[0], position_X_OP[1], 20, 80);

  d->createLine(SRC_INFO, is, name + "_OUT", name);
  d->createLine(SRC_INFO, std::get<0>(A), is, name);
  d->createLine(SRC_INFO, std::get<0>(B), is, name, 1, 2);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);
  updateRequiredPorts(d, name, bpm, B, portId);

  return std::make_tuple(name, bpm);
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
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating ComparisonExpression" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  std::string relOp;
  switch (_op) {
    case GEQ:     relOp = ">="; break;
    case LEQ:     relOp = "<="; break;
    case GREATER: relOp = ">";  break;
    case SMALLER: relOp = "<";  break;
    case EQUAL:   relOp = "=="; break;
    case NEQUAL:  relOp = "~="; break;
    default: break;
  }

  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + name + " '/OP']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'Operator', '" + relOp + "');");

  blockPortMapping A = left->generate(d, name, 0);
  blockPortMapping B = right->generate(d, name, 1);

  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  d->createLine(SRC_INFO, name + "_OP", name + "_OUT", name);
  d->createLine(SRC_INFO, std::get<0>(A), name + "_OP", name);
  d->createLine(SRC_INFO, std::get<0>(B), name + "_OP", name, 1, 2);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);
  updateRequiredPorts(d, name, bpm, B, portId);

  return std::make_tuple(name, bpm);
}

BooleanOperation::BooleanOperation(LogicalOperator op, BooleanExpression *b1, BooleanExpression *b2) :
  _op(op)
{
  std::cout << "--|--|--|--) Found boolean operation" << std::endl;
  left = b1;
  right = b2;
}

blockPortMapping BooleanOperation::generate(STLdriver *d, const std::string &parent, int vpos)
{
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating BooleanOperation" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  std::string logOp;
  if (_op == AND)
    logOp = "AND";
  else if (_op == OR)
    logOp = "OR";

  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/OP']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'Operator', '" + logOp + "');");

  blockPortMapping A = left->generate(d, name, 0);
  blockPortMapping B = right->generate(d, name, 1);

  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  d->createLine(SRC_INFO, name + "_OP", name + "_OUT", name);
  d->createLine(SRC_INFO, std::get<0>(A), name + "_OP", name);
  d->createLine(SRC_INFO, std::get<0>(B), name + "_OP", name, 1, 2);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);
  updateRequiredPorts(d, name, bpm, B, portId);

  return std::make_tuple(name, bpm);
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

/**
 * @brief Creates a block containing the expression.
 * This is a recursive function that explores the expression tree,
 * where the leaves nodes are constant values or ports and the other
 * nodes are operation performed on child nodes.
 *
 * In order, this function:
 * 1) Creates an empty container and its exit port. This container
 * represents the actual expression.
 * 2) In case of leaf node if fills the container with a constant value
 * or input port, otherwise it creates the mathematical operator and performs
 * a recursive call in order to create the expressions subtrees.
 */
blockPortMapping Expression::generate(STLdriver *d, const std::string &parent, int vpos)
{
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating Expression" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  if (_op == CONST || _op == PORT) {

    if (_op == CONST) {
      d->testBlockAppendLn(SRC_INFO_TEMP, name + "_IN = add_block('simulink/Sources/Constant', [" + name + " '/K']);");
      d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_IN, 'Value', '" + _value + "');");
    } else if (_op == PORT) {
      d->testBlockAppendLn(SRC_INFO_TEMP, name + "_IN = add_block('simulink/Sources/In1', [" + name + " '/" + _value + "']);");
      bpm[_value] = 1;
    }

    d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_IN,'position', [" + std::to_string(position_X_IN[0]) + ", 20, " + std::to_string(position_X_IN[1]) + ", 40]);");

    d->createLine(SRC_INFO, name + "_IN", name + "_OUT", name);

  } else {

    std::string matOp;
    switch (_op) {
      case SUM:
        matOp = "Add";
        break;
      case SUB:
        matOp = "Subtract";
        break;
      case MUL:
        matOp = "Product";
        break;
      case DIV:
        matOp = "Divide";
        break;
      default: break;
    }

    d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Math Operations/" + matOp + "', [" + name + " '/OP']);");
    d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'position',[" + std::to_string(position_X_OP[0]) + ", 20, " + std::to_string(position_X_OP[1]) + ", 40]);");

    ///////////////////////////
    /// Generate expressions //
    ///////////////////////////

    blockPortMapping A = left->generate(d, name, 0);
    blockPortMapping B = right->generate(d, name, 1);

    /////////////////////////////////////
    /// Create output port connections //
    /////////////////////////////////////

    d->createLine(SRC_INFO, name + "_OP", name + "_OUT", name);
    d->createLine(SRC_INFO, std::get<0>(A), name + "_OP", name);
    d->createLine(SRC_INFO, std::get<0>(B), name + "_OP", name, 1, 2);

    /////////////////////////
    /// Create input ports //
    /////////////////////////

    unsigned int portId = 1;
    updateRequiredPorts(d, name, bpm, A, portId);
    updateRequiredPorts(d, name, bpm, B, portId);
  }

  return std::make_tuple(name, bpm);
}

ExpressionFunction::ExpressionFunction(MathOperator op, Expression *e)
{
  std::cout << "--|--|--|--|--|--) Found ExpressionFunction" << std::endl;
  _op = op;
  left = e;
}

blockPortMapping ExpressionFunction::generate(STLdriver *d, const std::string &parent, int vpos)
{

  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating ExpressionFunction" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  switch (_op) {
    case ABS:
      d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Math Operations/Abs', [" + name + " '/OP']);");
      break;
    case DIFF:
      d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Continuous/Derivative', [" + name + " '/OP']);");
      break;
    default: break;
  }

  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'position',[" + std::to_string(position_X_OP[0]) + ", 20, " + std::to_string(position_X_OP[1]) + ", 40]);");

  ///////////////////////////
  /// Generate expressions //
  ///////////////////////////

  blockPortMapping A = left->generate(d, name, 0);

  /////////////////////////////////////
  /// Create output port connections //
  /////////////////////////////////////

  d->createLine(SRC_INFO, name + "_OP", name + "_OUT", name);
  d->createLine(SRC_INFO, std::get<0>(A), name + "_OP", name);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);

  return std::make_tuple(name, bpm);
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
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "TODO - Generating ";
  if (_tSet)
    std::cout << "Timed ";
  std::cout << "STLFormulaUNTIL" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  ///////////////////////////
  /// Generate expressions //
  ///////////////////////////

  blockPortMapping A = left->generate(d, name, 1);
  blockPortMapping B = right->generate(d, name, 2);

  std::string untilBlock = d->createSTLFormulaUntil(name);

  //
  d->createLine(SRC_INFO, std::get<0>(A), untilBlock, name, 1, 2);
  d->createLine(SRC_INFO, std::get<0>(B), untilBlock, name, 1, 3);

  d->createLine(SRC_INFO, untilBlock, name + "_OUT", name);

  if (_tSet) {
    std::string ti = d->createTimeInterval(_t, name);
    d->createLine(SRC_INFO, ti, untilBlock, name);
  } else {
    BooleanValue bv(true);
    blockPortMapping bvpm = bv.generate(d, name, 0);
    d->createLine(SRC_INFO, std::get<0>(bvpm), untilBlock, name);
  }

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);
  updateRequiredPorts(d, name, bpm, B, portId);

  return std::make_tuple(name, bpm);
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
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating ";
  if (_tSet)
    std::cout << "Timed ";
  std::cout << "STLEventually" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  ///////////////////////////
  /// Generate expressions //
  ///////////////////////////

  blockPortMapping A = left->generate(d, name, _tSet ? 1 : 0);
  std::string ti;

  if (_tSet) {
    ti = d->createTimeInterval(_t, name);
  } else {
    TimeInterval t("[", "0", "X", "]");
    ti = d->createTimeInterval(t, name);
  }

  std::string to = d->createSTLFormulaTemporalOperator(EVENTUALLY, name);

  d->createLine(SRC_INFO, ti, to, name);
  d->createLine(SRC_INFO, std::get<0>(A), to, name, 1, 2);
  d->createLine(SRC_INFO, to, name + "_OUT", name);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);

  return std::make_tuple(name, bpm);
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
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "Generating ";
  if (_tSet)
    std::cout << "Timed ";
  std::cout << "STLAlways" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  ///////////////////////////
  /// Generate expressions //
  ///////////////////////////

  blockPortMapping A = left->generate(d, name, _tSet ? 1 : 0);

  if (_tSet) {
    std::string to = d->createSTLFormulaTemporalOperator(ALWAYS, name);
    std::string ti = d->createTimeInterval(_t, name);

    d->createLine(SRC_INFO, ti, to, name);
    d->createLine(SRC_INFO, std::get<0>(A), to, name, 1, 2);
    d->createLine(SRC_INFO, to, name + "_OUT", name);
  } else {
    d->createLine(SRC_INFO, std::get<0>(A), name + "_OUT", name);
  }

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);

  return std::make_tuple(name, bpm);
}

STLFormulaAND::STLFormulaAND(STLFormula *f1, STLFormula *f2)
{
  std::cout << "--|--|--|--|--|--) Found  STLFormulaAND" << std::endl;
  left = f1;
  right = f2;
}

blockPortMapping STLFormulaAND::generate(STLdriver *d, const std::string &parent, int vpos)
{
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cout << "TODO - Generating STLFormulaAND" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  ///////////////////////////
  /// Generate expressions //
  ///////////////////////////

  blockPortMapping A = left->generate(d, name, 0);
  blockPortMapping B = right->generate(d, name, 1);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);
  updateRequiredPorts(d, name, bpm, B, portId);

  return std::make_tuple(name, bpm);
}

STLFormulaNOT::STLFormulaNOT(STLFormula *f)
{
  std::cout << "--|--|--|--|--|--) Found  STLFormulaNOT" << std::endl;
  left = f;
}

blockPortMapping STLFormulaNOT::generate(STLdriver *d, const std::string &parent, int vpos)
{
  portMapping bpm;
  unsigned int y = 40 * vpos + 20;

  std::cerr << "TODO - Generating STLFormulaNOT" << std::endl;

  // Create empty container
  std::string name = d->createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  // Create OUT port
  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  blockPortMapping A = left->generate(d, name, 0);

  d->testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/NEG']);");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'Operator', 'NOT');");
  d->testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  d->createLine(SRC_INFO, name + "_OP", name + "_OUT", name);
  d->createLine(SRC_INFO, std::get<0>(A), name + "_OP", name);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  updateRequiredPorts(d, name, bpm, A, portId);

  return std::make_tuple(name, bpm);
}
