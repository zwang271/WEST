{
open Parser
}

let white = [' ' '\t']+
let newline = "\r" | "\n"  | "\r\n"
let digit = ['0'-'9']
let nat = digit+
let letter = ['a'-'z' 'A'-'Z']
let identifier= letter (letter | digit)*
let comma = ','
let comment = "#" ['\r' '\n']*

rule read = 
  parse
  | comment  { read lexbuf}
  | white  {read lexbuf}
  | nat { NAT (int_of_string (Lexing.lexeme lexbuf)) }
  | newline {EOL}
  | "," { COMMA }
  | "(" { LPAREN }
  | ")" { RPAREN }
  | "[" { LB }
  | "]" { UB }
  | "false" | "⊥" | "False" | "FALSE" { FALSE }
  | "true" | "⊤"  | "True" | "TRUE" { TRUE }
  | '!' | "¬" | "NOT" { NEG }
  | '&' | "∧" | "AND" { CONJ }
  | '|' | "∨" | "OR" { DISJ }
  | "=>" | "->" | "→" { IMP }
  | "<=>"  | "<->" | "↔" { IFF }
  | "GLOBALLY" | "G" | "□" { GLOBAL }
  | "FUTURE"  | "F" | "◊" { FUTURE }
  | "UNTIL"  | "U"  { UNTIL }
  | identifier {ATOM (Lexing.lexeme lexbuf)}
  | eof { EOF }
  | _ {failwith "Unable to parse:" }