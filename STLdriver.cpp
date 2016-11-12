#include <STLdriver.h>
#include <STLparser.hh>

#include <algorithm>

#define SRC_INFO_TEMP __FILE__, __func__, __LINE__
#define SRC_INFO (std::make_tuple(__FILE__, __func__, __LINE__))

const std::string ADD_LINE_AUTOROUTING = ", 'autorouting','on'";

const unsigned int position_X_IN[2] = {20, 40};
const unsigned int position_X_EXP[2] = {80, 150};
const unsigned int position_X_OP[2] = {190, 210};
const unsigned int position_X_OUT[2] = {240, 260};

const unsigned int portOffset = 40;

STLdriver::STLdriver(const std::string &path) :
  trace_scanning(false),
  trace_parsing(false),
  REF_input(false),
  SIG_input(false),
  status(HEADER)
{
  this->path = path;

  testBlockAppendFile.open(path + "AUTOGEN_testBlock.m", std::ofstream::out);
  if (!testBlockAppendFile.is_open())
    throw "Error opening AUTOGEN_testBlock.m";

  testBlockRoutingAppendFile.open(path + "AUTOGEN_testBlockRouting.m", std::ofstream::out);
  if (!testBlockRoutingAppendFile.is_open())
    throw "Error opening AUTOGEN_testBlockRouting.m";
}

STLdriver::~STLdriver()
{
  testBlockAppendFile.close();
  testBlockRoutingAppendFile.close();
}

int STLdriver::parse(const std::string &f)
{
  file = f;
  scan_begin();
  yy::STLparser parser(*this);
  parser.set_debug_level(trace_parsing);
  int res = parser.parse();
  scan_end();

  return res;
}

void STLdriver::error(const yy::location& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}

void STLdriver::error(const std::string& m)
{
  std::cerr << m << std::endl;
}

void STLdriver::fileAppend(const std::string &s, std::ofstream &f)
{
  //std::cout << s;
  f << s;
}

void STLdriver::testBlockAppendLn(const std::string &fileName, const std::string &functionName, int lineNumber, const std::string &s)
{
  fileAppend("% " + fileName + "::" + functionName + "::" + std::to_string(lineNumber), testBlockAppendFile);
  //std::cout << std::endl;
  testBlockAppendFile << std::endl;

  fileAppend(s, testBlockAppendFile);
  //std::cout << std::endl;
  testBlockAppendFile << std::endl;

  //std::cout << std::endl;
  testBlockAppendFile << std::endl;
}

void STLdriver::testBlockRoutingAppendLn(const std::string &fileName, const std::string &functionName, int lineNumber, const std::string &s)
{
  fileAppend("% " + fileName + "::" + functionName + "::" + std::to_string(lineNumber), testBlockRoutingAppendFile);
  //std::cout << std::endl;
  testBlockRoutingAppendFile << std::endl;

  fileAppend(s, testBlockRoutingAppendFile);
  //std::cout << std::endl;
  testBlockRoutingAppendFile << std::endl;

  //std::cout << std::endl;
  testBlockRoutingAppendFile << std::endl;
}

void STLdriver::createIsStepBlock(std::string v1, std::string v2)
{
  std::cout << "createStepBlock [" + v1 + "] [" + v2 + "]" << std::endl;
}

void STLdriver::createDiffBlock(std::string v)
{
  std::cout << "createDiffBlock [" + v + "]" << std::endl;
}

ComparisonOperation * STLdriver::createComparisonBlock(ComparisonOperator op, MathOperation *a, MathOperation *b)
{
  std::cout << "--|--|--) createComparisonBlock" << std::endl;
  //createExpression(a);
  //createExpression(b);

  ComparisonOperation * c = new ComparisonOperation;
  c->op = op;
  c->a = a;
  c->b = b;

  return c;
}
MathOperation * STLdriver::createMathBlock(MathOperator op, MathOperation *a, MathOperation *b)
{
  std::cout << "--|--|--|--|--) createMathBlock" << std::endl;

  MathOperation * m = new MathOperation;
  m->op = op;
  m->a = a;
  m->b = b;

  return m;
}

LogicalOperation * STLdriver::createLogicalBlock(LogicalOperator op, LogicalOperation *a, LogicalOperation *b)
{
  std::cout << "--|--|--|--|--) createMathBlock" << std::endl;

  LogicalOperation * l = new LogicalOperation;
  l->op = op;
  l->a = a;
  l->b = b;

  return l;
}

bool STLdriver::variableExists(std::string v)
{
  return (variablesValues.find(v) != variablesValues.end());
}

bool STLdriver::portExists(std::string v)
{
  return (std::find(ports.begin(), ports.end(), v) != ports.end());
}

void STLdriver::setVariable(std::string name, std::string value)
{
  variablesValues[name] = value;
}

std::string STLdriver::getVariable(std::string name)
{
  return variablesValues[name];
}

void STLdriver::setStatus(DriverStatus s)
{
  status = s;
}

void STLdriver::printConstantValues()
{
  std::cout << "--) printConstantValues()" << std::endl;

  typedef std::map<std::string, std::string>::iterator it_type;
  for(it_type it = variablesValues.begin(); it != variablesValues.end(); ++it) {
    std::cout << "  " << it->first << " = " << it->second << std::endl;
  }

  std::cout << std::endl << "-/--/--/--/--/--/--/--/--/-" << std::endl << std::endl;
}


std::string STLdriver::createEmptyBlock(srcInfo code, const std::string &parent, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2)
{
  static unsigned int identifier = 0;
  std::string name = "blk" + std::to_string(identifier++);

  testBlockAppendLn(SRC_INFO_TEMP, name + " = addEmptySubsystem(" + parent + ", '" + name + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + ",'position',[" + std::to_string(x1) + ", " + std::to_string(y1) + ", " + std::to_string(x2) + ", " + std::to_string(y2) + "]);");

  return name;
}

void STLdriver::createLine(srcInfo code,
                           const std::string &src,
                           const std::string &dst,
                           const std::string &root,
                           unsigned int src_p,
                           unsigned int dst_p)
{
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "OutPort1 = get_param(" + src + ", 'PortHandles');");
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "InPort1 = get_param(" + dst + ", 'PortHandles');");
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "add_line(" + root + ", OutPort1.Outport(" + std::to_string(src_p) + "), InPort1.Inport(" + std::to_string(dst_p) + ")" + ADD_LINE_AUTOROUTING + ");");
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
blockPortMapping STLdriver::createExpression(MathOperation * e,
                                             std::string parent,
                                             unsigned int vpos,
                                             std::string BLOCK_ROOT)
{
  static unsigned int identifier = 0;
  std::string name = "exp" + std::to_string(identifier++);

  portMapping requiredPorts;
  unsigned int y = 40 * vpos + 20;

  // Create empty container block
  testBlockAppendLn(SRC_INFO_TEMP, name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], '" + name + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + ",'position',[" + std::to_string(position_X_EXP[0]) + ", " + std::to_string(y) + ", " + std::to_string(position_X_EXP[1]) + ", " + std::to_string(y + 20) + "]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + BLOCK_ROOT + " '/" + parent + "/" + name + "/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  if (e->op == CONST || e->op == PORT) {
    // Create block containing input port or constant values

    if (e->op == CONST) {
      testBlockAppendLn(SRC_INFO_TEMP, name + "_IN = addConst([" + BLOCK_ROOT + " '/" + parent + "/" + name + "'], '" + name + "', '" + e->value + "');");
    } else if (e->op == PORT) {
      std::string portName = e->value;

      testBlockAppendLn(SRC_INFO_TEMP, name + "_IN = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + name + "/" + portName + "']);");

      requiredPorts[portName] = 1;
    }

    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_IN,'position', [" + std::to_string(position_X_IN[0]) + ", 20, " + std::to_string(position_X_IN[1]) + ", 40]);");

    createLine(SRC_INFO, name + "_IN", name + "_OUT", "[" + BLOCK_ROOT + " '/" + parent + "/" + name + "']");
  } else {// SUM, SUB, MUL, DIV
    // Create mathematical block

    std::string matOp;
    switch (e->op) {
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

    testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Math Operations/" + matOp + "', [" + BLOCK_ROOT + " '/" + parent + "/" + name + "/" + name + "_OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'position',[" + std::to_string(position_X_OP[0]) + ", 20, " + std::to_string(position_X_OP[1]) + ", 40]);");

    ///////////////////////////
    /// Generate expressions //
    ///////////////////////////

    blockPortMapping A = createExpression(e->a, parent + "/" + name, 0, BLOCK_ROOT);
    blockPortMapping B = createExpression(e->b, parent + "/" + name, 1, BLOCK_ROOT);

    /////////////////////////////////////
    /// Create output port connections //
    /////////////////////////////////////

    createLine(SRC_INFO, name + "_OP", name + "_OUT", "[" + BLOCK_ROOT + " '/" + parent + "/" + name + "']");
    createLine(SRC_INFO, std::get<0>(A), name + "_OP", "[" + BLOCK_ROOT + " '/" + parent + "/" + name + "']", 1, 1);
    createLine(SRC_INFO, std::get<0>(B), name + "_OP", "[" + BLOCK_ROOT + " '/" + parent + "/" + name + "']", 1, 2);

    /////////////////////////
    /// Create input ports //
    /////////////////////////

    unsigned int portId = 1;

    for (auto pm : std::get<1>(A)) {
      portMapping::iterator it = requiredPorts.find(pm.first);
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(SRC_INFO_TEMP, name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + name + "/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        requiredPorts[pm.first] = portId;
        portId++;
      }

      // And line generated
      createLine(SRC_INFO, name + pm.first, std::get<0>(A), "[" + BLOCK_ROOT + " '/" + parent + "/" + name + "']", 1, pm.second);
    }

    for (auto pm : std::get<1>(B)) {
      portMapping::iterator it = requiredPorts.find(pm.first);
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(SRC_INFO_TEMP, name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + name + "/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        requiredPorts[pm.first] = portId;
        portId++;
      }

      // And line generated
      createLine(SRC_INFO, name + pm.first, std::get<0>(B), "[" + BLOCK_ROOT + " '/" + parent + "/" + name + "']", 1, pm.second);
    }
  }

  return std::make_tuple(name, requiredPorts);
}

void STLdriver::createSTLFormulaTimeInterval(const TimeInterval &time, std::string parent, std::string BLOCK_ROOT)
{
  std::string block_name = parent + "_TIME_RANGE";

  // Create empty container
  testBlockAppendLn(SRC_INFO_TEMP, block_name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], 'TIME_RANGE');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + ", 'position', [" + std::to_string(position_X_EXP[0])+ ", 60, " + std::to_string(position_X_EXP[1])+ ", 80])");

  // Create internals

  // Output port
  testBlockAppendLn(SRC_INFO_TEMP, block_name + "_OUT = add_block('simulink/Sinks/Out1', [" + block_name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_OUT, 'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

  // Clock
  testBlockAppendLn(SRC_INFO_TEMP, block_name + "_CLOCK = add_block('simulink/Sources/Clock', [" + block_name + " '/CLOCK']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_CLOCK, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");

  // Interval checker
  testBlockAppendLn(SRC_INFO_TEMP, block_name + "_CHECK = add_block('simulink/Logic and Bit Operations/Interval Test', [" + block_name + " '/CHECK']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_CHECK, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_CHECK, 'uplimit', '" + time.end + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_CHECK, 'lowlimit', '" + time.start + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_CHECK, 'IntervalClosedLeft', '" + time.startClosed + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_CHECK, 'IntervalClosedRight', '" + time.endClosed + "');");

  // Connect internals
  createLine(SRC_INFO, block_name + "_CLOCK", block_name + "_CHECK", block_name);
  createLine(SRC_INFO, block_name + "_CHECK", block_name + "_OUT", block_name);
}

void STLdriver::createSTLFormulaTemporalOperator(TemporalOperator op, std::string parent, std::string BLOCK_ROOT)
{
  std::string block_name = parent + "_TEMP_OP";

  // Create empty container
  testBlockAppendLn(SRC_INFO_TEMP, block_name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], 'TEMP_OP');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + ", 'position', [" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40])");

  // Create output port
  testBlockAppendLn(SRC_INFO_TEMP, block_name + "_OUT = add_block('simulink/Sinks/Out1', [" + block_name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_OUT, 'position',[" + std::to_string(position_X_OUT[0] + 60) + ", 20, " + std::to_string(position_X_OUT[1] + 60) + ", 40]);");

  // Create input ports
  testBlockAppendLn(SRC_INFO_TEMP, block_name + "_STL = add_block('simulink/Sources/In1', [" + block_name + " '/STL']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_STL, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");
  testBlockAppendLn(SRC_INFO_TEMP, block_name + "_TIME = add_block('simulink/Sources/In1', [" + block_name + " '/TIME']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_TIME, 'position',[" + std::to_string(position_X_IN[0])+ ", 60, " + std::to_string(position_X_IN[1])+ ", 80]);");

  switch (op) {
    case ALWAYS:
      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_OP1 = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + block_name + " '/NOT']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_OP1, 'Operator', '" + "NOT" + "');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_OP1, 'position',[" + std::to_string(position_X_EXP[0])+ ", 60, " + std::to_string(position_X_EXP[1])+ ", 80]);");

      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_OP2 = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + block_name + " '/OR']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_OP2, 'Operator', '" + "OR" + "');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_OP2, 'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

      // Connect ports
      createLine(SRC_INFO, block_name + "_OP2", block_name + "_OUT", block_name);
      createLine(SRC_INFO, block_name + "_TIME", block_name + "_OP1", block_name);
      createLine(SRC_INFO, block_name + "_STL", block_name + "_OP2", block_name, 1, 1);
      createLine(SRC_INFO, block_name + "_OP1", block_name + "_OP2", block_name, 1, 2);
      break;
    case EVENTUALLY:
      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_AND = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + block_name + " '/AND']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_AND, 'Operator', '" + "AND" + "');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_AND, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");

      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_FALL = add_block('simulink/Logic and Bit Operations/Detect Decrease', [" + block_name + " '/FALL_EDGE']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_FALL, 'position',[" + std::to_string(position_X_EXP[0])+ ", 60, " + std::to_string(position_X_EXP[1])+ ", 80]);");

      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_TRUE = addConst(" + block_name + ", 'TRUE', '1');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_TRUE, 'position',[" + std::to_string(position_X_EXP[0])+ ", 100, " + std::to_string(position_X_EXP[1])+ ", 120]);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_TRUE, 'OutDataTypeStr', 'boolean');");

      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_FALSE = addConst(" + block_name + ", 'FALSE', '0');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_FALSE, 'position',[" + std::to_string(position_X_EXP[0])+ ", 140, " + std::to_string(position_X_EXP[1])+ ", 160]);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_FALSE, 'OutDataTypeStr', 'boolean');");

      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_FFSR = add_block('simulink_extras/Flip Flops/S-R Flip-Flop', [" + block_name + " '/FFSR']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_FFSR, 'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_NULL = add_block('simulink/Sinks/Terminator', [" + block_name + " '/NULL']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_NULL, 'position',[" + std::to_string(position_X_OP[0])+ ", 140, " + std::to_string(position_X_OP[1])+ ", 160]);");

      testBlockAppendLn(SRC_INFO_TEMP, block_name + "_SWITCH = add_block('simulink/Signal Routing/Switch', [" + block_name + " '/SWITCH']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + block_name + "_SWITCH, 'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 120]);");

      // Connect ports
      createLine(SRC_INFO, block_name + "_STL", block_name + "_AND", block_name);
      createLine(SRC_INFO, block_name + "_TIME", block_name + "_AND", block_name, 1, 2);
      createLine(SRC_INFO, block_name + "_TIME", block_name + "_FALL", block_name);

      createLine(SRC_INFO, block_name + "_AND", block_name + "_FFSR", block_name);
      createLine(SRC_INFO, block_name + "_FALSE", block_name + "_FFSR", block_name, 1, 2);

      createLine(SRC_INFO, block_name + "_FFSR", block_name + "_NULL", block_name, 2, 1);

      createLine(SRC_INFO, block_name + "_FFSR", block_name + "_SWITCH", block_name, 1, 1);
      createLine(SRC_INFO, block_name + "_FALL", block_name + "_SWITCH", block_name, 1, 2);
      createLine(SRC_INFO, block_name + "_TRUE", block_name + "_SWITCH", block_name, 1, 3);

      createLine(SRC_INFO, block_name + "_SWITCH", block_name + "_OUT", block_name);
      break;
    default:
      break;
  }
}

void STLdriver::createSTLFormulas()
{
  portMapping requiredPorts;

  unsigned int counter = 0;
  for (auto ll : STLFormulas) {
    std::string formulaVisibleName = "STLFormula_" + std::to_string(counter);
    std::string formulaName = createEmptyBlock(SRC_INFO, "ROOT", position_X_EXP[0], position_X_EXP[1], portOffset * counter + 20, portOffset * counter + 20 + 20);

    //testBlockAppendLn(SRC_INFO_TEMP, formulaName + " = addEmptySubsystem(ROOT, '" + formulaName + "');");
    //testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + formulaName + ",'position',[" + std::to_string(position_X_EXP[0])+ ", " + std::to_string(portOffset * counter + 20) + ", " + std::to_string(position_X_EXP[1])+ ", " + std::to_string(portOffset * counter + 20 + 20) + "]);");

    blockPortMapping bpm = createSTLFormulaBody(std::get<2>(ll), formulaName, 0, "ROOT");

    testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_OUT" + std::get<0>(bpm) + " = add_block('simulink/Sinks/Out1', [ROOT '/" + formulaName + "/VALID']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_OUT" + std::get<0>(bpm) + ",'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40]);");

    testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_ASSERTION_" + std::get<0>(bpm) + " = add_block('simulink/Model Verification/Assertion', [ROOT '/VALID_" + std::to_string(counter) + "']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_ASSERTION_" + std::get<0>(bpm) + ",'position',[" + std::to_string(position_X_OUT[0])+ ", " + std::to_string(portOffset * counter + 20) + ", " + std::to_string(position_X_OUT[1])+ ", " + std::to_string(portOffset * counter + 20 + 20) + "]);");

    createLine(SRC_INFO, formulaName, "TEST_ROOT_ASSERTION_" + std::get<0>(bpm), "ROOT");

    createSTLFormulaTimeInterval(std::get<1>(ll), formulaName, "ROOT");
    createSTLFormulaTemporalOperator(std::get<0>(ll), formulaName, "ROOT");

    createLine(SRC_INFO, formulaName + "_TEMP_OP", "TEST_ROOT_OUT" + std::get<0>(bpm), "[ROOT '/" + formulaName + "']");
    createLine(SRC_INFO, std::get<0>(bpm), formulaName + "_TEMP_OP", "[ROOT '/" + formulaName + "']");
    createLine(SRC_INFO, formulaName + "_TIME_RANGE", formulaName + "_TEMP_OP", "[ROOT '/" + formulaName + "']", 1, 2);

    // Create ports required by the formula
    unsigned int portId = 1;
    for (auto pm : std::get<1>(bpm)) {

      // Create port inside block
      testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_IN" + formulaName + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + formulaName + "/" + pm.first + "']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_IN" + formulaName + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      // And line inside block
      createLine(SRC_INFO, "TEST_ROOT_IN" + formulaName + pm.first, std::get<0>(bpm), "[ROOT '/" + formulaName + "']", 1, pm.second);

      portMapping::iterator it = requiredPorts.find(std::get<0>(pm));
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_IN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_IN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        //testBlockRoutingAppendLn(SRC_INFO_TEMP, "MAIN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        //testBlockRoutingAppendLn(SRC_INFO_TEMP, "set_param(MAIN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * portId + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * portId + 20 + 20) + "]);");

        //testBlockRoutingAppendLn(SRC_INFO_TEMP, "OutPort1 = get_param(MAIN" + pm.first + ", 'PortHandles');");
        //testBlockRoutingAppendLn(SRC_INFO_TEMP, "InPort1 = get_param(TEST, 'PortHandles');");
        //testBlockRoutingAppendLn(SRC_INFO_TEMP, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport( " + std::to_string(portId) + " )" + ADD_LINE_AUTOROUTING + ");");

        requiredPorts[std::get<0>(pm)] = portId;
        portId++;
      }

      // And line outside block
      createLine(SRC_INFO, "TEST_ROOT_IN" + pm.first, formulaName, "ROOT", 1, pm.second);
    }

    counter++;
  }
}

blockPortMapping STLdriver::createSTLFormulaBody(LogicalOperation *l, std::string parent, unsigned int y, std::string BLOCK_ROOT)
{
  unsigned int vpos;
  portMapping requiredPorts;
  static unsigned int STLFormula_num = 0;
  std::string STLFormula_name = "STLFormulaSub_" + std::to_string(STLFormula_num++);
  blockPortMapping A;
  blockPortMapping B;

  vpos = 40 * y + 20;

  testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], '" + STLFormula_name + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + ",'position',[" + std::to_string(position_X_EXP[0])+ ", " + std::to_string(vpos) + ", " + std::to_string(position_X_EXP[1])+ ", " + std::to_string(vpos + 20) + "])");

  testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OUT = add_block('simulink/Sinks/Out1', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OUT,'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

  if (l->op == COMPARISON) {
    // Create block containing the TWO comparison expressions

    std::string relOp;
    switch (l->value->op) {
      case GEQ:
        relOp = ">=";
        break;
      case LEQ:
        relOp = "<=";
        break;
      case GREATER:
        relOp = ">";
        break;
      case SMALLER:
        relOp = "<";
        break;
      case EQUAL:
        relOp = "==";
        break;
      case NEQUAL:
        relOp = "~=";
        break;
      default: break;
    }

    testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/" + STLFormula_name + "_OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'Operator', '" + relOp + "');");

    A = createExpression(l->value->a, parent + "/" + STLFormula_name, 0, BLOCK_ROOT);
    B = createExpression(l->value->b, parent + "/" + STLFormula_name, 1, BLOCK_ROOT);

  } else {// AND, OR
    //////////////////////////
    // Create logical block //
    //////////////////////////

    // Create empty subsystem (this)
    std::string par = "";
    if (parent == "")
      par = BLOCK_ROOT;
    else
      par = "[" + BLOCK_ROOT + " '/" + parent + "']";

    std::string logOp;
    switch (l->op) {
      case AND:
        logOp = "AND";
        break;
      case OR:
        logOp = "OR";
        break;
      default: break;
    }

    testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'Operator', '" + logOp + "');");

    A = createSTLFormulaBody(l->a, parent + "/" + STLFormula_name, 0, BLOCK_ROOT);
    B = createSTLFormulaBody(l->b, parent + "/" + STLFormula_name, 1, BLOCK_ROOT);
  }
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  createLine(SRC_INFO, STLFormula_name + "_OP", STLFormula_name + "_OUT", "[" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "']");
  createLine(SRC_INFO, std::get<0>(A), STLFormula_name + "_OP", "[" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "']", 1, 1);
  createLine(SRC_INFO, std::get<0>(B), STLFormula_name + "_OP", "[" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "']", 1, 2);

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;

  for (auto pm : std::get<1>(A)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    createLine(SRC_INFO, STLFormula_name + pm.first, std::get<0>(A), "[" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "']", 1, pm.second);
  }

  for (auto pm : std::get<1>(B)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated

    createLine(SRC_INFO, STLFormula_name + pm.first, std::get<0>(B), "[" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "']", 1, pm.second);
  }

  return std::make_tuple(STLFormula_name, requiredPorts);
}

#if 0
blockPortMapping STLdriver::createSTLFormula(const TemporalOperator &tOp, const TimeInterval &tIn, LogicalOperation *l)
{
  blockPortMapping bpm = createSTLFormulaBody(l, "prova", 0);

  //createSTLFormulaTimeInterval()

  return bpm;

  unsigned int vpos;
  portMapping requiredPorts;
  static unsigned int STLFormula_num = 0;
  std::string STLFormula_name = "STLFormula_" + std::to_string(STLFormula_num++);
  blockPortMapping A;
  blockPortMapping B;

  vpos = 40 * y + 20;

  testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + " = addEmptySubsystem([" + TEST_ROOT + " '" + parent + "'], '" + STLFormula_name + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + ",'position',[" + std::to_string(position_X_EXP[0])+ ", " + std::to_string(vpos) + ", " + std::to_string(position_X_EXP[1])+ ", " + std::to_string(vpos + 20) + "])");

  testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OUT,'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

  if (l->op == COMPARISON) {
    // Create block containing the TWO comparison expressions

    std::string relOp;
    switch (l->value->op) {
      case GEQ:
        relOp = ">=";
        break;
      case LEQ:
        relOp = "<=";
        break;
      case GREATER:
        relOp = ">";
        break;
      case SMALLER:
        relOp = "<";
        break;
      case EQUAL:
        relOp = "==";
        break;
      case NEQUAL:
        relOp = "~=";
        break;
      default: break;
    }

    testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/" + STLFormula_name + "_OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'Operator', '" + relOp + "');");

    A = createExpression(l->value->a, parent + "/" + STLFormula_name, 0);
    B = createExpression(l->value->b, parent + "/" + STLFormula_name, 1);

  } else {// AND, OR
    //////////////////////////
    // Create logical block //
    //////////////////////////

    // Create empty subsystem (this)
    std::string par = "";
    if (parent == "")
      par = TEST_ROOT;
    else
      par = "[" + TEST_ROOT + " '" + parent + "']";

    std::string logOp;
    switch (l->op) {
      case AND:
        logOp = "AND";
        break;
      case OR:
        logOp = "OR";
        break;
      default: break;
    }

    testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'Operator', '" + logOp + "');");

    A = createSTLFormulaBody(l->a, parent + "/" + STLFormula_name, 0);
    B = createSTLFormulaBody(l->b, parent + "/" + STLFormula_name, 1);
  }
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  testBlockAppendLn(SRC_INFO_TEMP, "OutPort1 = get_param(" + STLFormula_name + "_OP, 'PortHandles');");
  testBlockAppendLn(SRC_INFO_TEMP, "InPort1 = get_param(" + STLFormula_name + "_OUT, 'PortHandles');");
  testBlockAppendLn(SRC_INFO_TEMP, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

  testBlockAppendLn(SRC_INFO_TEMP, "OutPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
  testBlockAppendLn(SRC_INFO_TEMP, "OutPort2 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
  testBlockAppendLn(SRC_INFO_TEMP, "InPort1 = get_param(" + STLFormula_name + "_OP, 'PortHandles');");
  testBlockAppendLn(SRC_INFO_TEMP, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
  testBlockAppendLn(SRC_INFO_TEMP, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort2.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;

  for (auto pm : std::get<1>(A)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    testBlockAppendLn(SRC_INFO_TEMP, "OutPort1 = get_param(" + STLFormula_name + pm.first + ", 'PortHandles');");
    testBlockAppendLn(SRC_INFO_TEMP, "InPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
    testBlockAppendLn(SRC_INFO_TEMP, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
  }

  for (auto pm : std::get<1>(B)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    testBlockAppendLn(SRC_INFO_TEMP, "OutPort1 = get_param(" + STLFormula_name + pm.first + ", 'PortHandles');");
    testBlockAppendLn(SRC_INFO_TEMP, "InPort1 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
    testBlockAppendLn(SRC_INFO_TEMP, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
  }

  return std::make_tuple(STLFormula_name, requiredPorts);
}
#endif

void STLdriver::parsePorts()
{
  systemPortsFile.open(path + "AUTOGEN_portList.txt", std::ifstream::in);
  if (!systemPortsFile.is_open())
    throw "Error opening AUTOGEN_portList.txt";

  std::cout << "##parsePorts()" << std::endl;

  std::string portName;
  while (systemPortsFile.good()) {
    std::getline(systemPortsFile, portName);

    if (portName.length() > 0)
      ports.push_back(portName);
    else
      break;
  }

  for (std::string p : ports)
    std::cout << "- " << p << std::endl;

  std::cout << "## DONE" << std::endl;

  systemPortsFile.close();
}

void STLdriver::addSTLFormula(const TemporalOperator &tOp,
                              const TimeInterval &tIn,
                              LogicalOperation *l)
{
  STLFormulas.push_back(std::make_tuple(tOp, tIn, l));
}
