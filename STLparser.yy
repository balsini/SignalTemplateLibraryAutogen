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
  INPUT     "sig"
  REFERENCE "ref"
;

%token <std::string>  VAR   "identifier"
%token <int>          INUM  "integer number"
%token <long double>  FNUM  "floating point number"

%type  <long double>  exp
%type  <bool>         cmp
%type  <long double>  assignment

%printer { yyoutput << $$; } <*>;

%% /*------------------------------------------------------------------------*/

parser: body footer;

body:
| body line
;

line:
  assignments SEMICOLON
| assertions   SEMICOLON
;

assignments:
  assignments assignment
| assignment
;

assignment:
  VAR ASSIGN exp        { driver.variables[$1] = $3; $$ = $3; }
| VAR ASSIGN assignment { driver.variables[$1] = $3; $$ = $3; }
;

assertions:
  assertions assertion  {}
| assertion             {}
;

assertion:
  ALWAYS      time formula {}
| EVENTUALLY  time formula {}
| ALWAYS      formula {}
| EVENTUALLY  formula {}
;

time:
  LSPAREN exp COMMA exp RSPAREN {}
;

formula:
  LRPAREN cmp RRPAREN {}
| LRPAREN assertion RRPAREN {}
;

cmp:
  exp GEQ exp     { $$ = ($1 <= $3); }
| exp LEQ exp     { $$ = ($1 >= $3); }
| exp GREATER exp { $$ = ($1 < $3); }
| exp SMALLER exp { $$ = ($1 > $3); }
| exp EQUAL exp   { $$ = ($1 == $3); }
| exp NEQUAL exp  { $$ = ($1 != $3); }
| "(" cmp ")"     { std::swap($$, $2); }
| cmp AND cmp     { $$ = ($1 && $3); }
| cmp OR cmp      { $$ = ($1 || $3); }
| NOT cmp         { $$ = !( $2 ); }
;

exp:
  exp "+" exp   { $$ = ($1 + $3); }
| exp "-" exp   { $$ = ($1 - $3); }
| exp "*" exp   { $$ = ($1 * $3); }
| exp "/" exp   { $$ = ($1 / $3); }
| "(" exp ")"   { std::swap($$, $2); }
| VAR           { $$ = driver.variables[$1]; }
| FNUM          { $$ = $1; }
| INUM          { $$ = $1; }
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
