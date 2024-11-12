open Interp
(* open Ast  *)
open Main


(* let temp = Sys.argv.(1)  *)


(* let () = print_endline "p U[1,2]q\n" *)
(* let temp2 = parse Sys.argv.(1) *)
(* let temp2 = parse "p U[1,2]q\n" *)

(* let expr = translate (temp2) (0) *)

(* let output  = toString expr *)

let () = Printf.printf "Hi\n"

let () = Sys.chdir "/home/gokul/MAXSAT_MLTL/artifact/"

let file = "Benchmarks/nasa-boeing/Boeing-WBS/models/arch1/Accumulator.smv.ltlf"

let ()=
  let ic = open_in file in
    try
      let line = input_line ic in
      print_string (line^"\n");
      let temp2 = parse (line^"\n") in 
      let expr = translate (temp2) (0) in
      print_endline (toString expr);
      flush stdout;
      close_in ic
    with e ->
      close_in_noerr ic;
      raise e
