import Regex_equiv ( WEST_bit, naive_equivalence );
import System.IO ( openFile, IOMode(..), hGetContents );
import Prelude;

main :: IO ()
main = do
    -- read file contents of out_isa.txt
    isa_handle <- openFile "out_isa.txt" ReadMode
    isa_contents <- hGetContents isa_handle
    let isa_regex = (read isa_contents::[[[WEST_bit]]])
    -- read file contents of out_west.txt
    west_handle <- openFile "out_west.txt" ReadMode
    west_contents <- hGetContents west_handle
    let west_regex = (read west_contents::[[[WEST_bit]]])
    -- compare the two regexes and write result to out_equiv.txt
    let result = naive_equivalence isa_regex west_regex in
        writeFile "out_equiv.txt" (show result)

