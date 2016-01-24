# EduParse

## TL;DR

This project has grown as an university finals project.

We decided to create a simple interpreter, which allows to evaluate arithmetic expressions, builtin functions, variable management and simple console I/O.

Also, we decided to publish our work to provide interested (students, hobbyists, etc) a small example how a simple interpreter works - out of the box.

## Lexer grammar definition:
```
char        = ("a"-"z"|"A"-"Z")

digit       = ("0"-"9")

eot         = <INTERNAL>
undefined   = <INTERNAL>
terminator  = <INTERNAL>

declare     = "let"
identifier  = char[{char|digit|"_"}] | undefined
number      = [digit]["."]{digit}    | undefined
op          = "+" | "-" | "*" | "/" | "(" | ")" | ";" | "," | "^" | "="
            | undefined 
```
## Parser grammar definition:
```
statement   = (declaration | assignment | expression) ";"
declaration = "let" decl_frag [{"," decl_frag}]
decl_frag   = identifier [assignment]
assignment  = "=" expression
expression  = term [{("+"|"-") term}]
term        = mod_oper [{"*"|"/"} mod_oper}]    
mod_oper    = ["-"] operand ["^" mod_oper]

identifier  = func | var
func        = token_identifier "(" expression ")"
var         = token_identifier;
operand     = number | identifier | "(" expression ")"
```

## Builtin functions and constants:
```C
// Mathematical functions ( math.h )
double  abs (double);

double  sin (double);
double  cos (double);
double  tan (double);

double  asin(double);
double  acos(double);
double  atan(double);

double  sqrt(double);
double  lg  (double);
double  ln  (double);

// Console output
void    print       (double);
// Read a single double from console
double  read        (void);
// Exit program
void    exit        (void);
// Print help information
void    help        (void);
// Clear (user defined) variables
void    clearstate  (void);

// constants 
const double PI = /* math.h */ M_PI;
const double E  = /* math.h */ M_E;

// obsolete
const double EXIT_SUCCESS = /* stdlib.h */ EXIT_SUCCESS;
const double EXIT_FAILURE = /* stdlib.h */ EXIT_FAILURE;
```
