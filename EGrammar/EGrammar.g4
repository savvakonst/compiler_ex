grammar EGrammar;
import ELex;



start:  NEWLINE* (stat? NEWLINE)* EOF;

stat:   func                                                                                # defFunc
    |   line                                                                                # none
    |   (ID (','ID)*) '=' SPEC_SYM='param' '(' ( STRINGLITERAL (','STRINGLITERAL)*)? ')'    # assignParam
    |   SPEC_SYM='out'  (ID (','ID)*)                                                       # outParam
    ;

func    : ('func'|'function') ID '('args ')' ':'?  NEWLINE* ( stat NEWLINE*)+ SPEC_SYM=(';'|'end')
    ;

//convolve: 'convolve' '('expr ',' expr (','STRINGLITERAL )? ')' ;
//
line :   ID '=' expr                                     # assign
     |   SPEC_SYM='return ' (expr (','expr)*)            # assignRetParam
     ;

args:  (ID (','ID)*)  ;

unaryBInFunc : LOG|LOG10|COS|SIN|EXP;
tConvBInFunc: C_FLOAT|C_INT|C_DOUBLE|C_INT64;

//cond: expr;
left_expr: expr;
right_expr: expr ;


expr:   ID '(' (expr (','expr)*) ')'                                # callFunc
    |   SPEC_SYM='convolve'  '('expr','expr(','STRINGLITERAL)?')'   # callConvolve
    |   SPEC_SYM='range'     '('(expr (','expr)*)')'                # range
    |   SPEC_SYM='decimation''('expr ','expr')'                     # decimation
    |   SPEC_SYM='shift'     '('expr ','expr')'                     # shift
    |   unaryBInFunc '(' expr ')'                                   # callUnaryBInFunc
    |   tConvBInFunc '(' expr ')'                                   # callTConvBInFunc
    |   expr ('**'|'^')   expr                                      # pow
    |   expr op=('*'|'/') expr                                      # mulDiv
    |   expr op=('+'|'-') expr                                      # addSub
    |   expr op=('>'|'<') expr                                      # moreLess
    |   expr op=('>='|'<=') expr                                    # moreeqLesseq
    |   expr  op=('=='|'!=') expr                                   # equality
    |   expr  op=('<<'|'>>') expr                                   # bitwise
    |   expr '?' left_expr ':' right_expr                           # condExpr
    |   '-' expr                                                    # inv
    |   (DOUBLE|FLOAT|INT64|INT)                                    # const
    |   ID                                                          # id
    |   '(' expr ')'                                                # parens
    |   '[' (expr (','expr)*) ']'                                   # smallArrayDefinition
    ;
