#include <STLdriver.h>
#include <STLparser.hh>

const std::string TEST_ROOT = "TEST";
const std::string ADD_LINE_AUTOROUTING = ", 'autorouting','on'";

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
  testBlockRoutingAppendFile.close();
  systemPortsFile.close();
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

void STLdriver::testBlockAppendLn(const std::string fileName, int lineNumber, const std::string &s)
{
  fileAppend("% " + fileName + "::" + std::to_string(lineNumber), testBlockAppendFile);
  //std::cout << std::endl;
  testBlockAppendFile << std::endl;

  fileAppend(s, testBlockAppendFile);
  //std::cout << std::endl;
  testBlockAppendFile << std::endl;

  //std::cout << std::endl;
  testBlockAppendFile << std::endl;
}

void STLdriver::testBlockRoutingAppendLn(const std::string fileName, int lineNumber, const std::string &s)
{
  fileAppend("% " + fileName + "::" + std::to_string(lineNumber), testBlockRoutingAppendFile);
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

void createConstantBlock(std::string v)
{
  std::cout << "--|--|--|--|--|--) createConstantBlock" << std::endl;
}

void STLdriver::createSignalBlock()
{
  SIG_input = true;
  std::cout << "--|--|--|--|--|--) createSignalBlock" << std::endl;
}

void STLdriver::createReferenceBlock()
{
  REF_input = true;
  std::cout << "--|--|--|--|--|--) createReferenceBlock" << std::endl;
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

bool STLdriver::variableExists(std::string v) {
  return (variablesValues.find(v) != variablesValues.end());
}
void STLdriver::setVariable(std::string name, std::string value) {
  variablesValues[name] = value;
}
std::string STLdriver::getVariable(std::string name) {
  return variablesValues[name];
}

void STLdriver::setStatus(DriverStatus s)
{
  status = s;
}

void STLdriver::printConstantValues()
{
  /*
  static unsigned int vertical_position = 20;
  unsigned int vertical_offset = 20;
  unsigned int horizontal_position = 20;
  unsigned int horizontal_offset = 20;
  */
  std::cout << "--) printConstantValues()" << std::endl;

  typedef std::map<std::string, std::string>::iterator it_type;
  for(it_type it = variablesValues.begin(); it != variablesValues.end(); ++it) {
    std::cout << "  " << it->first << " = " << it->second << std::endl;
    /*
    testBlockAppendLn(it->first + " = addConst(PATH, '" + it->first + "', '" + it->second + "');");
    testBlockAppendLn("set_param(" + it->first + ", 'position', ["
             + std::to_string(horizontal_position) + ", "
             + std::to_string(vertical_position) + ", "
             + std::to_string(horizontal_position + horizontal_offset) + ", "
             + std::to_string(vertical_position + vertical_offset)
             + "]);");
    testBlockAppendLn("");
    vertical_position = vertical_position + (vertical_offset * 2);
    */
  }

  std::cout << std::endl << "-/--/--/--/--/--/--/--/--/-" << std::endl << std::endl;
}

std::tuple<std::string, unsigned int> STLdriver::createExpression(MathOperation * e,
                                                                  std::string parent,
                                                                  unsigned int x,
                                                                  unsigned int y)
{
  unsigned int vpos;
  unsigned int hpos;
  static unsigned int exp_num = 0;
  unsigned int portRequired = 0;
  std::string exp_name = "Exp_" + std::to_string(exp_num++);

  vpos = 40 * y + 20;
  hpos = 40 * x + 20;

  if (e->op == CONST || e->op == SIG || e->op == REF) {
    // Create block containing input port or constant values

    if (e->op == CONST) {
      testBlockAppendLn(__FILE__, __LINE__, exp_name + " = addConst([" + TEST_ROOT + " '" + parent + "'], '" + exp_name + "', '" + e->value + "');");

      //testBlockAppendLn("set_param(" + exp_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "])");
    } else {
      std::string portName;
      switch (e->op) {
        case SIG:
          portName = "SIG";
          portRequired |= SIG_PORT;
          break;
        case REF:
          portName = "REF";
          portRequired |= REF_PORT;
          break;
        default: break;
      }

      testBlockAppendLn(__FILE__, __LINE__, exp_name + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + portName + "']);");

      if (e->op == SIG)
        testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + ", 'port', '1');");
      else
        testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + ", 'port', '2');");

    }

    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + ",'position', [60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "]);");
  } else {// SUM, SUB, MUL, DIV
    // Create mathematical block

    testBlockAppendLn(__FILE__, __LINE__, exp_name + " = addEmptySubsystem([" + TEST_ROOT + " '" + parent + "'], '" + exp_name + "');");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "]);");

    testBlockAppendLn(__FILE__, __LINE__, exp_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + exp_name + "/" + exp_name + "_OUT']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "])");

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

    testBlockAppendLn(__FILE__, __LINE__, exp_name + "_OP = add_block('simulink/Math Operations/" + matOp + "', [" + TEST_ROOT + " '" + parent + "/" + exp_name + "/" + exp_name + "_OP']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + "_OP,'position',[140, " + std::to_string(vpos) + ", 160, " + std::to_string(vpos + 20) + "]);");

    std::tuple<std::string, unsigned int> A = createExpression(e->a, parent + "/" + exp_name, x + 1, y);
    std::tuple<std::string, unsigned int> B = createExpression(e->b, parent + "/" + exp_name, x + 1, y + 1);

    testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + std::get<0>(A) + ",'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort2 = get_param(" + std::get<0>(B) + ",'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort3 = get_param(" + exp_name + "_OP,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + exp_name + "_OUT,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "InPort2 = get_param(" + exp_name + "_OP,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + exp_name + "'], OutPort1.Outport(1), InPort2.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + exp_name + "'], OutPort2.Outport(1), InPort2.Inport(2)" + ADD_LINE_AUTOROUTING + ");");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + exp_name + "'], OutPort3.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

    portRequired |= std::get<1>(A);
    portRequired |= std::get<1>(B);

    if (portRequired & SIG_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, exp_name + "_SIG = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + "SIG']);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + "_SIG,'position',[20, 20, 40, 40]);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + "_SIG, 'port', '1');");
    }
    if (portRequired & REF_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, exp_name + "_REF = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + "REF']);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + "_REF,'position',[20, 60, 40, 80]);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + exp_name + "_REF, 'port', '2');");
    }

    if (std::get<1>(A) & SIG_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + exp_name + "_SIG,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + exp_name + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    }
    if (std::get<1>(A) & REF_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + exp_name + "_REF,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + exp_name + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "'], OutPort1.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");
    }
    if (std::get<1>(B) & SIG_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + exp_name + "_SIG,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + exp_name + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    }
    if (std::get<1>(B) & REF_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + exp_name + "_REF,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + exp_name + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "'], OutPort1.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");
    }
  }

  return std::make_tuple(exp_name, portRequired);
}


void STLdriver::connectAssertions(std::list<std::tuple<std::string, unsigned int> > l)
{
  unsigned int portRequired = 0;
  std::string SIG_PORT_NAME = "TEST_ROOT_SIG";
  std::string REF_PORT_NAME = "TEST_ROOT_REF";
  std::string OUT_PORT_NAME = "TEST_ROOT_VALID";

  testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + OUT_PORT_NAME + ",'PortHandles');");
  for (std::tuple<std::string, unsigned int> A : l) {
    portRequired |= std::get<1>(A);

    testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + std::get<0>(A) + ",'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
  }

  if (portRequired & SIG_PORT) {
    testBlockAppendLn(__FILE__, __LINE__, SIG_PORT_NAME + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '/SIG']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + SIG_PORT_NAME + ",'position',[20, 20, 40, 40]);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + SIG_PORT_NAME + ", 'port', '1');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + SIG_PORT_NAME + ",'PortHandles');");

    testBlockRoutingAppendLn(__FILE__, __LINE__, "SIG = add_block('simulink/Sources/In1', [ROOT '/SIG']);");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "set_param(SIG,'position',[column_margin(1), row_margin(2), column_margin(1) + 20, row_margin(2) + 20]);");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(SIG,'PortHandles');");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "InPort1 = get_param(TEST,'PortHandles');");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport(1));");
  }
  if (portRequired & REF_PORT) {
    testBlockAppendLn(__FILE__, __LINE__, REF_PORT_NAME + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '/REF']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + REF_PORT_NAME + ",'position',[20, 60, 40, 80]);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + REF_PORT_NAME + ", 'port', '2');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort2 = get_param(" + REF_PORT_NAME + ",'PortHandles');");

    testBlockRoutingAppendLn(__FILE__, __LINE__, "REF = add_block('simulink/Sources/In1', [ROOT '/REF']);");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "set_param(REF,'position',[column_margin(1), row_margin(3), column_margin(1) + 20, row_margin(3) + 20]);");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(REF,'PortHandles');");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "InPort1 = get_param(TEST,'PortHandles');");
    testBlockRoutingAppendLn(__FILE__, __LINE__, "add_line(ROOT, OutPort1.Outport(1), InPort1.Inport(2));");
  }

  for (std::tuple<std::string, unsigned int> A : l) {
    std::string ass_name = std::get<0>(A);

    if (std::get<1>(A) & SIG_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + std::get<0>(A) + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    }
    if (std::get<1>(A) & REF_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + std::get<0>(A) + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line(" + TEST_ROOT + ", OutPort2.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");
    }
  }
}

std::tuple<std::string, unsigned int> STLdriver::createAssertionBody(LogicalOperation *l, std::string parent, unsigned int x, unsigned int y)
{
  unsigned int vpos;
  unsigned int hpos;
  unsigned int portRequired = 0;
  static unsigned int ass_num = 0;
  std::string ass_name = "Ass_" + std::to_string(ass_num++);

  vpos = 40 * y + 20;
  hpos = 40 * x + 20;

  if (l->op == COMPARISON) {
    // Create block containing the TWO comparison expressions

    testBlockAppendLn(__FILE__, __LINE__, ass_name + " = addEmptySubsystem([" + TEST_ROOT + " '" + parent + "'], '" + ass_name + "');");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "])");

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

    testBlockAppendLn(__FILE__, __LINE__, ass_name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/" + ass_name + "_OP']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_OP,'position',[140, " + std::to_string(vpos) + ", 160, " + std::to_string(vpos + 20) + "]);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_OP,'Operator', '" + relOp + "');");

    std::tuple<std::string, unsigned int> A = createExpression(l->value->a, parent + "/" + ass_name, x + 1, y);
    std::tuple<std::string, unsigned int> B = createExpression(l->value->b, parent + "/" + ass_name, x + 1, y + 1);

    testBlockAppendLn(__FILE__, __LINE__, ass_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/" + ass_name + "_OUT']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "])");

    testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + ass_name + "_OP,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort2 = get_param(" + std::get<0>(A) + ",'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort3 = get_param(" + std::get<0>(B) + ",'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + ass_name + "_OUT,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "InPort2 = get_param(" + ass_name + "_OP,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort2.Outport(1), InPort2.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort3.Outport(1), InPort2.Inport(2)" + ADD_LINE_AUTOROUTING + ");");

    portRequired |= std::get<1>(A);
    portRequired |= std::get<1>(B);

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

    testBlockAppendLn(__FILE__, __LINE__, ass_name + " = addEmptySubsystem(" + par + ", '" + ass_name + "');");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "]);");

    std::tuple<std::string, unsigned int> A = createAssertionBody(l->a, parent + "/" + ass_name, x + 1, y);
    std::tuple<std::string, unsigned int> B = createAssertionBody(l->b, parent + "/" + ass_name, x + 1, y + 1);

    testBlockAppendLn(__FILE__, __LINE__, ass_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/" + ass_name + "_OUT']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "]);");

    testBlockAppendLn(__FILE__, __LINE__, ass_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/OP']);");
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_OP,'position',[140, " + std::to_string(vpos) + ", 160, " + std::to_string(vpos + 20) + "]);");

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
    testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_OP,'Operator', '" + logOp + "');");

    testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + std::get<0>(A) + ",'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort2 = get_param(" + std::get<0>(B) + ",'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "OutPort3 = get_param(" + ass_name + "_OP,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + ass_name + "_OUT,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "InPort2 = get_param(" + ass_name + "_OP,'PortHandles');");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort2.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort2.Outport(1), InPort2.Inport(2)" + ADD_LINE_AUTOROUTING + ");");
    testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort3.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");

    portRequired |= std::get<1>(A);
    portRequired |= std::get<1>(B);

    if (portRequired & SIG_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, ass_name + "_SIG = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/SIG']);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_SIG,'position',[20, 20, 40, 40]);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_SIG, 'port', '1');");
    }
    if (portRequired & REF_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, ass_name + "_REF = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/REF']);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_REF,'position',[20, 60, 40, 80]);");
      testBlockAppendLn(__FILE__, __LINE__, "set_param(" + ass_name + "_REF, 'port', '2');");
    }
    if (std::get<1>(A) & SIG_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + ass_name + "_SIG,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + std::get<0>(A) + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    }
    if (std::get<1>(A) & REF_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + ass_name + "_REF,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + std::get<0>(A) + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");
    }
    if (std::get<1>(B) & SIG_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + ass_name + "_SIG,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + std::get<0>(B) + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort1.Inport(1)" + ADD_LINE_AUTOROUTING + ");");
    }
    if (std::get<1>(B) & REF_PORT) {
      testBlockAppendLn(__FILE__, __LINE__, "OutPort1 = get_param(" + ass_name + "_REF,'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "InPort1 = get_param(" + std::get<0>(B) + ",'PortHandles');");
      testBlockAppendLn(__FILE__, __LINE__, "add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort1.Inport(2)" + ADD_LINE_AUTOROUTING + ");");
    }
  }

  return std::make_tuple(ass_name, portRequired);
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

  for (auto p : ports)
    std::cout << "- " << p << std::endl;

  std::cout << "## DONE" << std::endl;

  testBlockAppendFile.close();
}
