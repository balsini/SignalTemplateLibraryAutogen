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
NOT       "!"
;

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
INPUT           "SIG"
REFERENCE       "REF"
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
%type  <std::string>  expWP
%type  <std::string>  expWP2
%type  <std::string>  cmp
%type  <ComparisonOperator> cmpOp
%type  <std::string>  assignment
%type  <std::string>  assignments
%type  <std::string>  assertion
%type  <std::string>  assertion_body
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
  driver.appendln("-) Header DONE");
  driver.setStatus(BODY);
}
body    {
  driver.appendln("-) Body DONE");
  driver.setStatus(FOOTER);
}
footer  {
  driver.appendln("-) Footer DONE");
}
;

header:
%empty
| header header_line
;

header_line:
assignments   SEMICOLON  // { driver.appendln($1); }
;

body:
%empty
| body body_line
;

body_line:
assertion   SEMICOLON  // { driver.appendln($1); }
;

assignments:
assignments assignment  { $$ = $1 + $2; }
| assignment              { $$ = $1; }
;

assignment:
VAR "=" exp         { $$ = $1 + " = " + $3; driver.setVariable($1, $3); }
| VAR "=" assignment  { $$ = $1 + " = " + $3; driver.setVariable($1, $3); }
;

assertion:
assertionOp time_range "(" assertion_body ")" {
  $$ = "TODO assertion" + $4;
  driver.createMainTimeRange($2);
}
;

assertionOp: ALWAYS | EVENTUALLY;

assertion_body:
cmp                 { $$ = "TODO assertion_body1"; }
| assertion_body cmp  { $$ = "TODO assertion_body2"; }
;

cmp:
"(" cmp ")" {  }
| expWP cmpOp expWP   {
  driver.createComparisonBlock($2, $1, $3);
}
| cmp AND cmp         {  }
| cmp OR cmp          {  }
| NOT cmp             {  }
| boolFunction        { $$ = "function"; }
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
  driver.createMathBlock(SUM, $1, $3);
}
| exp "-" exp {
  $$ = $1 + "-" + $3;
  driver.createMathBlock(SUB, $1, $3);
}
| exp2 { $$ = $1; }
;

exp2:
exp "*" exp {
  $$ = $1 + "*" + $3;
  driver.createMathBlock(MUL, $1, $3);
}
| exp "/" exp {
  $$ = $1 + "/" + $3;
  driver.createMathBlock(DIV, $1, $3);
}
| FNUM          {
  $$ = $1;
  driver.createConstantBlock($1);
}
| INUM          {
  $$ = $1;
  driver.createConstantBlock($1);
}
| VAR           {
  if (!driver.variableExists($1)) {
    error (yyla.location, "undefined variable <" + $1 + ">");
    YYABORT;
  }
  $$ = "(" + driver.getVariable($1) + ")";
  driver.createConstantBlock(driver.getVariable($1));
}
;

expWP: // Expressions plus external ports
"(" expWP ")" {
  $$ = "(" + $2 + ")";
}
| expWP "+" expWP {
  $$ = $1 + "+" + $3;
  driver.createMathBlock(SUM, $1, $3);
}
| expWP "-" expWP {
  $$ = $1 + "-" + $3;
  driver.createMathBlock(SUB, $1, $3);
}
| expWP2 { $$ = $1; }
;

expWP2: // Expressions plus external ports
expWP "*" expWP {
  $$ = $1 + "*" + $3;
  driver.createMathBlock(MUL, $1, $3);
}
| expWP "/" expWP {
  $$ = $1 + "/" + $3;
  driver.createMathBlock(DIV, $1, $3);
}
| INPUT         {
  $$ = "SIG";
  driver.createSignalBlock();
}
| REFERENCE     {
  $$ = "REF";
  driver.createReferenceBlock();
}
| FNUM          {
  $$ = $1;
  driver.createConstantBlock($1);
}
| INUM          {
  $$ = $1;
  driver.createConstantBlock($1);
}
| VAR           {
  if (!driver.variableExists($1)) {
    error (yyla.location, "undefined variable <" + $1 + ">");
    YYABORT;
  }
  $$ = "(" + driver.getVariable($1) + ")";
  driver.createConstantBlock(driver.getVariable($1));
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
