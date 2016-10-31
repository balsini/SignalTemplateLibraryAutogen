%{ /* -*- C++ -*- */
#include <cerrno>
#include <climits>
#include <float.h>
#include <cstdlib>
#include <string>
#include <STLdriver.h>
#include <STLparser.hh>

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
#undef yywrap
#define yywrap() 1

// The location of the current token.
static yy::location loc;
%}

%option noyywrap nounput batch debug noinput

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
float [0-9]+\.[0-9]+
blank [ \t]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%

%{
  // Code run each time yylex is called.
  loc.step();
%}

{blank}+   loc.step();
[\n]+      loc.lines(yyleng); loc.step();

"infty" return yy::STLparser::make_INFINITY(loc);

"="   return yy::STLparser::make_ASSIGN(loc);

"SIG" return yy::STLparser::make_INPUT(loc);
"REF" return yy::STLparser::make_REFERENCE(loc);

"isStep" return yy::STLparser::make_ISSTEP(loc);

","   return yy::STLparser::make_COMMA(loc);
";"   return yy::STLparser::make_SEMICOLON(loc);

"-"   return yy::STLparser::make_MINUS(loc);
"+"   return yy::STLparser::make_PLUS(loc);
"*"   return yy::STLparser::make_STAR(loc);
"/"   return yy::STLparser::make_SLASH(loc);

"("   return yy::STLparser::make_LRPAREN(loc);
")"   return yy::STLparser::make_RRPAREN(loc);
"["   return yy::STLparser::make_LSPAREN(loc);
"]"   return yy::STLparser::make_RSPAREN(loc);

">="  return yy::STLparser::make_GEQ(loc);
"<="  return yy::STLparser::make_LEQ(loc);
">"   return yy::STLparser::make_GREATER(loc);
"<"   return yy::STLparser::make_SMALLER(loc);
"=="  return yy::STLparser::make_EQUAL(loc);
"!="  return yy::STLparser::make_NEQUAL(loc);

"&&"  return yy::STLparser::make_AND(loc);
"||"  return yy::STLparser::make_OR(loc);
"!"   return yy::STLparser::make_NOT(loc);

"[]"  return yy::STLparser::make_ALWAYS(loc);
"<>"  return yy::STLparser::make_EVENTUALLY(loc);
"U"   return yy::STLparser::make_UNTIL(loc);

"//".*    {
  /* Single line comment */
}

[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/] {
  /* Multiline comment */
}

[/][*]    {
  driver.error (loc, "Unterminated comment");
}

{int}     {
  errno = 0;
  long n = strtol(yytext, NULL, 10);
  if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
    driver.error (loc, "integer is out of range");
  //return yy::STLparser::make_INUM(n, loc);
  return yy::STLparser::make_INUM(yytext, loc);
}

{float}   {
  errno = 0;
  long double n = strtold(yytext, NULL);
  if (! (DBL_MIN <= n && n <= DBL_MAX && errno != ERANGE))
    driver.error (loc, "floating point number is out of range");
  //return yy::STLparser::make_FNUM(n, loc);
  return yy::STLparser::make_FNUM(yytext, loc);
}

{id}      return yy::STLparser::make_VAR(yytext, loc);
.         driver.error (loc, "invalid character");
<<EOF>>   return yy::STLparser::make_END(loc);
%%

void STLdriver::scan_begin()
{
  yy_flex_debug = trace_scanning;
  if (file.empty () || file == "-") {
    yyin = stdin;
  } else if (!(yyin = fopen (file.c_str (), "r"))) {
    error ("cannot open " + file + ": " + strerror(errno));
    exit (EXIT_FAILURE);
  }
}

void STLdriver::scan_end()
{
  fclose (yyin);
}
