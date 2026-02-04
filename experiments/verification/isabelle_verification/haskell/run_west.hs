import WEST_simp_pad (Nat, Mltl(..), WEST_bit, int_to_nat, nat_to_int, simp_pad_WEST_reg);
-- import WEST (Nat, Mltl(..), WEST_datatype, int_to_nat, nat_to_int, wEST_reg);
import Regex_equiv ( naive_equivalence );
import System.Environment ( getArgs );
import System.IO ( openFile, IOMode(..), hGetContents );
import Prelude;

main :: IO ()
main = do
    args <- getArgs
    let formula = (read (head args)::(Mltl Nat)) in
        let regex = simp_pad_WEST_reg formula in do
        -- let regex = wEST_reg formula in do
            print formula
            -- write regex to the file "out.txt"
            writeFile "out_isa.txt" (show regex)
            print "Regex written to out.txt\n"
        

                