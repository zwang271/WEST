open OUnit2
open Interp
open Ast
open Main

(** [make_i n i s] makes an OUnit test named [n] that expects
    [s] to evalute to [Int i]. *)
let make n realform s =
  [n >:: (fun _ -> assert_equal (realform) (translate (parse s) 0))]

let tests = [
  make "true" true "true;";
  make "false" false "false;";
  make "Proposition" (Prop ({value="p";i=0;})) "p;";
  make "Proposition2"  (Conj ((Prop ({value="p";i=0})) ,(Prop ({value="q";i=0})) )) "p & q;";
  make "Proposition3" (Disj (Conj
    (Prop({value = "p"; i = 0}),
    Prop({value = "q"; i = 0})),
  Prop {value = "r"; i = 0})) "p & q | r;";
  make "Proposition4" (Disj (Conj
    (Prop {value = "p"; i = 0},
    Prop {value = "q"; i = 0}),
  Neg
   (Prop {value = "r"; i = 0}))) "p & q | !r;";
   make "Global1" (Interp.Ast.Conj
   (Interp.Ast.Prop
     {Interp.Ast.value = "p"; i = 1},
   Interp.Ast.Prop
    {Interp.Ast.value = "p"; i = 2})) "G[1,2]p;";
   (* make "FG1" (Interp.Ast.F (2, 4,
   Interp.Ast.G (1, 2,
    Interp.Ast.Prop {Interp.Ast.value = "p"; i = 0}))) "F[2,4] G[1,2]p;"; *)
   ]


let _ = run_test_tt_main ("suite" >::: List.flatten tests)
