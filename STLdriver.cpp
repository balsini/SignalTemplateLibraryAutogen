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

void STLdriver::createMainTimeRange(TimeInterval t)
{
  // TODO
  appendln("createMainTimeRange : "
           + (t.startBorder == INTERVAL_OPEN ? std::string("(") : std::string("["))
           + t.start + " , "
           + t.end
           + (t.endBorder == INTERVAL_OPEN ? std::string(")") : std::string("]")));
}

void STLdriver::createIsStepBlock(std::string v1, std::string v2)
{
  // TODO
  appendln("StepBlockCreation [" + v1 + "] [" + v2 + "]");
}

void STLdriver::createDiffBlock(std::string v)
{
  // TODO
  appendln("DiffBlockCreation [" + v + "]");
}

void STLdriver::createExpBlock()
{
  appendln("ExpressionBlock");

  if (SIG_input) {
    // TODO add SIG port
    SIG_input = false;
  }

  if (REF_input) {
    // TODO add REF port
    REF_input = false;
  }
}

void STLdriver::createConstantBlock(std::string v)
{
  appendln("ConstantBlock [" + v + "]");
}

void STLdriver::createSignalBlock()
{
  SIG_input = true;
  appendln("SignalBlock");
}

void STLdriver::createReferenceBlock()
{
  REF_input = true;
  appendln("ReferenceBlock");
}
