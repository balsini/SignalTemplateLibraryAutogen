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
  SIG_input(false)
{
  this->path = path;

  testBlockAppendFile.open(path + "AUTOGEN_testBlock.m", std::ofstream::out);
  if (!testBlockAppendFile.is_open())
    throw "Error opening AUTOGEN_testBlock.m";
}

STLdriver::~STLdriver()
{
  testBlockAppendFile.close();

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
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_ENABLE, 'position',[20, 60, 40, 80]);");
  testBlockAppendLn(SRC_INFO_TEMP, name + "_EOI = add_block('simulink/Sources/In1', [" + name + " '/EOI']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_EOI, 'position',[20, 20, 40, 40]);");
  testBlockAppendLn(SRC_INFO_TEMP, name + "_FIRST = add_block('simulink/Sources/In1', [" + name + " '/FIRST']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_FIRST, 'position',[20, 140, 40, 160]);");
  testBlockAppendLn(SRC_INFO_TEMP, name + "_SECOND = add_block('simulink/Sources/In1', [" + name + " '/SECOND']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_SECOND, 'position',[20, 100, 40, 120]);");

  // Create output port
  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT, 'position',[380, 20, 400, 40]);");



  //
  //testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT, 'position',[380, 20, 400, 40]);");

  return name;
}

std::string STLdriver::createTimeInterval(const TimeInterval &time, const std::string &parent, bool withDelay)
{
  int x_pos_start = position_X_EXP[0];
  int x_pos_end = position_X_EXP[1];

  if (withDelay)
    x_pos_start += (position_X_EXP[1] - position_X_EXP[0]) / 2;

  // Create empty container
  std::string name = createEmptyBlock(SRC_INFO, parent, x_pos_start, x_pos_end, 20, 40);

  // Create internals

  // Output port
  testBlockAppendLn(SRC_INFO_TEMP, name + "_OUT = add_block('simulink/Sinks/Out1', [" + name + " '/OUT']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_OUT, 'position',[320, 20, 340, 40]);");

  // Clock
  testBlockAppendLn(SRC_INFO_TEMP, name + "_CLOCK = add_block('simulink/Sources/Clock', [" + name + " '/CLOCK']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CLOCK, 'position',[" + std::to_string(position_X_IN[0]) + ", 60, " + std::to_string(position_X_IN[1]) + ", 80]);");

  if (withDelay) {
    // Dealy input port
    testBlockAppendLn(SRC_INFO_TEMP, name + "_DELAY = add_block('simulink/Sources/In1', [" + name + " '/DELAY']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_DELAY, 'position',[" + std::to_string(position_X_IN[0])+ ", 100, " + std::to_string(position_X_IN[1]) + ", 120]);");
    // Sub
    testBlockAppendLn(SRC_INFO_TEMP, name + "_DELAY_SUB = add_block('simulink/Math Operations/Subtract', [" + name + " '/DELAY_SUB']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_DELAY_SUB,'position',[" + std::to_string(position_X_IN[0] + 60) + ", 60, " + std::to_string(position_X_IN[1] + 60) + ", 80]);");;
  }

  testBlockAppendLn(SRC_INFO_TEMP, name + "_START = add_block('simulink/Sources/Constant', [" + name + " '/START']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_START, 'Value', '" + time.start + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_START, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_END = add_block('simulink/Sources/Constant', [" + name + " '/END']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_END, 'Value', '" + time.end + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_END, 'position',[" + std::to_string(position_X_EXP[0])+ ", 100, " + std::to_string(position_X_EXP[1])+ ", 120]);");

  testBlockAppendLn(SRC_INFO_TEMP, name + "_REL1 = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + name + " '/REL1']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_REL1,'position',[" + std::to_string(position_X_OP[0])+ ", 20, " + std::to_string(position_X_OP[1])+ ", 40]);");

  if (time.startClosed)
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_REL1,'Operator', '<=');");
  else
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_REL1,'Operator', '<');");

  createLine(SRC_INFO, name + "_START", name + "_REL1", name);

  testBlockAppendLn(SRC_INFO_TEMP, name + "_REL2 = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + name + " '/REL2']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_REL2,'position',[" + std::to_string(position_X_OP[0])+ ", 100, " + std::to_string(position_X_OP[1])+ ", 120]);");

  if (time.endClosed)
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_REL2,'Operator', '<=');");
  else
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_REL2,'Operator', '<');");

  if (withDelay) {
    createLine(SRC_INFO, name + "_DELAY", name + "_DELAY_SUB", name, 1, 2);
    createLine(SRC_INFO, name + "_CLOCK", name + "_DELAY_SUB", name, 1, 1);

    createLine(SRC_INFO, name + "_DELAY_SUB", name + "_REL1", name, 1, 2);
    createLine(SRC_INFO, name + "_DELAY_SUB", name + "_REL2", name);
  } else {
    createLine(SRC_INFO, name + "_CLOCK", name + "_REL1", name, 1, 2);
    createLine(SRC_INFO, name + "_CLOCK", name + "_REL2", name);
  }

  createLine(SRC_INFO, name + "_END", name + "_REL2", name, 1, 2);

  testBlockAppendLn(SRC_INFO_TEMP, name + "_AND = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/AND']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_AND, 'Operator', '" + "AND" + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_AND, 'position',[260, 20, 280, 40]);");

  createLine(SRC_INFO, name + "_REL1", name + "_AND", name);
  createLine(SRC_INFO, name + "_REL2", name + "_AND", name, 1, 2);
  createLine(SRC_INFO, name + "_AND", name + "_OUT", name);

  // Interval checker
  /*
  testBlockAppendLn(SRC_INFO_TEMP, name + "_CHECK = add_block('simulink/Logic and Bit Operations/Interval Test', [" + name + " '/CHECK']);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'position',[" + std::to_string(position_X_EXP[0])+ ", 20, " + std::to_string(position_X_EXP[1])+ ", 40]);");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'uplimit', '" + time.end + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'lowlimit', '" + time.start + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'IntervalClosedLeft', '" + time.startClosed + "');");
  testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_CHECK, 'IntervalClosedRight', '" + time.endClosed + "');");

  // Connect internals
  createLine(SRC_INFO, name + "_CLOCK", name + "_CHECK", name);
  createLine(SRC_INFO, name + "_CHECK", name + "_OUT", name);
  */

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

      testBlockAppendLn(SRC_INFO_TEMP, name + "_ALWAYS = add_block('STLLib/Always', [" + name + " '/ALWAYS']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_ALWAYS, 'position',[" + std::to_string(position_X_OP[0]) + ", 20, " + std::to_string(position_X_OP[1]) + ", 160]);");

      createLine(SRC_INFO, name + "_ALWAYS", name + "_OUT", name);
      createLine(SRC_INFO, name + "_TIME", name + "_ALWAYS", name, 1, 2);
      createLine(SRC_INFO, name + "_STL", name + "_ALWAYS", name);

      break;
    case EVENTUALLY:

      testBlockAppendLn(SRC_INFO_TEMP, name + "_EVENTUALLY = add_block('STLLib/Eventually', [" + name + " '/EVENTUALLY']);");
      testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + name + "_EVENTUALLY, 'position',[" + std::to_string(position_X_OP[0]) + ", 20, " + std::to_string(position_X_OP[1]) + ", 160]);");

      createLine(SRC_INFO, name + "_EVENTUALLY", name + "_OUT", name);
      createLine(SRC_INFO, name + "_TIME", name + "_EVENTUALLY", name, 1, 2);
      createLine(SRC_INFO, name + "_STL", name + "_EVENTUALLY", name);

      break;
    default:
      break;
  }
  return name;
}

blockPortMapping STLdriver::createSTLFormulas()
{
  portMapping bpm;
  unsigned int portId = 1;
  std::string name = "ROOT";

  unsigned int counter = 0;
  for (TreeNode * n : nodes) {
    std::cout << std::endl << "Predicate_" << std::to_string(counter) << std::endl;

    std::string assertName = n->getName() + std::to_string(counter);

    blockPortMapping bm = n->generate(this, name, counter);

    testBlockAppendLn(SRC_INFO_TEMP, std::get<0>(bm) + "_NEG_ASSERT = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + name + " '/NOT_" + assertName + "']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + std::get<0>(bm) + "_NEG_ASSERT, 'Operator', '" + "NOT" + "');");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + std::get<0>(bm) + "_NEG_ASSERT,'position',[" + std::to_string(position_X_OUT[0] - 60)+ ", " + std::to_string(portOffset * counter + 20) + ", " + std::to_string(position_X_OUT[0] - 40) + ", " + std::to_string(portOffset * counter + 20 + 20) + "]);");

    testBlockAppendLn(SRC_INFO_TEMP, std::get<0>(bm) + "_ASSERT = add_block('simulink/Model Verification/Assertion', [" + name + " '/VALID_" + assertName + "']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + std::get<0>(bm) + "_ASSERT,'position',[" + std::to_string(position_X_OUT[0])+ ", " + std::to_string(portOffset * counter + 20) + ", " + std::to_string(position_X_OUT[1])+ ", " + std::to_string(portOffset * counter + 20 + 20) + "]);");

    createLine(SRC_INFO, std::get<0>(bm) + "_NEG_ASSERT", std::get<0>(bm) + "_ASSERT", name);
    createLine(SRC_INFO, std::get<0>(bm), std::get<0>(bm) + "_NEG_ASSERT", name);

    updateRequiredPorts(this, name, bpm, bm, portId);

    counter++;
  }

  return std::make_tuple(name, bpm);
}

void STLdriver::linkSTLFormulas(const blockPortMapping &bpm)
{
  unsigned int counter = 0;
  std::string name = "ROOT";
  unsigned int number_of_ports = std::get<1>(bpm).size();

  std::cout << "Generating STLFormulas links to model" << std::endl;

  testBlockAppendLn(SRC_INFO_TEMP, "BLOCKS = find_system(SYSTEM,'SearchDepth',1);");
  testBlockAppendLn(SRC_INFO_TEMP, "SRC_PORT_REL_POSITIONING = [20 -30 35 -15];");

  std::string portHeight = "((ROOT_POSITION(4) - ROOT_POSITION(2)) / " + std::to_string(number_of_ports) + ")";

  for (auto p : std::get<1>(bpm)) {
    std::cout << " - " << p.first << std::endl;

    testBlockAppendLn(SRC_INFO_TEMP, p.first + "_SRC = add_block('simulink/Signal Routing/Goto', [SYSTEM '/" + p.first + "_SRC']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + p.first + "_SRC,'GotoTag','" + p.first + "');");
    testBlockAppendLn(SRC_INFO_TEMP, p.first + "_SRC_PORT = getSourcePortHandleOfSignal(SYSTEM, BLOCKS, '" + p.first + "');");
    testBlockAppendLn(SRC_INFO_TEMP, p.first + "_SRC_POS = get_param(" + p.first + "_SRC_PORT, 'position');");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + p.first + "_SRC,'position',["
                      + p.first + "_SRC_POS(1) + SRC_PORT_REL_POSITIONING(1) , "
                      + p.first + "_SRC_POS(2) + SRC_PORT_REL_POSITIONING(2) , "
                      + p.first + "_SRC_POS(1) + SRC_PORT_REL_POSITIONING(3) , "
                      + p.first + "_SRC_POS(2) + SRC_PORT_REL_POSITIONING(4)"
                      + "]);");

    testBlockAppendLn(SRC_INFO_TEMP, "InPort1 = get_param(" + p.first + "_SRC, 'PortHandles');");
    testBlockAppendLn(SRC_INFO_TEMP, "add_line(SYSTEM, " + p.first + "_SRC_PORT, InPort1.Inport(1), 'autorouting','on');");

    std::string portHeight0 = "(" + portHeight + " * " + std::to_string(p.second - 1) + ")";
    std::string portHeight1 = "(" + portHeight + " * " + std::to_string(p.second) + ")";

    testBlockAppendLn(SRC_INFO_TEMP, p.first + "_DST = add_block('simulink/Signal Routing/From', [SYSTEM '/" + p.first + "_DST']);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + p.first + "_DST,'position',[ROOT_POSITION(1) - 60 , ROOT_POSITION(2) + " + portHeight0 + " ROOT_POSITION(1) - 30 ROOT_POSITION(2) + " + portHeight1 + "]);");
    testBlockAppendLn(SRC_INFO_TEMP, "set_param(" + p.first + "_DST,'GotoTag','" + p.first + "');");

    createLine(SRC_INFO, p.first + "_DST", std::get<0>(bpm), "SYSTEM", 1, p.second);

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

void STLdriver::addSTLFormula(TreeNode *f, const std::string &name)
{
  f->setName(name);
  nodes.push_back(f);
}
