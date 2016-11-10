#include <STLdriver.h>
#include <STLparser.hh>

#include <algorithm>

const std::string ADD_LINE_AUTOROUTING = ", 'autorouting','on'";

const unsigned int position_X_IN[2] = {20, 40};
const unsigned int position_X_EXP[2] = {80, 150};
const unsigned int position_X_OP[2] = {190, 210};
const unsigned int position_X_OUT[2] = {230, 250};

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

void STLdriver::testBlockAppendLn(const std::string fileName, const std::string &functionName, int lineNumber, const std::string &s)
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

void STLdriver::testBlockRoutingAppendLn(const std::string fileName, const std::string &functionName, int lineNumber, const std::string &s)
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

blockPortMapping STLdriver::createExpression(MathOperation * e,
                                             std::string parent,
                                             unsigned int y,
                                             std::string BLOCK_ROOT)
{
  unsigned int vpos;
  static unsigned int exp_num = 0;
  portMapping requiredPorts;
  std::string exp_name = "Exp_" + std::to_string(exp_num++);

  vpos = 40 * y + 20;

  testBlockAppendLn(__FILE__, __func__, __LINE__, exp_name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], '" + exp_name + "');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + exp_name + ",'position',[" + std::to_string(position_X_EXP[0]) + ", " + std::to_string(vpos) + ", " + std::to_string(position_X_EXP[1]) + ", " + std::to_string(vpos + 20) + "]);");

  testBlockAppendLn(__FILE__, __func__, __LINE__, exp_name + "_OUT = add_block('simulink/Sinks/Out1', [" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "/OUT']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + exp_name + "_OUT,'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40])");

  if (e->op == CONST || e->op == PORT) {
    // Create block containing input port or constant values

    if (e->op == CONST) {
      testBlockAppendLn(__FILE__, __func__, __LINE__, exp_name + "_IN = addConst([" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "'], '" + exp_name + "', '" + e->value + "');");
    } else if (e->op == PORT) {
      std::string portName = e->value;

      testBlockAppendLn(__FILE__, __func__, __LINE__, exp_name + "_IN = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "/" + portName + "']);");

      requiredPorts[portName] = 1;
    }

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + exp_name + "_IN, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + exp_name + "_OUT, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + exp_name + "_IN,'position', [" + std::to_string(position_X_IN[0]) + ", 20, " + std::to_string(position_X_IN[1]) + ", 40]);");
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

    testBlockAppendLn(__FILE__, __func__, __LINE__, exp_name + "_OP = add_block('simulink/Math Operations/" + matOp + "', [" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "/" + exp_name + "_OP']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + exp_name + "_OP,'position',[" + std::to_string(position_X_OP[0]) + ", 20, " + std::to_string(position_X_OP[1]) + ", 40]);");

    ///////////////////////////
    /// Generate expressions //
    ///////////////////////////

    blockPortMapping A = createExpression(e->a, parent + "/" + exp_name, 0, BLOCK_ROOT);
    blockPortMapping B = createExpression(e->b, parent + "/" + exp_name, 1, BLOCK_ROOT);

    /////////////////////////////////////
    /// Create output port connections //
    /////////////////////////////////////

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + exp_name + "_OP, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + exp_name + "_OUT, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort2 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + exp_name + "_OP, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "'], OutPort2.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");

    /////////////////////////
    /// Create input ports //
    /////////////////////////

    unsigned int portId = 1;

    for (auto pm : std::get<1>(A)) {
      portMapping::iterator it = requiredPorts.find(pm.first);
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(__FILE__, __func__, __LINE__, exp_name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "/" + pm.first + "']);");
        testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + exp_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        requiredPorts[pm.first] = portId;
        portId++;
      }

      // And line generated
      testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + exp_name + pm.first + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
    }

    for (auto pm : std::get<1>(B)) {
      portMapping::iterator it = requiredPorts.find(pm.first);
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(__FILE__, __func__, __LINE__, exp_name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "/" + pm.first + "']);");
        testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + exp_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        requiredPorts[pm.first] = portId;
        portId++;
      }

      // And line generated
      testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + exp_name + pm.first + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + exp_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
    }
  }

  return std::make_tuple(exp_name, requiredPorts);
}

void STLdriver::createSTLFormulaTimeInterval(const TimeInterval &time, std::string parent, std::string BLOCK_ROOT)
{
  std::string block_name = parent + "_TIME_RANGE";

  // Create empty container
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], 'TIME_RANGE');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + ", 'position', [" + std::to_string(position_X_EXP[0])+ ", 60, " + std::to_string(position_X_EXP[1])+ ", 80])");

  // Create internals

  // Output port
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + "_OUT = add_block('simulink/Sinks/Out1', [" + block_name + " '/OUT']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_OUT, 'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

  // Clock
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + "_CLOCK = add_block('simulink/Sources/Clock', [" + block_name + " '/CLOCK']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_CLOCK, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");

  // Interval checker
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + "_CHECK = add_block('simulink/Logic and Bit Operations/Interval Test', [" + block_name + " '/CHECK']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_CHECK, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_CHECK, 'uplimit', '" + time.end + "');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_CHECK, 'lowlimit', '" + time.start + "');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_CHECK, 'IntervalClosedLeft', '" + time.startClosed + "');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_CHECK, 'IntervalClosedRight', '" + time.endClosed + "');");

  // Connect internals

  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + block_name + "_CLOCK, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + block_name + "_CHECK, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(" + block_name + ", OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + block_name + "_CHECK, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + block_name + "_OUT, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(" + block_name + ", OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
}

void STLdriver::createSTLFormulaTemporalOperator(TemporalOperator op, std::string parent, std::string BLOCK_ROOT)
{
  std::string block_name = parent + "_TEMP_OP";

  // Create empty container
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], 'TEMP_OP');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + ", 'position', [" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40])");

  // Create output port
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + "_OUT = add_block('simulink/Sinks/Out1', [" + block_name + " '/OUT']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_OUT, 'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

  // Create input ports
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + "_STL = add_block('simulink/Sources/In1', [" + block_name + " '/STL']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_STL, 'position',[" + std::to_string(position_X_IN[0])+ ", 20, " + std::to_string(position_X_IN[1])+ ", 40]);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, block_name + "_TIME = add_block('simulink/Sources/In1', [" + block_name + " '/TIME']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + block_name + "_TIME, 'position',[" + std::to_string(position_X_IN[0])+ ", 60, " + std::to_string(position_X_IN[1])+ ", 80]);");

  switch (op) {
    case ALWAYS:
      break;
    case EVENTUALLY:
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
    std::string formulaName = "STLFormula_" + std::to_string(counter);

    testBlockAppendLn(__FILE__, __func__, __LINE__, formulaName + " = addEmptySubsystem(ROOT, '" + formulaName + "');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + formulaName + ",'position',[" + std::to_string(position_X_EXP[0])+ ", " + std::to_string(portOffset * counter + 20) + ", " + std::to_string(position_X_EXP[1])+ ", " + std::to_string(portOffset * counter + 20 + 20) + "]);");

    blockPortMapping bpm = createSTLFormulaBody(std::get<2>(ll), formulaName, 0, "ROOT");

    testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_OUT" + std::get<0>(bpm) + " = add_block('simulink/Sinks/Out1', [ROOT '/" + formulaName + "/VALID']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_OUT" + std::get<0>(bpm) + ",'position',[" + std::to_string(position_X_OUT[0]) + ", 20, " + std::to_string(position_X_OUT[1]) + ", 40]);");


    testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_ASSERTION_" + std::get<0>(bpm) + " = add_block('simulink/Model Verification/Assertion', [ROOT '/VALID_" + std::to_string(counter) + "']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_ASSERTION_" + std::get<0>(bpm) + ",'position',[" + std::to_string(position_X_OUT[0])+ ", " + std::to_string(portOffset * counter + 20) + ", " + std::to_string(position_X_OUT[1])+ ", " + std::to_string(portOffset * counter + 20 + 20) + "]);");

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + formulaName + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(TEST_ROOT_ASSERTION_" + std::get<0>(bpm) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

    createSTLFormulaTimeInterval(std::get<1>(ll), formulaName, "ROOT");
    createSTLFormulaTemporalOperator(std::get<0>(ll), formulaName, "ROOT");

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + formulaName + "_TEMP_OP, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(TEST_ROOT_OUT" + std::get<0>(bpm) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([ROOT '/" + formulaName + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + std::get<0>(bpm) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + formulaName + "_TEMP_OP, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([ROOT '/" + formulaName + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + formulaName + "_TIME_RANGE, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + formulaName + "_TEMP_OP, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([ROOT '/" + formulaName + "'], OutPort1.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");

    // Create ports required by the formula
    unsigned int portId = 1;
    for (auto pm : std::get<1>(bpm)) {

      // Create port inside block
      testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_IN" + formulaName + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + formulaName + "/" + pm.first + "']);");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_IN" + formulaName + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      // And line inside block
      testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(TEST_ROOT_IN" + formulaName + pm.first + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(bpm) + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([ROOT '/" + formulaName + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");

      portMapping::iterator it = requiredPorts.find(std::get<0>(pm));
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_IN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_IN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        //testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "MAIN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        //testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "set_param(MAIN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * portId + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * portId + 20 + 20) + "]);");

        //testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(MAIN" + pm.first + ", 'PortHandles');");
        //testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(TEST, 'PortHandles');");
        //testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport( " + std::to_string(portId) + " )" + ADD_LINE_AUTOROUTING + ");");

        requiredPorts[std::get<0>(pm)] = portId;
        portId++;
      }

      // And line outside block
      testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(TEST_ROOT_IN" + pm.first + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + formulaName + ", 'PortHandles');");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
    }

    counter++;
  }

#if 1
  /*
  portMapping requiredPorts;

  testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_VALID = add_block('simulink/Sinks/Out1', [l" + TEST_ROOT + " '/VALID']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_VALID,'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_OUT_PORT = get_param(TEST_ROOT_VALID, 'PortHandles');");

  unsigned int portId = 1;
  //unsigned int counter = 1;

  for (auto ll : STLFormulas) {
    for (auto pm : std::get<1>(ll)) {
      portMapping::iterator it = requiredPorts.find(std::get<0>(pm));
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT" + pm.first + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '/" + pm.first + "']);");
        testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "MAIN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "set_param(MAIN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * portId + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * portId + 20 + 20) + "]);");

        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(MAIN" + pm.first + ", 'PortHandles');");
        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(TEST, 'PortHandles');");
        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport( " + std::to_string(portId) + " )" + ADD_LINE_AUTOROUTING + ");");

        requiredPorts[std::get<0>(pm)] = portId;
        portId++;
      }

      // And line generated
      testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(TEST_ROOT" + pm.first + ", 'PortHandles');");
      testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(ll) + ", 'PortHandles');");
      testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
    }

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + std::get<0>(ll) + ", 'PortHandles');");

    //testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort1.Outport(1), TEST_ROOT_OUT_PORT.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

  }
  */
#else
  // Old, wrong implementation
  portMapping requiredPorts;

  testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_VALID = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '/VALID']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_VALID,'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_OUT_PORT = get_param(TEST_ROOT_VALID, 'PortHandles');");

  if (STLFormulas.size() > 1) {
    testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + TEST_ROOT + " '/OP']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT_OP,'Inputs', '" + std::to_string(STLFormulas.size()) + "');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT_OP_PORT = get_param(TEST_ROOT_OP, 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(" + TEST_ROOT + ", TEST_ROOT_OP_PORT.Outport(1), TEST_ROOT_OUT_PORT.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
  }

  unsigned int portId = 1;
  unsigned int counter = 1;

  for (auto ll : STLFormulas) {
    for (auto pm : std::get<1>(ll)) {
      portMapping::iterator it = requiredPorts.find(std::get<0>(pm));
      if (it == requiredPorts.end()) {
        // Port needs to be created

        testBlockAppendLn(__FILE__, __func__, __LINE__, "TEST_ROOT" + pm.first + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '/" + pm.first + "']);");
        testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(TEST_ROOT" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "MAIN" + pm.first + " = add_block('simulink/Sources/In1', [ROOT '/" + pm.first + "']);");
        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "set_param(MAIN" + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * portId + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * portId + 20 + 20) + "]);");

        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(MAIN" + pm.first + ", 'PortHandles');");
        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(TEST, 'PortHandles');");
        testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport( " + std::to_string(portId) + " )" + ADD_LINE_AUTOROUTING + ");");

        requiredPorts[std::get<0>(pm)] = portId;
        portId++;
      }

      // And line generated
      testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(TEST_ROOT" + pm.first + ", 'PortHandles');");
      testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(ll) + ", 'PortHandles');");
      testBlockRoutingAppendLn(__FILE__, __func__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
    }

    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + std::get<0>(ll) + ", 'PortHandles');");

    if (STLFormulas.size() == 1) {
      testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort1.Outport(1), TEST_ROOT_OUT_PORT.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    } else {
      // Moving STLformula
      testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + std::get<0>(ll) + ",'position',[" + std::to_string(position_X_EXP[0]) + ", " + std::to_string(portOffset * (counter - 1) + 20) + ", " + std::to_string(position_X_EXP[1]) + ", " + std::to_string(portOffset * (counter - 1) + 20 + 20) + "]);");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort1.Outport(1), TEST_ROOT_OP_PORT.Inport(" + std::to_string(counter) + ")" + ADD_LINE_AUTOROUTING + ");");
      counter++;
    }
  }
#endif
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

  testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + " = addEmptySubsystem([" + BLOCK_ROOT + " '/" + parent + "'], '" + STLFormula_name + "');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + ",'position',[" + std::to_string(position_X_EXP[0])+ ", " + std::to_string(vpos) + ", " + std::to_string(position_X_EXP[1])+ ", " + std::to_string(vpos + 20) + "])");

  testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + "_OUT = add_block('simulink/Sinks/Out1', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/OUT']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OUT,'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

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

    testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/" + STLFormula_name + "_OP']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OP,'Operator', '" + relOp + "');");

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

    testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/OP']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OP,'Operator', '" + logOp + "');");

    A = createSTLFormulaBody(l->a, parent + "/" + STLFormula_name, 0, BLOCK_ROOT);
    B = createSTLFormulaBody(l->b, parent + "/" + STLFormula_name, 1, BLOCK_ROOT);
  }
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + STLFormula_name + "_OP, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + STLFormula_name + "_OUT, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort2 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + STLFormula_name + "_OP, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "'], OutPort2.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;

  for (auto pm : std::get<1>(A)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + STLFormula_name + pm.first + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
  }

  for (auto pm : std::get<1>(B)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + STLFormula_name + pm.first + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + BLOCK_ROOT + " '/" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
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

  testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + " = addEmptySubsystem([" + TEST_ROOT + " '" + parent + "'], '" + STLFormula_name + "');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + ",'position',[" + std::to_string(position_X_EXP[0])+ ", " + std::to_string(vpos) + ", " + std::to_string(position_X_EXP[1])+ ", " + std::to_string(vpos + 20) + "])");

  testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/OUT']);");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OUT,'position',[" + std::to_string(position_X_OUT[0])+ ", 20, " + std::to_string(position_X_OUT[1])+ ", 40]);");

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

    testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/" + STLFormula_name + "_OP']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OP,'Operator', '" + relOp + "');");

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

    testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/OP']);");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OP,'Operator', '" + logOp + "');");

    A = createSTLFormulaBody(l->a, parent + "/" + STLFormula_name, 0);
    B = createSTLFormulaBody(l->b, parent + "/" + STLFormula_name, 1);
  }
  testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + "_OP,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + STLFormula_name + "_OP, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + STLFormula_name + "_OUT, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort2 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + STLFormula_name + "_OP, 'PortHandles');");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
  testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort2.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");

  /////////////////////////
  /// Create input ports //
  /////////////////////////

  unsigned int portId = 1;

  for (auto pm : std::get<1>(A)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + STLFormula_name + pm.first + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(A) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
  }

  for (auto pm : std::get<1>(B)) {
    portMapping::iterator it = requiredPorts.find(pm.first);
    if (it == requiredPorts.end()) {
      // Port needs to be created

      testBlockAppendLn(__FILE__, __func__, __LINE__, STLFormula_name + pm.first + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "/" + pm.first + "']);");
      testBlockAppendLn(__FILE__, __func__, __LINE__, "set_param(" + STLFormula_name + pm.first + ",'position',[" + std::to_string(position_X_IN[0]) + ", " + std::to_string(portOffset * (portId - 1) + 20) + ", " + std::to_string(position_X_IN[1]) + ", " + std::to_string(portOffset * (portId - 1) + 20 + 20) + "]);");

      requiredPorts[pm.first] = portId;
      portId++;
    }

    // And line generated
    testBlockAppendLn(__FILE__, __func__, __LINE__, "OutPort1 = get_param(" + STLFormula_name + pm.first + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "InPort1 = get_param(" + std::get<0>(B) + ", 'PortHandles');");
    testBlockAppendLn(__FILE__, __func__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + STLFormula_name + "'], OutPort1.Outport(1), InPort1.Inport(" + std::to_string(pm.second) + ")" + ADD_LINE_AUTOROUTING + ");");
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
