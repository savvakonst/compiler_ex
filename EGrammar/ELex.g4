lexer grammar ELex;

LOG :   'log'|'Log';
LOG10:  'log10'|'Log10';
COS:    'cos'|'Cos';
SIN:    'sin'|'Sin';
EXP:    'exp'|'Exp';

//conversion func

C_FLOAT:    'float';
C_INT:      'int';
C_DOUBLE:   'double';
C_INT64:    'int64';

SPEC_SYMBOL : 'func' | 'return ' | 'convolve' | 'range' | 'shift'| 'param' | 'out';

MUL :     '*' ;
DIV :     '/' ;
ADD :     '+' ;
SUB :     '-' ;
EQ :      '==';
NEQ :     '!=';
LESS_ :    '<' ;
MORE_ :    '>' ;
LSH :     '<<';
RSH :     '>>';
LESSEQ :  '<=';
MOREEQ :  '=>';

ID  :   [a-zA-Z][a-zA-Z0-9_]* ;





INT64 : [0-9]+'i64'  ;
INT :   [0-9]+'i32'? ;
DOUBLE  :   [0-9]+'.'[0-9]*(DoublePostfix)? ;
FLOAT   :   [0-9]+'.'[0-9]*(FloatPostfix);
DoublePostfix:  'd' ;
FloatPostfix :  'f' ;

//COMMENT
//   : '#' (~ [\r\n])*
//   ;

STRINGLITERAL :   '"' ~ [\r\n"]* '"' ;
COMMENT       :   '#' ~ [\r\n]* -> skip;

NEWLINE       :   ('\r'? '\n')  ;     // return newlines to parser (is end-statement signal)
WS            :   [ \t]+ -> skip ;  
