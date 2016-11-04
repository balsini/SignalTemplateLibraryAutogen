#include <STLdriver.h>
#include <STLparser.hh>

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

std::string STLdriver::createExpression(MathOperation * e)
{
  static unsigned int exp_num = 0;
  std::string expression_name = "Exp_" + std::to_string(exp_num++);

  if (e->op == CONST) {
    // Create block containing constant value
  } else if (e->op == SIG || e->op == REF) {
    // Create block containing input port
  } else {// SUM, SUB, MUL, DIV
    // Create mathematical block
  }

  std::cout << expression_name << std::endl;

  return expression_name;
}

std::string STLdriver::createComparisonBody()
{
  /*
  // Insert comparison in (this) subsystem

  appendln(ass_name + "_A = addEmptySubsystem([" + root_name + " '" + parent + "/" + ass_name + "'], 'A');");
  appendln(ass_name + "_B = addEmptySubsystem([" + root_name + " '" + parent + "/" + ass_name + "'], 'B');");
  appendln("set_param(" + ass_name + "_A,'position',[60, " + std::to_string(vpos) + ", 80, " + std::to_string(vpos + 20) + "])");
  appendln("set_param(" + ass_name + "_B,'position',[60, " + std::to_string(vpos + 40) + ", 80, " + std::to_string(vpos + 20 + 40) + "])");

  appendln("addCmpSubsystem([" + root_name + " '" + parent + "/" + ass_name + "'], 'OP', '" + ass_name_OP +  "', '" + ass_name + "_A', '" + ass_name + "_B');");
  */
  return "";
}

std::string STLdriver::createAssertionBody(LogicalOperation *l, std::string parent, unsigned int x, unsigned int y)
{
  unsigned int vpos;
  unsigned int hpos;
  static unsigned int ass_num = 0;
  std::string ass_name = "Ass_" + std::to_string(ass_num++);
  std::string root_name = "TEST";

  vpos = 40 * y + 20;
  hpos = 40 * x + 20;

  if (l->op == COMPARISON) {
    // Create block containing comparison expression

    appendln(ass_name + " = addEmptySubsystem([" + root_name + " '" + parent + "'], '" + ass_name + "');");
    appendln("set_param(" + ass_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "])");

    appendln(ass_name + "_OUT = add_block('simulink/Sinks/Out1', [" + root_name + " '" + parent + "/" + ass_name + "/" + ass_name + "_OUT']);");
    appendln("set_param(" + ass_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "])");

  } else {// AND, OR
    //////////////////////////
    // Create logical block //
    //////////////////////////

    // Create empty subsystem (this)
    std::string par = "";
    if (parent == "")
      par = root_name;
    else
      par = "[" + root_name + " '" + parent + "']";

    appendln(ass_name + " = addEmptySubsystem(" + par + ", '" + ass_name + "');");
    appendln("set_param(" + ass_name + ",'position',[60, " + std::to_string(vpos) + ", 100, " + std::to_string(vpos + 20) + "]);");

    std::string A = createAssertionBody(l->a, parent + "/" + ass_name, x + 1, y);
    std::string B = createAssertionBody(l->b, parent + "/" + ass_name, x + 1, y + 1);

    appendln(ass_name + "_OUT = add_block('simulink/Sinks/Out1', [" + root_name + " '" + parent + "/" + ass_name + "/" + ass_name + "_OUT']);");
    appendln("set_param(" + ass_name + "_OUT,'position',[180, " + std::to_string(vpos) + ", 200, " + std::to_string(vpos + 20) + "]);");

    appendln(ass_name + "_OP = add_block('simulink/Logic and Bit Operations/Logical Operator', [" + root_name + " '" + parent + "/" + ass_name + "/" + ass_name + "_OP']);");
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
    appendln("add_line([" + root_name + " '" + parent + "/" + ass_name + "'], OutPort1.Outport(1), InPort2.Inport(1));");
    appendln("add_line([" + root_name + " '" + parent + "/" + ass_name + "'], OutPort2.Outport(1), InPort2.Inport(2));");
    appendln("add_line([" + root_name + " '" + parent + "/" + ass_name + "'], OutPort3.Outport(1), InPort1.Inport(1));");
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
