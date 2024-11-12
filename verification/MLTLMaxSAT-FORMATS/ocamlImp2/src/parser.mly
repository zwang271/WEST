%{
open Ast
%}

%token <int> NAT
%token COMMA 
%token LPAREN
%token RPAREN
%token LB
%token UB
%token TRUE
%token FALSE
%token NEG
%token CONJ  
%token DISJ
%token IMP
%token IFF
%token GLOBAL
%token FUTURE
%token UNTIL
%token <string> ATOM
%token EOF
%token EOL

%nonassoc ATOM
%nonassoc GLOBAL FUTURE UNTIL
%right IFF
%right IMP
%left DISJ
%left CONJ
%nonassoc NEG


%start <Ast.mltl> formSheet

%%

formSheet:
	| e = statements EOF { e }
	;

statements:
	| e = mltl EOL { e }



mltl:
	| LPAREN; e=mltl; RPAREN {e}
	| GLOBAL; LB; lb = NAT; COMMA; ub = NAT; UB; e1 = mltl { G (lb, ub, e1) } 
	| FUTURE; LB; lb = NAT; COMMA; ub = NAT; UB; e1 = mltl { F (lb, ub, e1) }
	| e1 = mltl; UNTIL; LB; lb = NAT; COMMA; ub = NAT; UB; e2 = mltl { U (e1, lb, ub, e2) }
	| NEG; e1 = mltl;  { Neg e1 } 
	| e1 = mltl; CONJ; e2 = mltl { Conj (e1, e2) }
	| e1 = mltl; DISJ; e2 = mltl { Disj (e1, e2) }
	| e1 = mltl; IFF; e2 = mltl { Iff (e1, e2) } 
	| e1 = mltl; IMP; e2 = mltl { Imp (e1, e2) }
	| s = ATOM { Prop ({value=s;i=0}) }
	| TRUE { true }
	| FALSE { false } 
	;
	
