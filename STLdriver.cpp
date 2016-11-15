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

void STLdriver::testBlockAppendLn(const std::string &fileName,
                                  const std::string &functionName,
                                  int lineNumber,
                                  const std::string &s)
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

void STLdriver::testBlockRoutingAppendLn(const std::string &fileName,
                                         const std::string &functionName,
                                         int lineNumber,
                                         const std::string &s)
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

std::string STLdriver::createIsStepBlock(srcInfo code,
                                         const std::string &parent,
                                         unsigned int x1,
                                         unsigned int x2,
                                         unsigned int y1,
                                         unsigned int y2)
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

std::string STLdriver::createEmptyBlock(srcInfo code,
                                        const std::string &parent,
                                        unsigned int x1,
                                        unsigned int x2,
                                        unsigned int y1,
                                        unsigned int y2)
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

std::string STLdriver::createTimeInterval(const TimeInterval &time, const std::string &parent)
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

void STLdriver::createSTLFormulas()
{
  portMapping bpm;
  unsigned int portId = 1;

  unsigned int counter = 0;
  for (TreeNode * n : nodes) {
    std::string name = "Predicate_" + std::to_string(counter);
    std::cout << std::endl << name << std::endl;

    blockPortMapping bm = n->generate(this, "ROOT", counter);

    updateRequiredPorts(this, "ROOT", bpm, bm, portId);

    counter++;
  }
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
