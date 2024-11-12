open Ast

(** [parse s] parses [s] into an AST. *)
let parse (s : string) : mltl =
  let lexbuf = Lexing.from_string s in
  let ast = Parser.formSheet Lexer.read lexbuf in
  ast


let rec toString form = match form with 
| true -> "T"
| false -> "F"
| Prop (a) -> a.value^string_of_int(a.i) 
| Neg (a) -> "(!"^toString(a)^")"
| Conj (a,b) -> "("^toString(a)^"&"^toString(b)^")"
| Disj (a,b) -> "("^toString(a)^"|"^toString(b)^")"
| Imp (a,b) -> "("^toString(a)^"->"^toString(b)^")"
| Iff (a,b) -> "("^toString(a)^"<->"^toString(b)^")"
| G(lb,ub,a) -> "("^"G["^string_of_int(lb)^","^string_of_int(ub)^"]"^toString(a)^")"
| F(lb,ub,a) -> "("^"F["^string_of_int(lb)^","^string_of_int(ub)^"]"^toString(a)^")"
| U(a,lb,ub,b) -> "("^toString(a)^"U["^string_of_int(lb)^","^string_of_int(ub)^"]"^toString(b)^")"

let range lb ub  = List.init (ub-lb+1) (fun x -> x + lb )

(* let rec conjs = function 
| [] -> true 
| [h] -> h
| h::tail -> Conj(h, conjs tail )

let rec disjs = function 
| [] -> true 
| [h] -> h
| h::tail -> Conj(h, conjs tail ) *)

let rec repDisj temp1 intList1 = match intList1 with 
  | [] -> true
  | [h] -> temp1 h
  | h::tail1 -> Disj(temp1 h, repDisj temp1 tail1 )
  let rec repConj temp1 intList1 = match intList1 with 
  | [] -> true
  | [h] -> temp1 h
  | h::tail1 -> Conj(temp1 h, repConj temp1 tail1 )
(* Translates an MLTL formula (form) into a propositional logic formula (an MLTL formula with no temporal operators)  *)
  let rec translate form i = match form with
  | true -> true  
  | false -> false
  | Prop (a) -> Prop ({value=a.value; i = a.i + i})
  | Conj (a,b) -> Conj ((translate a i),  (translate b i))
  | Disj (a,b) -> Disj (translate a i, translate b i)
  | Iff (a,b) -> Iff (translate a i, translate b i)
  | Imp (a,b) -> Imp (translate a i, translate b i)
  | Neg a -> Neg (translate a i)
  | G (lb,ub,a) -> 
    if (ub == 0) then translate a i else 
      if (lb > 0) then translate (G(0,ub-lb,a))(i+lb) else 
        Conj(translate a i, translate (G(lb,ub-1,a)) (i+1))
  | F (lb,ub,a) -> 
    if (ub == 0) then translate a i else 
    if (lb > 0) then translate (F(0,ub-lb,a))(i+lb) else 
      Disj(translate a i, translate (F(lb,ub-1,a)) (i+1))
  | U (a,lb,ub,b) -> 
    if (ub == 0) then translate b i else 
    if (lb > 0) then translate (U(a,0,ub-lb,b)) (i+lb) else  
      Disj(translate b i, Conj(translate a i, translate (U(a,lb,ub-1,b)) (i+1)))

(* 
t qU[1,2]p 0:
t (qU[0,1]p)) 1   
t p 1 | (t q 1 /\ t qU[0,0]p 2)
t p 1 | (t q 1 & t p 1 )

*)
(* [2,3,4]: 
   Conj ( temp 2, repConj temp [3,4] )
   Conj ( temp 2, Conj (temp 3, repConj temp [4] ) )
   Conj ( temp 2, Conj (temp 3, repConj temp [4] ) )  
*)
  (* | F (lb,ub,a) -> 
    if (lb < ub) then Disj( (translate a (i+lb)), ( translate (F(lb+1,ub,a) ) i)) 
  else 
    (if lb > ub then 
      failwith "Wrong limits on lb and ub" 
  else translate a (i+lb)) *)
  
(* let rec f a b = match a with 
| 0 -> 1
| _ -> f b (a-1) + f b (a-2)

let rec f b a = match a with 
| 0 -> 1
| _ -> let part = f b in part (a-1) + part (a-2) *)



