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
  // TODO
  appendln("createStepBlock [" + v1 + "] [" + v2 + "]");
}

void STLdriver::createDiffBlock(std::string v)
{
  // TODO
  appendln("createDiffBlock [" + v + "]");
}

void STLdriver::createExpressionBlock()
{
  appendln("--------) ExpressionBlock");

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
  //appendln("------------) createConstantBlock [" + v + "]");
}

void STLdriver::createSignalBlock()
{
  SIG_input = true;
  appendln("------------) createSignalBlock");
}

void STLdriver::createReferenceBlock()
{
  REF_input = true;
  appendln("------------) createReferenceBlock");
}

ComparisonOperation * STLdriver::createComparisonBlock(ComparisonOperator op, MathOperation *a, MathOperation *b)
{
  appendln("------) createComparisonBlock");
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
  appendln("----------) createMathBlock");

  MathOperation * m = new MathOperation;
  m->op = op;
  m->a = a;
  m->b = b;

  return m;
}

LogicalOperation * STLdriver::createLogicalBlock(LogicalOperator op, LogicalOperation *a, LogicalOperation *b)
{
  appendln("----------) createMathBlock");

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
  std::string expression_name = "Exp_" + std::to_string(exp_num);

  if (e->op == CONST) {
    // Create block containing constant value
  } else if (e->op == SIG || e->op == REF) {
    // Create block containing input port
  } else {// SUM, SUB, MUL, DIV
    // Create mathematical block
  }

  ++exp_num;

  std::cout << expression_name << std::endl;

  return expression_name;
}

void STLdriver::createAssertionBody(LogicalOperation *l)
{
  std::cout << "----) foundAssertionBody" << std::endl;
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
