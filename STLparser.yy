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
END  0      "end of file"
BODYSTART   "%%"
ASSIGN      "="
INFINITY    "infty"
COMMA       ","
SEMICOLON   ";"
;

%token
MINUS       "-"
PLUS        "+"
STAR        "*"
SLASH       "/"
;

%token
AND         "&&"
OR          "||"
;

%token
NOT "<!> (to be implemented)"
;

%token
LRPAREN     "("
RRPAREN     ")"
LSPAREN     "["
RSPAREN     "]"
;

%token
ALWAYS      "[]"
EVENTUALLY  "<>"
UNTIL       "U"
;

%token
GEQ         ">="
LEQ         "<="
GREATER     ">"
SMALLER     "<"
EQUAL       "=="
NEQUAL      "!="
;

%token
ISSTEP      "isStep"
DIFF        "diff"
TRUE        "TRUE"
FALSE       "FALSE"
;

%left "&&" "||";
%left "!"
%left "+" "-";
%left "*" "/";



%token <std::string>            VAR   "identifier"
%token <std::string>            INUM  "integer number"
%token <std::string>            FNUM  "floating point number"

%type  <std::string>            exp
%type  <std::string>            exp2
%type  <Expression *>           expWP
%type  <Expression *>           expWP1
%type  <Expression *>           expWP2
%type  <Expression *>           expWP3
%type  <ComparisonExpression *> cmp
%type  <BooleanExpression *>    BoolExpr
%type  <LogicalOperator>        boolOp
%type  <ComparisonOperator>     cmpOp
%type  <std::string>            assignment
%type  <std::string>            assignments
%type  <STLFormula *>           STLFormula
%type  <TimeInterval>           time_range
%type  <TemporalOperator>       temporalOperator
%type  <std::string>            lparen
%type  <std::string>            rparen

%printer { yyoutput << $$; } <*>;

%% /*------------------------------------------------------------------------*/

/*
 *   **********************
 *   * Language structure *
 *   **********************
 *
 * STLFormula : BoolExpr | !STLFormula | STLFormula AND STLFormula | STLUntil | STLAlways | STLEventually
 * STLAlways : [] {TIME} STLFormula
 * STLEventually : <> {TIME} STLFormula
 * STLUntil : STLFormula U {TIME} STLFormula
 *
 * Expr : VAL [+ | * | - | /] VAL
 * CmpExpr : Expr [>= | < | ...] Expr
 * BoolExpr : CmpExpr | BoolExpr [&& | ||] BoolExpr | boolFunction | TRUE | FALSE
 *
 */

parser:
header  {
    std::cout << "## Header DONE ##" << std::endl;
    driver.printConstantValues();
    driver.setStatus(BODY);
}
BODYSTART
body    {
    std::cout << "## Body DONE ##" << std::endl;
    driver.createSTLFormulas();
    driver.setStatus(FOOTER);
}
footer  {
    std::cout << "## Footer DONE ##" << std::endl;
}
;

/*
 * *************************** HEADER ***************************
 */

header:
%empty
| header header_line
;

header_line:
assignments   SEMICOLON
;

assignments:
assignments assignment  {
    $$ = $1 + $2;
}
| assignment            {
    $$ = $1;
}
;

assignment:
VAR "=" exp             {
    $$ = $1 + " = " + $3; driver.setVariable($1, $3);
}
| VAR "=" assignment    {
    $$ = $1 + " = " + $3; driver.setVariable($1, $3);
}
;

exp:
"(" exp ")"     {
    $$ = "(" + $2 + ")";
}
| exp "+" exp   {
    $$ = $1 + "+" + $3;
}
| exp "-" exp   {
    $$ = $1 + "-" + $3;
}
| exp2          {
    $$ = $1;
}
;

exp2:
exp "*" exp     {
    $$ = $1 + "*" + $3;
}
| exp "/" exp   {
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

/*
 * *************************** BODY ***************************
 */

body:
%empty
| body body_line
;

body_line:
STLFormula   SEMICOLON  {
    driver.addSTLFormula($1);
}
;

STLFormula:
BoolExpr                {
    $$ = $1;
}
| STLFormula AND STLFormula
| STLAlways
| STLEventually
//| STLUntil
//| NOT STLFormula
/*
temporalOperator time_range "(" BoolExpr ")" {
  foundMainTimeRange($2);
  driver.addSTLFormula($1, $2, $4);
}
| temporalOperator time_range "(" BoolExpr UNTIL BoolExpr ")" {
  foundUntil();
  foundMainTimeRange($2);
  driver.addSTLFormula($1, $2, $4, $6);
}
*/
;

STLAlways:
ALWAYS STLFormula                   {

}
| ALWAYS time_range STLFormula      {

}
;

STLEventually:
EVENTUALLY STLFormula               {

}
| EVENTUALLY time_range STLFormula  {

}
;

BoolExpr:
"(" BoolExpr ")"                                    {
    $$ = $2;
}
| BoolExpr boolOp BoolExpr                          {
    $$ = new BooleanOperation($2, $1, $3);
}
| cmp                                               {
    $$ = $1;
}
| ISSTEP "(" expWP COMMA expWP ")"                  {
    $$ = new isStepFunction($3, $5);
}
| TRUE                                              {
    $$ = new BooleanValue(true);
}
| FALSE                                             {
    $$ = new BooleanValue(false);
}
;

cmp:
"(" cmp ")"         {
    $$ = $2;
}
| expWP cmpOp expWP {
    $$ = new ComparisonExpression($2, $1, $3);
}
;

boolOp:
AND     {
    $$ = AND;
}
| OR    {
    $$ = OR;
}
;

cmpOp:
GEQ         {
    $$ = GEQ;
}
| LEQ       {
$$ = LEQ;
}
| GREATER   {
    $$ = GREATER;
}
| SMALLER   {
    $$ = SMALLER;
}
| EQUAL     {
    $$ = EQUAL;
}
| NEQUAL    {
    $$ = NEQUAL;
}
;

expWP:
expWP1  {
    $$ = $1;
}
;

expWP1: // Expressions plus external ports
"(" expWP1 ")"      {
    $$ = $2;
}
| expWP1 "+" expWP1 {
    $$ = new Expression(SUM, $1, $3);
}
| expWP1 "-" expWP1 {
    $$ = new Expression(SUB, $1, $3);
}
| expWP2            {
    $$ = $1;
}
;

expWP2: // Expressions plus external ports
expWP1 "*" expWP1   {
    $$ = new Expression(MUL, $1, $3);
}
| expWP1 "/" expWP1 {
    $$ = new Expression(DIV, $1, $3);
}
| expWP3            {
    $$ = $1;
}
;

expWP3:
FNUM    {
    $$ = new Expression(CONST, $1);
}
| INUM  {
    $$ = new Expression(CONST, $1);
}
| VAR   {
    if (driver.variableExists($1)) {
        $$ = new Expression(CONST, driver.getVariable($1));
    } else if (driver.portExists($1)) {
        $$ = new Expression(PORT, $1);
    } else {
        error (yyla.location, "undefined variable or port <" + $1 + ">");
        YYABORT;
    }
}
;

//function:
//  DIFF "(" exp ")" {
//    driver.createDiffBlock($3);
//  }
//;

time_range:
lparen exp "," exp rparen   {
    $$ = TimeInterval($2, $1, $4, $5);
}
;

lparen:
LRPAREN     {
    $$ = "(";
}
| LSPAREN   {
    $$ = "[";
}
;

rparen:
RRPAREN     {
    $$ = ")";
}
| RSPAREN   {
    $$ = "]";
}
;

/*
 * *************************** FOOTER ***************************
 */

footer:
END
;

%% /*------------------------------------------------------------------------*/

void yy::STLparser::error(const location_type& l, const std::string& m)
{
    driver.error(l, "ERROR: " + m);
}
