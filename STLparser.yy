%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.0"
%defines
%define parser_class_name {STLparser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%code requires
{
# include <string>
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

%left "+" "-";
%left "*" "/";

%token
  AND       "&&"
  OR        "||"
  NOT       "!"
;

%left "&&" "||" "!";

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
  INPUT     "SIG"
  REFERENCE "REF"
;

%token
  ISSTEP      "isStep"
;

%token <std::string>  VAR   "identifier"
%token <std::string>  INUM  "integer number"
%token <std::string>  FNUM  "floating point number"

%type  <std::string>  exp
%type  <std::string>  cmp
%type  <std::string>  assignment
%type  <std::string>  assignments
%type  <std::string>  assertion
%type  <std::string>  assertion_body

%printer { yyoutput << $$; } <*>;

%% /*------------------------------------------------------------------------*/

parser: body footer;

body:
  %empty
| body line
;

line:
  assignments SEMICOLON   { driver.appendln($1 + ";"); }
| assertion   SEMICOLON   { driver.appendln($1 + ";"); }
;

assignments:
  assignments assignment  { $$ = $1 + $2; }
| assignment              { $$ = $1; }
;

assignment:
  VAR ASSIGN exp          { $$ = $1 + " = " + $3; }
| VAR ASSIGN assignment   { $$ = $1 + " = " + $3; }
;

assertion:
  ALWAYS      global_time "(" assertion_body ")" { $$ = "TODO assertion1" + $4; }
| EVENTUALLY  global_time "(" assertion_body ")" { $$ = "TODO assertion2" + $4; }
;

assertion_body:
  cmp                 { $$ = "TODO assertion_body1"; }
| assertion_body cmp  { $$ = "TODO assertion_body2"; }
;

cmp:
  "(" cmp ")" {  }
| exp GEQ exp         {  }
| exp LEQ exp         {  }
| exp GREATER exp     {  }
| exp SMALLER exp     {  }
| exp EQUAL exp       {  }
| exp NEQUAL exp      {  }
| cmp AND cmp         {  }
| cmp OR cmp          {  }
| NOT cmp             {  }
| function            { $$ = "function"; }
;

exp:
  "(" exp ")"   { $$ = " ( " + $2 + " ) "; }
| exp "+" exp   { $$ = $1 + " + " + $3; }
| exp "-" exp   { $$ = $1 + " - " + $3; }
| exp "*" exp   { $$ = $1 + " * " + $3; }
| exp "/" exp   { $$ = $1 + " / " + $3; }
| VAR           { $$ = $1; }
| FNUM          { $$ = $1; }
| INUM          { $$ = $1; }
| INPUT         { $$ = "inputSignal"; }
| REFERENCE     { $$ = "referenceSignal"; }
;

function:
  ISSTEP "(" exp "," exp ")" {  }
;

global_time:
  lparen exp "," exp rparen {  }
;

lparen:
  "(" {  }
| "[" {  }
;

rparen:
  ")" {  }
| "]" {  }
;

footer:
  END
;

%% /*------------------------------------------------------------------------*/

void
yy::STLparser::error (const location_type& l,
                          const std::string& m)
{
  driver.error (l, m);
}
