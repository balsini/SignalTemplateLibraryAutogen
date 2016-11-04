#include <STLdriver.h>
#include <STLparser.hh>

const std::string TEST_ROOT = "TEST";

STLdriver::STLdriver(char * filename) :
  trace_scanning(false),
  trace_parsing(false),
  REF_input(false),
  SIG_input(false),
  status(HEADER)
{
  outputFile.open(filename, std::ofstream::out);
  if (outputFile.is_open()) {
    std::cout << "Output file opened" << std::endl;
  } else {
    std::cout << "Error opening file" << std::endl;
  }
}

STLdriver::~STLdriver()
{
  outputFile.close();
  std::cout << "Output file closed" << std::endl;
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

void STLdriver::append(const std::string &s)
{
  std::cout << s;
  outputFile << s;
}

void STLdriver::appendln(const std::string &s)
{
  append(s);
  std::cout << std::endl;
  outputFile << std::endl;
}

void STLdriver::createIsStepBlock(std::string v1, std::string v2)
{
  std::cout << "createStepBlock [" + v1 + "] [" + v2 + "]" << std::endl;
}

void STLdriver::createDiffBlock(std::string v)
{
  std::cout << "createDiffBlock [" + v + "]" << std::endl;
}

void STLdriver::createExpressionBlock()
{
  std::cout << "--------) ExpressionBlock" << std::endl;

  if (SIG_input) {
    // TODO add SIG port
    SIG_input = false;
  }

  if (REF_input) {
    // TODO add REF port
    REF_input = false;
  }
}

void createConstantBlock(std::string v)
{
  std::cout << "------------) createConstantBlock" << std::endl;
}

void STLdriver::createSignalBlock()
{
  SIG_input = true;
  std::cout << "------------) createSignalBlock" << std::endl;
}

void STLdriver::createReferenceBlock()
{
  REF_input = true;
  std::cout << "------------) createReferenceBlock" << std::endl;
}

ComparisonOperation * STLdriver::createComparisonBlock(ComparisonOperator op, MathOperation *a, MathOperation *b)
{
  std::cout << "------) createComparisonBlock" << std::endl;
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
  std::cout << "----------) createMathBlock" << std::endl;

  MathOperation * m = new MathOperation;
  m->op = op;
  m->a = a;
  m->b = b;

  return m;
}

LogicalOperation * STLdriver::createLogicalBlock(LogicalOperator op, LogicalOperation *a, LogicalOperation *b)
{
  std::cout << "----------) createMathBlock" << std::endl;

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
    appendln(it->first + " = addConst(PATH, '" + it->first + "', '" + it->second + "');");
    appendln("set_param(" + it->first + ", 'position', ["
             + std::to_string(horizontal_position) + ", "
             + std::to_string(vertical_position) + ", "
             + std::to_string(horizontal_position + horizontal_offset) + ", "
             + std::to_string(vertical_position + vertical_offset)
             + "]);");
    appendln("");
    vertical_position = vertical_position + (vertical_offset * 2);
    */
  }

  std::cout << std::endl << "-/--/--/--/--/--/--/--/--/-" << std::endl << std::endl;
}

void STLdriver::printAssertions()
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
    appendln(it->first + " = addConst(PATH, '" + it->first + "', '" + it->second + "');");
    appendln("set_param(" + it->first + ", 'position', ["
             + std::to_string(horizontal_position) + ", "
             + std::to_string(vertical_position) + ", "
             + std::to_string(horizontal_position + horizontal_offset) + ", "
             + std::to_string(vertical_position + vertical_offset)
             + "]);");
    appendln("");
    vertical_position = vertical_position + (vertical_offset * 2);
    */
  }

  std::cout << "--------------------------" << std::endl;
}

std::string STLdriver::createExpression(MathOperation * e,
                                        std::string parent,
                                        unsigned int x,
                                        unsigned int y)
{
  unsigned int vpos;
  unsigned int hpos;
  static unsigned int exp_num = 0;
  std::string exp_name = "Exp_" + std::to_string(exp_num++);

  vpos = 40 * y + 20;
  hpos = 40 * x + 20;

  if (e->op == CONST || e->op == SIG || e->op == REF) {
    // Create block containing input port or constant values

    if (e->op == CONST) {
      appendln(exp_name + " = addConst([" + TEST_ROOT + " '" + parent + "'], '" + exp_name + "', '" + e->value + "');");

      //appendln(exp_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + exp_name + "/" + exp_name + "_OUT']);");
      //appendln("set_param(" + exp_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "])");
    } else {
      std::string portName;
      if (e->op == SIG)
        portName = "SIG";
      else
        portName = "REF";

      appendln(exp_name + " = add_block('simulink/Sources/In1', [" + TEST_ROOT + " '" + parent + "/" + portName + "']);");
    }

    appendln("set_param(" + exp_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "]);");
  } else {// SUM, SUB, MUL, DIV
    // Create mathematical block

    appendln(exp_name + " = addEmptySubsystem([" + TEST_ROOT + " '" + parent + "'], '" + exp_name + "');");
    appendln("set_param(" + exp_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "]);");

    appendln(exp_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + exp_name + "/" + exp_name + "_OUT']);");
    appendln("set_param(" + exp_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "])");

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

    appendln(exp_name + "_OP = add_block('simulink/Math Operations/" + matOp + "', [" + TEST_ROOT + " '" + parent + "/" + exp_name + "/" + exp_name + "_OP']);");
    appendln("set_param(" + exp_name + "_OP,'position',[140, " + std::to_string(vpos) + ", 160, " + std::to_string(vpos + 20) + "]);");

    std::string A = createExpression(e->a, parent + "/" + exp_name, x + 1, y);
    std::string B = createExpression(e->b, parent + "/" + exp_name, x + 1, y + 1);

    appendln("OutPort1 = get_param(" + A + ",'PortHandles');");
    appendln("OutPort2 = get_param(" + B + ",'PortHandles');");
    appendln("OutPort3 = get_param(" + exp_name + "_OP,'PortHandles');");
    appendln("InPort1 = get_param(" + exp_name + "_OUT,'PortHandles');");
    appendln("InPort2 = get_param(" + exp_name + "_OP,'PortHandles');");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + exp_name + "'], OutPort1.Outport(1), InPort2.Inport(1));");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + exp_name + "'], OutPort2.Outport(1), InPort2.Inport(2));");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + exp_name + "'], OutPort3.Outport(1), InPort1.Inport(1));");
  }

  std::cout << exp_name << std::endl;

  return exp_name;
}

std::string STLdriver::createAssertionBody(LogicalOperation *l, std::string parent, unsigned int x, unsigned int y)
{
  unsigned int vpos;
  unsigned int hpos;
  static unsigned int ass_num = 0;
  std::string ass_name = "Ass_" + std::to_string(ass_num++);

  vpos = 40 * y + 20;
  hpos = 40 * x + 20;

  if (l->op == COMPARISON) {
    // Create block containing the TWO comparison expressions

    appendln(ass_name + " = addEmptySubsystem([" + TEST_ROOT + " '" + parent + "'], '" + ass_name + "');");
    appendln("set_param(" + ass_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "])");

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

    appendln(ass_name + "_OP = add_block('simulink/Logic and Bit Operations/Relational Operator', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/" + ass_name + "_OP']);");
    appendln("set_param(" + ass_name + "_OP,'position',[140, " + std::to_string(vpos) + ", 160, " + std::to_string(vpos + 20) + "]);");
    appendln("set_param(" + ass_name + "_OP,'Operator', '" + relOp + "');");

    std::string A = createExpression(l->value->a, parent + "/" + ass_name, x + 1, y);
    std::string B = createExpression(l->value->b, parent + "/" + ass_name, x + 1, y + 1);

    appendln(ass_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/" + ass_name + "_OUT']);");
    appendln("set_param(" + ass_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "])");

    appendln("OutPort1 = get_param(" + ass_name + "_OP,'PortHandles');");
    appendln("OutPort2 = get_param(" + A + ",'PortHandles');");
    appendln("OutPort3 = get_param(" + B + ",'PortHandles');");
    appendln("InPort1 = get_param(" + ass_name + "_OUT,'PortHandles');");
    appendln("InPort2 = get_param(" + ass_name + "_OP,'PortHandles');");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort1.Inport(1));");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort2.Outport(1), InPort2.Inport(1));");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort3.Outport(1), InPort2.Inport(2));");

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

    appendln(ass_name + " = addEmptySubsystem(" + par + ", '" + ass_name + "');");
    appendln("set_param(" + ass_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "]);");

    std::string A = createAssertionBody(l->a, parent + "/" + ass_name, x + 1, y);
    std::string B = createAssertionBody(l->b, parent + "/" + ass_name, x + 1, y + 1);

    appendln(ass_name + "_OUT = add_block('simulink/Sinks/Out1', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/" + ass_name + "_OUT']);");
    appendln("set_param(" + ass_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "]);");

    appendln(ass_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + TEST_ROOT + " '" + parent + "/" + ass_name + "/" + ass_name + "_OP']);");
    appendln("set_param(" + ass_name + "_OP,'position',[140, " + std::to_string(vpos) + ", 160, " + std::to_string(vpos + 20) + "]);");

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
    appendln("set_param(" + ass_name + "_OP,'Operator', '" + logOp + "');");

    appendln("OutPort1 = get_param(" + A + ",'PortHandles');");
    appendln("OutPort2 = get_param(" + B + ",'PortHandles');");
    appendln("OutPort3 = get_param(" + ass_name + "_OP,'PortHandles');");
    appendln("InPort1 = get_param(" + ass_name + "_OUT,'PortHandles');");
    appendln("InPort2 = get_param(" + ass_name + "_OP,'PortHandles');");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort2.Inport(1));");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort2.Outport(1), InPort2.Inport(2));");
    appendln("add_line([" + TEST_ROOT + " '" + parent + "/" + ass_name + "'], OutPort3.Outport(1), InPort1.Inport(1));");
  }

  std::cout << std::endl;

  return ass_name;
}

void foundConstantBlock(std::string v)
{
  std::cout << "------------) foundConstantBlock [" << v << "]" << std::endl;
}

void foundMainTimeRange(TimeInterval t)
{
  std::cout << "----) foundMainTimeRange ["
            << (t.startBorder == INTERVAL_OPEN ? std::string("(") : std::string("["))
            << t.start << " , "
            << t.end
            << (t.endBorder == INTERVAL_OPEN ? std::string(")") : std::string("]"))
            << "]" << std::endl;
}

void foundComparisonExpression(LogicalOperator op, std::string v1, std::string v2)
{
  std::cout << "------) foundComparisonExpression [" << v1 << "][" << v2 << "]" << std::endl;
}
