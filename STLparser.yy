%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.0"
%defines
%define parser_class_name {STLparser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%code requires
{
#include <string>
#include <utility.h>
  class STLdriver;
}
// The parsing context.
%param { STLdriver &driver }
%locations
%initial-action
{
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &driver.file;
};
%define parse.trace
%define parse.error verbose
%code
{
#include <STLdriver.h>
}
%define api.token.prefix {TOK_}
%token
END  0    "end of file"
ASSIGN    "="
INFINITY  "infty"
COMMA     ","
SEMICOLON ";"
;

%token
MINUS     "-"
PLUS      "+"
STAR      "*"
SLASH     "/"
;

%token
AND       "&&"
OR        "||"
;

%token    NOT "<!> (to be implemented)"

%token
LRPAREN   "("
RRPAREN   ")"
LSPAREN   "["
RSPAREN   "]"
;

%token
ALWAYS      "[]"
EVENTUALLY  "<>"
UNTIL       "U"
;

%token
GEQ       ">="
LEQ       "<="
GREATER   ">"
SMALLER   "<"
EQUAL     "=="
NEQUAL    "!="
;

%token
ISSTEP      "isStep"
DIFF        "diff"
;

%token <std::string>  VAR   "identifier"
%token <std::string>  INUM  "integer number"
%token <std::string>  FNUM  "floating point number"

%type  <std::string>  exp
%type  <std::string>  exp2
%type  <MathOperation *>  expWP
%type  <MathOperation *>  expWP1
%type  <MathOperation *>  expWP2
%type  <MathOperation *>  expWP3
%type  <ComparisonOperation *>  cmp
%type  <LogicalOperation *>  boolExp
%type  <LogicalOperator>  boolOp
%type  <ComparisonOperator> cmpOp
%type  <std::string>  assignment
%type  <std::string>  assignments
%type  <std::string>  STLFormula
%type  <TimeInterval> time_range
%type  <Border>       lparen
%type  <Border>       rparen

%left "&&" "||";
%left "!"
%left "+" "-";
%left "*" "/";

%printer { yyoutput << $$; } <*>;

%% /*------------------------------------------------------------------------*/

parser:
header  {
  std::cout << "## Header DONE ##" << std::endl;
  driver.printConstantValues();
  driver.setStatus(BODY);
}
body    {
  std::cout << "## Body DONE ##" << std::endl;
  driver.setStatus(FOOTER);
}
footer  {
  std::cout << "## Footer DONE ##" << std::endl;
}
;

header:
%empty
| header header_line
;

header_line:
assignments   SEMICOLON
;

body:
%empty
| body body_line
;

body_line:
STLFormula   SEMICOLON
;

assignments:
assignments assignment  { $$ = $1 + $2; }
| assignment              { $$ = $1; }
;

assignment:
VAR "=" exp         { $$ = $1 + " = " + $3; driver.setVariable($1, $3); }
| VAR "=" assignment  { $$ = $1 + " = " + $3; driver.setVariable($1, $3); }
;

STLFormula:
temporalOperator time_range "(" boolExp ")" {
  std::list<blockPortMapping> l;
  l.push_back(driver.createSTLFormulaBody($4));
  driver.connectSTLFormulas(l);
  foundMainTimeRange($2);
}
;

temporalOperator: ALWAYS | EVENTUALLY;

boolExp:
"(" boolExp ")" {
  $$ = $2;
  //driver.appendln("\"(\" boolExp \")\"");
}
| boolExp boolOp boolExp  {
  $$ = driver.createLogicalBlock($2, $1, $3);
  //driver.appendln("boolExp boolOp boolExp");
  //foundComparisonExpression($2, $1, $3);
}
| cmp                   {
  $$ = driver.createLogicalBlock(COMPARISON);
  $$->value = $1;
  //driver.appendln("cmp");
}
//| NOT "(" cmp ")"             {
  //$$ = driver.createComparisonExpressionBlock(NOT, $3);
//}
//| boolFunction        {
  //$$ = nullptr;
//}
;

cmp:
"(" cmp ")" {
  $$ = $2;
}
| expWP cmpOp expWP   {
  $$ = driver.createComparisonBlock($2, $1, $3);
}
;

boolOp:
AND {
  $$ = AND;
}
| OR  {
  $$ = OR;
}
;

cmpOp:
">="  { $$ = GEQ; }
| "<="  { $$ = LEQ; }
| ">"   { $$ = GREATER; }
| "<"   { $$ = SMALLER; }
| "=="  { $$ = EQUAL; }
| "!="  { $$ = NEQUAL; }
;

exp:
"(" exp ")"   {
  $$ = "(" + $2 + ")";
}
| exp "+" exp {
  $$ = $1 + "+" + $3;
}
| exp "-" exp {
  $$ = $1 + "-" + $3;
}
| exp2 { $$ = $1; }
;

exp2:
exp "*" exp {
  $$ = $1 + "*" + $3;
}
| exp "/" exp {
  $$ = $1 + "/" + $3;
}
| FNUM          {
  $$ = $1;
}
| INUM          {
  $$ = $1;
}
| VAR           {
  if (driver.portExists($1)) {
    error (yyla.location, "variable named <" + $1 + "> already defined in model");
    YYABORT;
  }
  if (!driver.variableExists($1)) {
    error (yyla.location, "undefined variable <" + $1 + ">");
    YYABORT;
  }
  $$ = "(" + driver.getVariable($1) + ")";
}
;

expWP:
expWP1 {
  $$ = $1;
}
;

expWP1: // Expressions plus external ports
"(" expWP1 ")" {
  $$ = $2;
}
| expWP1 "+" expWP1 {
  $$ = driver.createMathBlock(SUM, $1, $3);
}
| expWP1 "-" expWP1 {
  $$ = driver.createMathBlock(SUB, $1, $3);
}
| expWP2 { $$ = $1; }
;

expWP2: // Expressions plus external ports
expWP1 "*" expWP1 {
  $$ = driver.createMathBlock(MUL, $1, $3);
}
| expWP1 "/" expWP1 {
  $$ = driver.createMathBlock(DIV, $1, $3);
}
| expWP3 {
  $$ = $1;
}
;

expWP3:
FNUM          {
  $$ = driver.createMathBlock(CONST);
  $$->value = $1;
  foundConstantBlock($1);
}
| INUM          {
  $$ = driver.createMathBlock(CONST);
  $$->value = $1;
  foundConstantBlock($1);
}
| VAR           {
  if (driver.variableExists($1)) {
    $$ = driver.createMathBlock(CONST);
    $$->value = driver.getVariable($1);
    foundConstantBlock(driver.getVariable($1));
  } else if (driver.portExists($1)) {
    $$ = driver.createMathBlock(PORT);
    $$->value = $1;
    foundPortBlock($1);
  } else {
    error (yyla.location, "undefined variable or port <" + $1 + ">");
    YYABORT;
  }
}
;

boolFunction:
ISSTEP "(" exp "," exp ")" {
  driver.createIsStepBlock($3, $5);
}
;

//function:
//  DIFF "(" exp ")" {
//    driver.createDiffBlock($3);
//  }
//;

time_range:
lparen exp "," exp rparen {
  $$ = TimeInterval($2, $1, $4, $5);
}
;

lparen:
"(" { $$ = INTERVAL_OPEN; }
| "[" { $$ = INTERVAL_CLOSED; }
;

rparen:
")" { $$ = INTERVAL_OPEN; }
| "]" { $$ = INTERVAL_CLOSED; }
;

footer:
END
;

%% /*------------------------------------------------------------------------*/

void yy::STLparser::error(const location_type& l, const std::string& m)
{
  driver.error(l, "ERROR: " + m);
}
