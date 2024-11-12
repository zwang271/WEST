grammar MLTL;
// Grammar Rules
program: expr EOF;

expr:
	'(' expr ')'									# parens_expr
	| Neg expr										# neg_expr
	| Global '[' Number ',' Number ']' expr			# global_expr
	| Future '[' Number ',' Number ']' expr			# future_expr
	| expr Until '[' Number ',' Number ']' expr		# until_expr
	| expr Release '[' Number ',' Number ']' expr	# release_expr
	| expr And expr									# and_expr
	| expr Or expr									# or_expr
	| expr Equiv expr								# equiv_expr
	| expr NotEquiv expr								# notequiv_expr
	| expr Implies expr								# implies_expr
	| expr '?' expr ':' expr 					# ite_expr
	| Identifier									# atom_expr
	| True											# true_expr
	| False											# false_expr;

Global: 'G' | 'GLOBAL' | '□';
Future: 'F' | 'FUTURE' | '◊';
Until: 'U' | 'UNTIL';
Release: 'R' | 'V' | 'RELEASE';
Next: 'X' | '○' | 'NEXT' | 'next';
Neg: '!' | '~' | '¬' | 'NEG';
And: '&&' | '&' | '∧' | 'AND';
Or: '||' | '|' | '∨' | 'OR';
Equiv: '<->' | '<=>' | '↔' | '=' | '==' | 'EQUIV';
NotEquiv:  '!=' | 'NOTEQUIV';
Implies: '->' | '=>' | '→' | 'IMPLIES';
True: 'TRUE' | '⊤' | 'true';
False: 'FALSE' | '⊥' | 'false';

Identifier: Letter (Letter | Digit)*;

Number: Integer | Float;

fragment Integer: Sign? NonzeroDigit Digit* | '0';

fragment Float: Sign? Digit+ '.' Digit+;

fragment Sign: [+-];

fragment Digit: [0-9];

fragment NonzeroDigit: [1-9];

fragment Letter: [a-zA-Z_.];

Comment: '#' ~[\r\n]* -> skip;
WS: [ \t\r]+ -> skip;
