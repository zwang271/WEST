(** Atom, has a string and a number *)
type atom = {value : string; mutable i : int;}

(** An mltl formula *)
type mltl = 
  | true 
  | false 
  | Prop of atom 
  | Conj of mltl*mltl 
  | Disj of mltl*mltl
  | Iff of mltl*mltl 
  | Imp of mltl*mltl 
  | Neg of mltl 
  | G of int*int*mltl 
  | F of int*int*mltl
  | U of mltl*int*int*mltl
