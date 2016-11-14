#include <STLdriver.h>
#include <STLparser.hh>

#include <algorithm>

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

  for (auto n : nodes)
    delete n;
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
  l->value = nullptr;
  l->arg1 = nullptr;
  l->arg2 = nullptr;

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

std::string STLdriver::createIsStepBlock(srcInfo code, const std::string &parent, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2)
{
  // Create empty container
  std::string name = createEmptyBlock(SRC_INFO, parent, x1, x2, y1, y2);

  // Create input ports
  testBlockAppendLn(SRC_INFO_TEMP, name + "_SIG = add_block('simulink/Sources/In1', [" + name + " '/SIG']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_SIG, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");
  testBlockAppendLn(SRC_INFO_TEMP, name + "_H = add_block('simulink/Sources/In1', [" + name + " '/H']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_H, 'position',[" + std::to_string(position_X_IN[0])+ ", 100, " + std::to_string(position_X_IN[1])+ ", 120]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_MEM = add_block('simulink/Discrete/Memory', [" + name + " '/MEM']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_MEM, 'position',[80, 60, 100, 80]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_SUB = add_block('simulink/Math Operations/Subtract', [" + name + " '/SUB']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_SUB,'position',[140, 20, 160, 40]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_CMP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + name + " '/CMP']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CMP,'Operator', '>');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CMP,'position',[200, 20, 220, 40]);");

  // Create output port
  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT, 'position',[260, 20, 280, 40]);");

  // Create links
  createLine(SRC_INFO, name + "_SIG", name + "_MEM", name);
  createLine(SRC_INFO, name + "_SIG", name + "_SUB", name);
  createLine(SRC_INFO, name + "_MEM", name + "_SUB", name, 1, 2);
  createLine(SRC_INFO, name + "_SUB", name + "_CMP", name);
  createLine(SRC_INFO, name + "_H", name + "_CMP", name, 1, 2);
  createLine(SRC_INFO, name + "_CMP", name + "_OUT", name);

  return name;
}

std::string STLdriver::createEmptyBlock(srcInfo code, const std::string &parent, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2)
{
  static unsigned int identifier = 0;
  std::string name = "blk" + std::to_string(identifier++);
  std::string path = parent + " '/" + name + "'";

  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), name + " = [" + path + "];");
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "add_block('simulink/Ports & Subsystems/Subsystem', " + name + ");");
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "delete_line(" + name + ", 'In1/1', 'Out1/1');");
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "delete_block([" + path + " '/In1']);");
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "delete_block([" + path + " '/Out1']);");
  testBlockAppendLn(std::get<0>(code), std::get<1>(code), std::get<2>(code), "set_param(" + name + ", 'position', [" + std::to_string(x1) + ", " + std::to_string(y1) + ", " + std::to_string(x2) + ", " + std::to_string(y2) + "]);");

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
                                             unsigned int vpos)
{
  unsigned int y = 40 * vpos + 20;
  portMapping requiredPorts;
  //static unsigned int STLFormula_num = 0;
  //std::string STLFormula_name = "STLFormulaSub_" + std::to_string(STLFormula_num++);
  blockPortMapping A;
  blockPortMapping B;

  // Create empty container block
  std::string name = createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  if (e->op == CONST || e->op == PORT) {
    // Create block containing input port or constant values

    if (e->op == CONST) {
      testBlockAppendLn(SRC_INFO_TEMP, name + "_IN = add_block('simulink/Sources/Constant', [" + name + " '/K']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_IN, 'Value', '" + e->value + "');");
    } else if (e->op == PORT) {
      std::string portName = e->value;
      testBlockAppendLn(SRC_INFO_TEMP, name + "_IN = add_block('simulink/Sources/In1', [" + name + " '/" + portName + "']);");
      requiredPorts[portName] = 1;
    }

    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_IN,'position', [" + std::to_string(position_X_IN[0]) + ", 20, " + std::to_string(position_X_IN[1]) + ", 40]);");

    createLine(SRC_INFO, name + "_IN", name + "_OUT", name);
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

    testBlockAppendLn(SRC_INFO_TEMP, name + "_OP = add_block('simulink/Math Operations/" + matOp + "', [" + name + " '/OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP,'position',[" + std::to_string(position_X_OP[0]) + ", 20, " + std::to_string(position_X_OP[1]) + ", 40]);");

    ///////////////////////////
    /// Generate expressions //
    ///////////////////////////

    blockPortMapping A = createExpression(e->a, name, 0);
    blockPortMapping B = createExpression(e->b, name, 1);

    /////////////////////////////////////
    /// Create output port connections //
    /////////////////////////////////////

    createLine(SRC_INFO, name + "_OP", name + "_OUT", name);
    createLine(SRC_INFO, std::get<0>(A), name + "_OP", name);
    createLine(SRC_INFO, std::get<0>(B), name + "_OP", name, 1, 2);

    /////////////////////////
    /// Create input ports //
    /////////////////////////

    unsigned int portId = 1;

    for (auto pm : std::get<1>(A)) {
      portMapping::iterator it = requiredPorts.find(pm.first);
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(SRC_INFO_TEMP, name + pm.first + " = add_block('simulink/Sources/In1', [" + name + " '/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        requiredPorts[pm.first] = portId;
        portId++;
      }

      // And line generated
      createLine(SRC_INFO, name + pm.first, std::get<0>(A), name, 1, pm.second);
    }

    for (auto pm : std::get<1>(B)) {
      portMapping::iterator it = requiredPorts.find(pm.first);
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(SRC_INFO_TEMP, name + pm.first + " = add_block('simulink/Sources/In1', [" + name + " '/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        requiredPorts[pm.first] = portId;
        portId++;
      }

      // And line generated
      createLine(SRC_INFO, name + pm.first, std::get<0>(B), name, 1, pm.second);
    }
  }

  return std::make_tuple(name, requiredPorts);
}

std::string STLdriver::createSTLFormulaUntil(const std::string &parent)
{
  // Create empty container
  std::string name = createEmptyBlock(SRC_INFO, parent, position_X_OP[0], position_X_OP[1], 20, 120);

  // Create input ports
  testBlockAppendLn(SRC_INFO_TEMP, name + "_ENABLE = add_block('simulink/Sources/In1', [" + name + " '/ENABLE']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_ENABLE, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");
  testBlockAppendLn(SRC_INFO_TEMP, name + "_FIRST = add_block('simulink/Sources/In1', [" + name + " '/FIRST']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FIRST, 'position',[" + std::to_string(position_X_IN[0])+ ", 60, " + std::to_string(position_X_IN[1])+ ", 80]);");
  testBlockAppendLn(SRC_INFO_TEMP, name + "_SECOND = add_block('simulink/Sources/In1', [" + name + " '/SECOND']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_SECOND, 'position',[" + std::to_string(position_X_IN[0])+ ", 100, " + std::to_string(position_X_IN[1])+ ", 120]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_NOT1 = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/NOT1']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_NOT1, 'Operator', '" + "NOT" + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_NOT1, 'position',[80, 60, 100, 80]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_NOT2 = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/NOT2']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_NOT2, 'Operator', '" + "NOT" + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_NOT2, 'position',[140, 20, 160, 40]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_AND = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/AND']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_AND, 'Operator', '" + "AND" + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_AND, 'position',[140, 60, 160, 80]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_FFSR = add_block('simulink_extras/Flip Flops/S-R Flip-Flop', [" + name + " '/FFSR']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FFSR, 'position',[200, 60, 220, 80]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_FALSE = add_block('simulink/Sources/Constant', [" + name + " '/FALSE']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FALSE, 'Value', '0');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FALSE, 'position',[140, 100, 160, 120]);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FALSE, 'OutDataTypeStr', 'boolean');");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_NULL = add_block('simulink/Sinks/Terminator', [" + name + " '/NULL']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_NULL, 'position',[260, 60, 280, 80]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_SWITCH = add_block('simulink/Signal Routing/Switch', [" + name + " '/SWITCH']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_SWITCH, 'position',[260, 100, 280, 160]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_OR = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/OR']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OR, 'Operator', '" + "OR" + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OR, 'position',[320, 20, 340, 40]);");

  // Create output port
  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT, 'position',[380, 20, 400, 40]);");

  // Create links
  createLine(SRC_INFO, name + "_ENABLE", name + "_NOT2", name);
  createLine(SRC_INFO, name + "_ENABLE", name + "_AND", name);
  createLine(SRC_INFO, name + "_FIRST", name + "_NOT1", name);
  createLine(SRC_INFO, name + "_SECOND", name + "_SWITCH", name, 1, 3);
  createLine(SRC_INFO, name + "_NOT1", name + "_AND", name, 1, 2);
  createLine(SRC_INFO, name + "_AND", name + "_FFSR", name);
  createLine(SRC_INFO, name + "_FFSR", name + "_NULL", name);
  createLine(SRC_INFO, name + "_FFSR", name + "_SWITCH", name, 2, 2);
  createLine(SRC_INFO, name + "_FIRST", name + "_SWITCH", name);
  createLine(SRC_INFO, name + "_NOT2", name + "_OR", name);
  createLine(SRC_INFO, name + "_OR", name + "_OUT", name);
  createLine(SRC_INFO, name + "_SWITCH", name + "_OR", name, 1, 2);
  createLine(SRC_INFO, name + "_FALSE", name + "_FFSR", name, 1, 2);

  return name;
}

std::string STLdriver::createSTLFormulaTimeInterval(const TimeInterval &time, const std::string &parent)
{
  // Create empty container
  std::string name = createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], 20, 40);

  // Create internals

  // Output port
  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT, 'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

  // Clock
  testBlockAppendLn(SRC_INFO_TEMP, name + "_CLOCK = add_block('simulink/Sources/Clock', [" + name + " '/CLOCK']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CLOCK, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");

  // Interval checker
  testBlockAppendLn(SRC_INFO_TEMP, name + "_CHECK = add_block('simulink/Logic and Bit Operations/Interval Test', [" + name + " '/CHECK']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'uplimit', '" + time.end + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'lowlimit', '" + time.start + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'IntervalClosedLeft', '" + time.startClosed + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'IntervalClosedRight', '" + time.endClosed + "');");

  // Connect internals
  createLine(SRC_INFO, name + "_CLOCK", name + "_CHECK", name);
  createLine(SRC_INFO, name + "_CHECK", name + "_OUT", name);

  return name;
}
/*
std::string STLdriver::createSTLFormulaTemporalOperator(TemporalOperator op, std::string parent)
{
  // Create empty container
  std::string name = createEmptyBlock(SRC_INFO, parent, position_X_OP[0], position_X_OP[1], 20, 80);

  // Create output port
  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT, 'position',[" + std::to_string(position_X_OUT[0] + 60) + ", 20, " + std::to_string(position_X_OUT[1] + 60) + ", 40]);");

  // Create input ports
  testBlockAppendLn(SRC_INFO_TEMP, name + "_TIME = add_block('simulink/Sources/In1', [" + name + " '/TIME']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_TIME, 'position',[" + std::to_string(position_X_IN[0])+ ", 60, " + std::to_string(position_X_IN[1])+ ", 80]);");
  testBlockAppendLn(SRC_INFO_TEMP, name + "_STL = add_block('simulink/Sources/In1', [" + name + " '/STL']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_STL, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");

  switch (op) {
    case ALWAYS:
      testBlockAppendLn(SRC_INFO_TEMP, name + "_OP1 = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/NOT']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP1, 'Operator', '" + "NOT" + "');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP1, 'position',[" + std::to_string(position_X_EXP[0])+ ", 60, " + std::to_string(position_X_EXP[1])+ ", 80]);");

      testBlockAppendLn(SRC_INFO_TEMP, name + "_OP2 = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/OR']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP2, 'Operator', '" + "OR" + "');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OP2, 'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

      // Connect ports
      createLine(SRC_INFO, name + "_OP2", name + "_OUT", name);
      createLine(SRC_INFO, name + "_TIME", name + "_OP1", name);
      createLine(SRC_INFO, name + "_STL", name + "_OP2", name, 1, 1);
      createLine(SRC_INFO, name + "_OP1", name + "_OP2", name, 1, 2);
      break;
    case EVENTUALLY:
      testBlockAppendLn(SRC_INFO_TEMP, name + "_AND = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/AND']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_AND, 'Operator', '" + "AND" + "');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_AND, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");

      testBlockAppendLn(SRC_INFO_TEMP, name + "_FALL = add_block('simulink/Logic and Bit Operations/Detect Decrease', [" + name + " '/FALL_EDGE']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FALL, 'position',[" + std::to_string(position_X_EXP[0])+ ", 60, " + std::to_string(position_X_EXP[1])+ ", 80]);");

      testBlockAppendLn(SRC_INFO_TEMP, name + "_TRUE = add_block('simulink/Sources/Constant', [" + name + " '/TRUE']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_TRUE, 'Value', '1');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_TRUE, 'position',[" + std::to_string(position_X_EXP[0])+ ", 100, " + std::to_string(position_X_EXP[1])+ ", 120]);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_TRUE, 'OutDataTypeStr', 'boolean');");

      testBlockAppendLn(SRC_INFO_TEMP, name + "_FALSE = add_block('simulink/Sources/Constant', [" + name + " '/FALSE']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FALSE, 'Value', '0');");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FALSE, 'position',[" + std::to_string(position_X_EXP[0])+ ", 140, " + std::to_string(position_X_EXP[1])+ ", 160]);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FALSE, 'OutDataTypeStr', 'boolean');");

      testBlockAppendLn(SRC_INFO_TEMP, name + "_FFSR = add_block('simulink_extras/Flip Flops/S-R Flip-Flop', [" + name + " '/FFSR']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FFSR, 'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

      testBlockAppendLn(SRC_INFO_TEMP, name + "_NULL = add_block('simulink/Sinks/Terminator', [" + name + " '/NULL']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_NULL, 'position',[" + std::to_string(position_X_OP[0])+ ", 140, " + std::to_string(position_X_OP[1])+ ", 160]);");

      testBlockAppendLn(SRC_INFO_TEMP, name + "_SWITCH = add_block('simulink/Signal Routing/Switch', [" + name + " '/SWITCH']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_SWITCH, 'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 120]);");

      // Connect ports
      createLine(SRC_INFO, name + "_STL", name + "_AND", name);
      createLine(SRC_INFO, name + "_TIME", name + "_AND", name, 1, 2);
      createLine(SRC_INFO, name + "_TIME", name + "_FALL", name);

      createLine(SRC_INFO, name + "_AND", name + "_FFSR", name);
      createLine(SRC_INFO, name + "_FALSE", name + "_FFSR", name, 1, 2);

      createLine(SRC_INFO, name + "_FFSR", name + "_NULL", name, 2, 1);

      createLine(SRC_INFO, name + "_FFSR", name + "_SWITCH", name, 1, 1);
      createLine(SRC_INFO, name + "_FALL", name + "_SWITCH", name, 1, 2);
      createLine(SRC_INFO, name + "_TRUE", name + "_SWITCH", name, 1, 3);

      createLine(SRC_INFO, name + "_SWITCH", name + "_OUT", name);
      break;
    default:
      break;
  }

  return name;
}
*/
void STLdriver::createSTLFormulas()
{
  unsigned int counter = 0;
  for (TreeNode * n : nodes) {
    std::cout << std::endl << "Node " << counter++ << std::endl;
    n->generate(this, "ROOT", 0);
  }

  /*
  portMapping requiredPorts;
  unsigned int mainPortId = 1;
  unsigned int counter = 0;
  for (auto ll : STLFormulas) {
    std::string formulaName = createEmptyBlock(SRC_INFO, "ROOT", position_X_EXP[0], position_X_EXP[1], portOffset * counter + 20, portOffset * counter + 20 + 20);

    blockPortMapping bpm = createSTLFormulaBody(std::get<2>(ll), formulaName, 1);

    std::string untilBlock;
    blockPortMapping bpm2;

    if (std::get<3>(ll) != nullptr) {
      bpm2 = createSTLFormulaBody(std::get<3>(ll), formulaName, 2);
      untilBlock = createSTLFormulaUntil(formulaName);
    }

    testBlockAppendLn(SRC_INFO_TEMP, formulaName + "_OUT = add_block('simulink/Sinks/Out1', [ROOT '/" + formulaName + "/VALID']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + formulaName + "_OUT, 'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40]);");

    testBlockAppendLn(SRC_INFO_TEMP, formulaName + "_ASSERT = add_block('simulink/Model Verification/Assertion', [ROOT '/VALID_" + std::to_string(counter) + "']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + formulaName + "_ASSERT,'position',[" + std::to_string(position_X_OUT[0])+ ", " + std::to_string(portOffset * counter + 20) + ", " + std::to_string(position_X_OUT[1])+ ", " + std::to_string(portOffset * counter + 20 + 20) + "]);");

    createLine(SRC_INFO, formulaName, formulaName + "_ASSERT", "ROOT");

    std::string ti = createSTLFormulaTimeInterval(std::get<1>(ll), formulaName);


    if (std::get<3>(ll) == nullptr) {
      std::string to = createSTLFormulaTemporalOperator(std::get<0>(ll), formulaName);

      createLine(SRC_INFO, ti, to, formulaName, 1, 1);
      createLine(SRC_INFO, std::get<0>(bpm), to, formulaName, 1, 2);

      createLine(SRC_INFO, to, formulaName + "_OUT", formulaName);
    } else {
      createLine(SRC_INFO, ti, untilBlock, formulaName, 1, 1);
      createLine(SRC_INFO, std::get<0>(bpm), untilBlock, formulaName, 1, 2);
      createLine(SRC_INFO, std::get<0>(bpm2), untilBlock, formulaName, 1, 3);

      createLine(SRC_INFO, untilBlock, formulaName + "_OUT", formulaName);
    }

    portMapping localRequiredPorts;

    // Create ports required by the formula
    unsigned int portId = 1;
    bool portFound = false;
    for (auto pm : std::get<1>(bpm)) {

      portMapping::iterator itl = localRequiredPorts.find(std::get<0>(pm));
      if (itl == localRequiredPorts.end()) {
        // Create port inside block
        testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_IN" + formulaName + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + formulaName + "/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_IN" + formulaName + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        localRequiredPorts[std::get<0>(pm)] = portId;
        portFound = true;
      }
      // And line inside block
      createLine(SRC_INFO, "TEST_ROOT_IN" + formulaName + pm.first, std::get<0>(bpm), formulaName, 1, pm.second);

      portMapping::iterator it = requiredPorts.find(std::get<0>(pm));
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_IN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_IN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (mainPortId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (mainPortId - 1) + 20 + 20) + "]);");

        requiredPorts[std::get<0>(pm)] = mainPortId;
        mainPortId++;
      }

      // And line outside block
      if (portFound) {
        portFound = false;

        createLine(SRC_INFO, "TEST_ROOT_IN" + pm.first, formulaName, "ROOT", 1, portId);
        portId++;
      }
    }

    for (auto pm : std::get<1>(bpm2)) {

      portMapping::iterator itl = localRequiredPorts.find(std::get<0>(pm));
      if (itl == localRequiredPorts.end()) {
        // Create port inside block
        testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_IN" + formulaName + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + formulaName + "/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_IN" + formulaName + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        localRequiredPorts[std::get<0>(pm)] = portId;
        portFound = true;
      }
      // And line inside block
      createLine(SRC_INFO, "TEST_ROOT_IN" + formulaName + pm.first, std::get<0>(bpm2), formulaName, 1, pm.second);

      portMapping::iterator it = requiredPorts.find(std::get<0>(pm));
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(SRC_INFO_TEMP, "TEST_ROOT_IN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        testBlockAppendLn(SRC_INFO_TEMP, "set_param(TEST_ROOT_IN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (mainPortId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (mainPortId - 1) + 20 + 20) + "]);");

        requiredPorts[std::get<0>(pm)] = mainPortId;
        mainPortId++;
      }

      // And line outside block
      if (portFound) {
        portFound = false;

        createLine(SRC_INFO, "TEST_ROOT_IN" + pm.first, formulaName, "ROOT", 1, portId);
        portId++;
      }
    }

    counter++;
  }
  */
}

blockPortMapping STLdriver::createSTLFormulaBody(LogicalOperation *l, const std::string &parent, unsigned int vpos)
{
  unsigned int y = 40 * vpos + 20;
  portMapping requiredPorts;
  //static unsigned int STLFormula_num = 0;
  //std::string STLFormula_name = "STLFormulaSub_" + std::to_string(STLFormula_num++);
  blockPortMapping A;
  blockPortMapping B;

  std::string STLFormula_name = createEmptyBlock(SRC_INFO, parent, position_X_EXP[0], position_X_EXP[1], y, y + 20);

  testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OUT = add_block('simulink/Sinks/Out1', [" + STLFormula_name + " '/OUT']);");
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

    testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + STLFormula_name + " '/OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'Operator', '" + relOp + "');");

    A = createExpression(l->value->a, STLFormula_name, 0);
    B = createExpression(l->value->b, STLFormula_name, 1);

    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

    createLine(SRC_INFO, STLFormula_name + "_OP", STLFormula_name + "_OUT", STLFormula_name);
    createLine(SRC_INFO, std::get<0>(A), STLFormula_name + "_OP", STLFormula_name, 1, 1);
    createLine(SRC_INFO, std::get<0>(B), STLFormula_name + "_OP", STLFormula_name, 1, 2);
  } else if (l->op == ISSTEP) {

    A = createExpression(l->arg1, STLFormula_name, 0);
    B = createExpression(l->arg2, STLFormula_name, 1);

    std::string is = createIsStepBlock(SRC_INFO, STLFormula_name, position_X_OP[0], position_X_OP[1], 20, 80);

    createLine(SRC_INFO, is, STLFormula_name + "_OUT", STLFormula_name);
    createLine(SRC_INFO, std::get<0>(A), is, STLFormula_name, 1, 1);
    createLine(SRC_INFO, std::get<0>(B), is, STLFormula_name, 1, 2);

  } else {// AND, OR
    //////////////////////////
    // Create logical block //
    //////////////////////////

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

    testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + STLFormula_name + " '/OP']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'Operator', '" + logOp + "');");

    A = createSTLFormulaBody(l->a, STLFormula_name, 0);
    B = createSTLFormulaBody(l->b, STLFormula_name, 1);

    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

    createLine(SRC_INFO, STLFormula_name + "_OP", STLFormula_name + "_OUT", STLFormula_name);
    createLine(SRC_INFO, std::get<0>(A), STLFormula_name + "_OP", STLFormula_name, 1, 1);
    createLine(SRC_INFO, std::get<0>(B), STLFormula_name + "_OP", STLFormula_name, 1, 2);
  }


  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;
  for (auto pm : std::get<1>(A)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + STLFormula_name + " '/" + pm.first + "']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    createLine(SRC_INFO, STLFormula_name + pm.first, std::get<0>(A), STLFormula_name, 1, pm.second);
  }

  for (auto pm : std::get<1>(B)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(SRC_INFO_TEMP, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + STLFormula_name + " '/" + pm.first + "']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    createLine(SRC_INFO, STLFormula_name + pm.first, std::get<0>(B), STLFormula_name, 1, pm.second);
  }

  return std::make_tuple(STLFormula_name, requiredPorts);
}

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

void STLdriver::addSTLFormula(TreeNode *f)
{
  nodes.push_back(f);
}
