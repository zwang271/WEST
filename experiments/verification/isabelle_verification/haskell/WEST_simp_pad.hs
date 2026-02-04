{-# LANGUAGE EmptyDataDecls, RankNTypes, ScopedTypeVariables #-}

module WEST_simp_pad(Nat, Mltl, WEST_bit, int_to_nat, nat_to_int, simp_pad_WEST_reg) where {

import Prelude ((==), (/=), (<), (<=), (>=), (>), (+), (-), (*), (/), (**),
  (>>=), (>>), (=<<), (&&), (||), (^), (^^), (.), ($), ($!), (++), (!!), Eq,
  error, id, return, not, fst, snd, map, filter, concat, concatMap, reverse,
  zip, null, takeWhile, dropWhile, all, any, Integer, negate, abs, divMod,
  String, Bool(True, False), Maybe(Nothing, Just), Show, Read);
import qualified Prelude;

data Nat = Zero_nat | Suc Nat;
-- Define a function to make a Nat from an integer
int_to_nat :: Integer -> Nat;
int_to_nat 0 = Zero_nat;
int_to_nat n = Suc (int_to_nat (n - 1));
-- Define a function to make an integer from a Nat
nat_to_int :: Nat -> Integer;
nat_to_int Zero_nat = 0;
nat_to_int (Suc n) = 1 + nat_to_int n;
-- Define a Show instance for Nat
instance Show Nat where {
  show n = Prelude.show (nat_to_int n);
};
-- Define a Read instance for Nat
instance Read Nat where {
  readsPrec i s = Prelude.readsPrec i s Prelude.>>= \ (n, s) -> Prelude.return (int_to_nat n, s);
};

equal_nat :: Nat -> Nat -> Bool;
equal_nat Zero_nat (Suc x2) = False;
equal_nat (Suc x2) Zero_nat = False;
equal_nat (Suc x2) (Suc y2) = equal_nat x2 y2;
equal_nat Zero_nat Zero_nat = True;

instance Eq Nat where {
  a == b = equal_nat a b;
};

less_eq_nat :: Nat -> Nat -> Bool;
less_eq_nat (Suc m) n = less_nat m n;
less_eq_nat Zero_nat n = True;

less_nat :: Nat -> Nat -> Bool;
less_nat m (Suc n) = less_eq_nat m n;
less_nat n Zero_nat = False;

class Ord a where {
  less_eq :: a -> a -> Bool;
  less :: a -> a -> Bool;
};

instance Ord Nat where {
  less_eq = less_eq_nat;
  less = less_nat;
};

class (Ord a) => Preorder a where {
};

class (Preorder a) => Order a where {
};

instance Preorder Nat where {
};

instance Order Nat where {
};

class (Order a) => Linorder a where {
};

instance Linorder Nat where {
};

data Set a = Set [a] | Coset [a];

data Mltl a = True_mltl | False_mltl | Prop_mltl a | Not_mltl (Mltl a)
  | And_mltl (Mltl a) (Mltl a) | Or_mltl (Mltl a) (Mltl a)
  | Future_mltl (Mltl a) Nat Nat | Global_mltl (Mltl a) Nat Nat
  | Until_mltl (Mltl a) (Mltl a) Nat Nat
  | Release_mltl (Mltl a) (Mltl a) Nat Nat deriving (Read, Show);

data WEST_bit = Zero | One | S;
-- Define a Show instance for WEST_datatype
instance Show WEST_bit where {
  show Zero = "0";
  show One = "1";
  show S = "S";
};

nth :: forall a. [a] -> Nat -> a;
nth (x : xs) (Suc n) = nth xs n;
nth (x : xs) Zero_nat = x;

upt :: Nat -> Nat -> [Nat];
upt i j = (if less_nat i j then i : upt (Suc i) j else []);

drop :: forall a. Nat -> [a] -> [a];
drop n [] = [];
drop n (x : xs) = (case n of {
                    Zero_nat -> x : xs;
                    Suc m -> drop m xs;
                  });

fold :: forall a b. (a -> b -> b) -> [a] -> b -> b;
fold f (x : xs) s = fold f xs (f x s);
fold f [] s = s;

take :: forall a. Nat -> [a] -> [a];
take n [] = [];
take n (x : xs) = (case n of {
                    Zero_nat -> [];
                    Suc m -> x : take m xs;
                  });

removeAll :: forall a. (Eq a) => a -> [a] -> [a];
removeAll x [] = [];
removeAll x (y : xs) = (if x == y then removeAll x xs else y : removeAll x xs);

member :: forall a. (Eq a) => [a] -> a -> Bool;
member [] y = False;
member (x : xs) y = x == y || member xs y;

inserta :: forall a. (Eq a) => a -> [a] -> [a];
inserta x xs = (if member xs x then xs else x : xs);

insert :: forall a. (Eq a) => a -> Set a -> Set a;
insert x (Coset xs) = Coset (removeAll x xs);
insert x (Set xs) = Set (inserta x xs);

gen_length :: forall a. Nat -> [a] -> Nat;
gen_length n (x : xs) = gen_length (Suc n) xs;
gen_length n [] = n;

max :: forall a. (Ord a) => a -> a -> a;
max a b = (if less_eq a b then b else a);

one_nat :: Nat;
one_nat = Suc Zero_nat;

bot_set :: forall a. Set a;
bot_set = Set [];

arbitrary_state :: Nat -> [WEST_bit];
arbitrary_state num_vars = map (\ _ -> S) (upt Zero_nat num_vars);

arbitrary_trace :: Nat -> Nat -> [[WEST_bit]];
arbitrary_trace num_vars num_pad =
  map (\ _ -> arbitrary_state num_vars) (upt Zero_nat num_pad);

pad :: [[WEST_bit]] -> Nat -> Nat -> [[WEST_bit]];
pad trace num_vars num_pad = trace ++ arbitrary_trace num_vars num_pad;

plus_nat :: Nat -> Nat -> Nat;
plus_nat (Suc m) n = plus_nat m (Suc n);
plus_nat Zero_nat n = n;

convert_nnf :: forall a. Mltl a -> Mltl a;
convert_nnf True_mltl = True_mltl;
convert_nnf False_mltl = False_mltl;
convert_nnf (Prop_mltl p) = Prop_mltl p;
convert_nnf (And_mltl phi psi) = And_mltl (convert_nnf phi) (convert_nnf psi);
convert_nnf (Or_mltl phi psi) = Or_mltl (convert_nnf phi) (convert_nnf psi);
convert_nnf (Future_mltl phi a b) = Future_mltl (convert_nnf phi) a b;
convert_nnf (Global_mltl phi a b) = Global_mltl (convert_nnf phi) a b;
convert_nnf (Until_mltl phi psi a b) =
  Until_mltl (convert_nnf phi) (convert_nnf psi) a b;
convert_nnf (Release_mltl phi psi a b) =
  Release_mltl (convert_nnf phi) (convert_nnf psi) a b;
convert_nnf (Not_mltl True_mltl) = False_mltl;
convert_nnf (Not_mltl False_mltl) = True_mltl;
convert_nnf (Not_mltl (Prop_mltl p)) = Not_mltl (Prop_mltl p);
convert_nnf (Not_mltl (Not_mltl phi)) = convert_nnf phi;
convert_nnf (Not_mltl (And_mltl phi psi)) =
  Or_mltl (convert_nnf (Not_mltl phi)) (convert_nnf (Not_mltl psi));
convert_nnf (Not_mltl (Or_mltl phi psi)) =
  And_mltl (convert_nnf (Not_mltl phi)) (convert_nnf (Not_mltl psi));
convert_nnf (Not_mltl (Future_mltl phi a b)) =
  Global_mltl (convert_nnf (Not_mltl phi)) a b;
convert_nnf (Not_mltl (Global_mltl phi a b)) =
  Future_mltl (convert_nnf (Not_mltl phi)) a b;
convert_nnf (Not_mltl (Until_mltl phi psi a b)) =
  Release_mltl (convert_nnf (Not_mltl phi)) (convert_nnf (Not_mltl psi)) a b;
convert_nnf (Not_mltl (Release_mltl phi psi a b)) =
  Until_mltl (convert_nnf (Not_mltl phi)) (convert_nnf (Not_mltl psi)) a b;

shift :: [[[WEST_bit]]] -> Nat -> Nat -> [[[WEST_bit]]];
shift traceList num_vars num_pad =
  map (\ a -> arbitrary_trace num_vars num_pad ++ a) traceList;

minus_nat :: Nat -> Nat -> Nat;
minus_nat (Suc m) (Suc n) = minus_nat m n;
minus_nat Zero_nat n = Zero_nat;
minus_nat m Zero_nat = m;

complen_mltl :: forall a. Mltl a -> Nat;
complen_mltl False_mltl = one_nat;
complen_mltl True_mltl = one_nat;
complen_mltl (Prop_mltl p) = one_nat;
complen_mltl (Not_mltl phi) = complen_mltl phi;
complen_mltl (And_mltl phi psi) = max (complen_mltl phi) (complen_mltl psi);
complen_mltl (Or_mltl phi psi) = max (complen_mltl phi) (complen_mltl psi);
complen_mltl (Global_mltl phi a b) = plus_nat b (complen_mltl phi);
complen_mltl (Future_mltl phi a b) = plus_nat b (complen_mltl phi);
complen_mltl (Release_mltl phi psi a b) =
  plus_nat b (max (minus_nat (complen_mltl phi) one_nat) (complen_mltl psi));
complen_mltl (Until_mltl phi psi a b) =
  plus_nat b (max (minus_nat (complen_mltl phi) one_nat) (complen_mltl psi));

size_list :: forall a. [a] -> Nat;
size_list = gen_length Zero_nat;

equal_WEST_bit :: WEST_bit -> WEST_bit -> Bool;
equal_WEST_bit One S = False;
equal_WEST_bit S One = False;
equal_WEST_bit Zero S = False;
equal_WEST_bit S Zero = False;
equal_WEST_bit Zero One = False;
equal_WEST_bit One Zero = False;
equal_WEST_bit S S = True;
equal_WEST_bit One One = True;
equal_WEST_bit Zero Zero = True;

wEST_and_bitwise :: WEST_bit -> WEST_bit -> Maybe WEST_bit;
wEST_and_bitwise b One = (if equal_WEST_bit b Zero then Nothing else Just One);
wEST_and_bitwise b Zero = (if equal_WEST_bit b One then Nothing else Just Zero);
wEST_and_bitwise b S = Just b;

wEST_and_state :: [WEST_bit] -> [WEST_bit] -> Maybe [WEST_bit];
wEST_and_state [] [] = Just [];
wEST_and_state (h1 : t1) (h2 : t2) =
  (case wEST_and_bitwise h1 h2 of {
    Nothing -> Nothing;
    Just b -> (case wEST_and_state t1 t2 of {
                Nothing -> Nothing;
                Just l -> Just (b : l);
              });
  });
wEST_and_state (v : va) [] = Nothing;
wEST_and_state [] (v : va) = Nothing;

wEST_and_trace :: [[WEST_bit]] -> [[WEST_bit]] -> Maybe [[WEST_bit]];
wEST_and_trace trace [] = Just trace;
wEST_and_trace [] (v : va) = Just (v : va);
wEST_and_trace (h1 : t1) (h2 : t2) =
  (case wEST_and_state h1 h2 of {
    Nothing -> Nothing;
    Just state -> (case wEST_and_trace t1 t2 of {
                    Nothing -> Nothing;
                    Just trace -> Just (state : trace);
                  });
  });

wEST_and_helper :: [[WEST_bit]] -> [[[WEST_bit]]] -> [[[WEST_bit]]];
wEST_and_helper trace [] = [];
wEST_and_helper trace (t : traces) =
  (case wEST_and_trace trace t of {
    Nothing -> wEST_and_helper trace traces;
    Just res -> res : wEST_and_helper trace traces;
  });

wEST_and :: [[[WEST_bit]]] -> [[[WEST_bit]]] -> [[[WEST_bit]]];
wEST_and traceList [] = [];
wEST_and [] (v : va) = [];
wEST_and (trace : traceList1) (v : va) =
  (case wEST_and_helper trace (v : va) of {
    [] -> wEST_and traceList1 (v : va);
    a : list -> (a : list) ++ wEST_and traceList1 (v : va);
  });

maxa :: forall a. (Linorder a) => Set a -> a;
maxa (Set (x : xs)) = fold max xs x;

wEST_num_vars :: Mltl Nat -> Nat;
wEST_num_vars True_mltl = one_nat;
wEST_num_vars False_mltl = one_nat;
wEST_num_vars (Prop_mltl p) = plus_nat p one_nat;
wEST_num_vars (Not_mltl phi) = wEST_num_vars phi;
wEST_num_vars (And_mltl phi psi) =
  maxa (insert (wEST_num_vars phi) (insert (wEST_num_vars psi) bot_set));
wEST_num_vars (Or_mltl phi psi) =
  maxa (insert (wEST_num_vars phi) (insert (wEST_num_vars psi) bot_set));
wEST_num_vars (Future_mltl phi a b) = wEST_num_vars phi;
wEST_num_vars (Global_mltl phi a b) = wEST_num_vars phi;
wEST_num_vars (Until_mltl phi psi a b) =
  maxa (insert (wEST_num_vars phi) (insert (wEST_num_vars psi) bot_set));
wEST_num_vars (Release_mltl phi psi a b) =
  maxa (insert (wEST_num_vars phi) (insert (wEST_num_vars psi) bot_set));

enumerate_pairs :: [Nat] -> [(Nat, Nat)];
enumerate_pairs [] = [];
enumerate_pairs (x : xs) = map (\ a -> (x, a)) xs ++ enumerate_pairs xs;

enum_pairs :: forall a. [a] -> [(Nat, Nat)];
enum_pairs l = enumerate_pairs (upt Zero_nat (size_list l));

count_nonS_trace :: [WEST_bit] -> Nat;
count_nonS_trace [] = Zero_nat;
count_nonS_trace (h : t) =
  (if not (equal_WEST_bit h S) then plus_nat one_nat (count_nonS_trace t)
    else count_nonS_trace t);

count_diff_state :: [WEST_bit] -> [WEST_bit] -> Nat;
count_diff_state [] [] = Zero_nat;
count_diff_state (v : va) [] = count_nonS_trace (v : va);
count_diff_state [] (v : va) = count_nonS_trace (v : va);
count_diff_state (h1 : t1) (h2 : t2) =
  (if equal_WEST_bit h1 h2 then count_diff_state t1 t2
    else plus_nat one_nat (count_diff_state t1 t2));

count_diff :: [[WEST_bit]] -> [[WEST_bit]] -> Nat;
count_diff [] [] = Zero_nat;
count_diff [] (h : t) = plus_nat (count_diff_state [] h) (count_diff [] t);
count_diff (h : t) [] = plus_nat (count_diff_state [] h) (count_diff [] t);
count_diff (h1 : t1) (h2 : t2) =
  plus_nat (count_diff_state h1 h2) (count_diff t1 t2);

check_simp :: [[WEST_bit]] -> [[WEST_bit]] -> Bool;
check_simp trace1 trace2 =
  (if less_eq_nat (count_diff trace1 trace2) one_nat &&
        equal_nat (size_list trace1) (size_list trace2)
    then True else False);

remove_element_at_index :: forall a. Nat -> [a] -> [a];
remove_element_at_index n l = take n l ++ drop (plus_nat n one_nat) l;

wEST_simp_bitwise :: WEST_bit -> WEST_bit -> WEST_bit;
wEST_simp_bitwise b S = S;
wEST_simp_bitwise b Zero = (if equal_WEST_bit b Zero then Zero else S);
wEST_simp_bitwise b One = (if equal_WEST_bit b One then One else S);

wEST_simp_state :: [WEST_bit] -> [WEST_bit] -> [WEST_bit];
wEST_simp_state s1 s2 =
  map (\ k -> wEST_simp_bitwise (nth s1 k) (nth s2 k))
    (upt Zero_nat (size_list s1));

wEST_get_state :: [[WEST_bit]] -> Nat -> Nat -> [WEST_bit];
wEST_get_state regex time num_vars =
  (if less_eq_nat (size_list regex) time
    then map (\ _ -> S) (upt Zero_nat num_vars) else nth regex time);

wEST_simp_trace :: [[WEST_bit]] -> [[WEST_bit]] -> Nat -> [[WEST_bit]];
wEST_simp_trace trace1 trace2 num_vars =
  map (\ k ->
        wEST_simp_state (wEST_get_state trace1 k num_vars)
          (wEST_get_state trace2 k num_vars))
    (upt Zero_nat
      (maxa (insert (size_list trace1) (insert (size_list trace2) bot_set))));

update_L :: [[[WEST_bit]]] -> (Nat, Nat) -> Nat -> [[[WEST_bit]]];
update_L l h num_vars =
  remove_element_at_index (fst h) (remove_element_at_index (snd h) l) ++
    [wEST_simp_trace (nth l (fst h)) (nth l (snd h)) num_vars];

wEST_simp_helper ::
  [[[WEST_bit]]] -> [(Nat, Nat)] -> Nat -> Nat -> [[[WEST_bit]]];
wEST_simp_helper l idx_pairs i num_vars =
  (if not (idx_pairs == enum_pairs l) || less_eq_nat (size_list idx_pairs) i
    then l
    else (if check_simp (nth l (fst (nth idx_pairs i)))
               (nth l (snd (nth idx_pairs i)))
           then let {
                  newL = update_L l (nth idx_pairs i) num_vars;
                } in wEST_simp_helper newL (enum_pairs newL) Zero_nat num_vars
           else wEST_simp_helper l idx_pairs (plus_nat i one_nat) num_vars));

wEST_simp :: [[[WEST_bit]]] -> Nat -> [[[WEST_bit]]];
wEST_simp l num_vars = wEST_simp_helper l (enum_pairs l) Zero_nat num_vars;

wEST_and_simp :: [[[WEST_bit]]] -> [[[WEST_bit]]] -> Nat -> [[[WEST_bit]]];
wEST_and_simp l1 l2 num_vars = wEST_simp (wEST_and l1 l2) num_vars;

wEST_or_simp :: [[[WEST_bit]]] -> [[[WEST_bit]]] -> Nat -> [[[WEST_bit]]];
wEST_or_simp l1 l2 num_vars = wEST_simp (l1 ++ l2) num_vars;

wEST_global :: [[[WEST_bit]]] -> Nat -> Nat -> Nat -> [[[WEST_bit]]];
wEST_global l a b num_vars =
  (if equal_nat a b then shift l num_vars a
    else (if less_nat a b
           then wEST_and_simp (shift l num_vars b)
                  (wEST_global l a (minus_nat b one_nat) num_vars) num_vars
           else []));

wEST_release_helper ::
  [[[WEST_bit]]] -> [[[WEST_bit]]] -> Nat -> Nat -> Nat -> [[[WEST_bit]]];
wEST_release_helper l_phi l_psi a ub num_vars =
  (if equal_nat a ub
    then wEST_and_simp (wEST_global l_phi a a num_vars)
           (wEST_global l_psi a a num_vars) num_vars
    else (if less_nat a ub
           then wEST_or_simp
                  (wEST_release_helper l_phi l_psi a (minus_nat ub one_nat)
                    num_vars)
                  (wEST_and_simp (wEST_global l_psi a ub num_vars)
                    (wEST_global l_phi ub ub num_vars) num_vars)
                  num_vars
           else []));

wEST_release ::
  [[[WEST_bit]]] -> [[[WEST_bit]]] -> Nat -> Nat -> Nat -> [[[WEST_bit]]];
wEST_release l_phi l_psi a b num_vars =
  (if less_nat a b
    then wEST_or_simp (wEST_global l_psi a b num_vars)
           (wEST_release_helper l_phi l_psi a (minus_nat b one_nat) num_vars)
           num_vars
    else wEST_global l_psi a b num_vars);

wEST_future :: [[[WEST_bit]]] -> Nat -> Nat -> Nat -> [[[WEST_bit]]];
wEST_future l a b num_vars =
  (if equal_nat a b then shift l num_vars a
    else (if less_nat a b
           then wEST_or_simp (shift l num_vars b)
                  (wEST_future l a (minus_nat b one_nat) num_vars) num_vars
           else []));

wEST_until ::
  [[[WEST_bit]]] -> [[[WEST_bit]]] -> Nat -> Nat -> Nat -> [[[WEST_bit]]];
wEST_until l_phi l_psi a b num_vars =
  (if equal_nat a b then wEST_global l_psi a a num_vars
    else (if less_nat a b
           then wEST_or_simp
                  (wEST_until l_phi l_psi a (minus_nat b one_nat) num_vars)
                  (wEST_and_simp
                    (wEST_global l_phi a (minus_nat b one_nat) num_vars)
                    (wEST_global l_psi b b num_vars) num_vars)
                  num_vars
           else []));

wEST_reg_aux :: Mltl Nat -> Nat -> [[[WEST_bit]]];
wEST_reg_aux True_mltl num_vars = [[map (\ _ -> S) (upt Zero_nat num_vars)]];
wEST_reg_aux False_mltl num_vars = [];
wEST_reg_aux (Prop_mltl p) num_vars =
  [[map (\ j -> (if equal_nat p j then One else S)) (upt Zero_nat num_vars)]];
wEST_reg_aux (Not_mltl (Prop_mltl p)) num_vars =
  [[map (\ j -> (if equal_nat p j then Zero else S)) (upt Zero_nat num_vars)]];
wEST_reg_aux (Or_mltl phi psi) num_vars =
  wEST_or_simp (wEST_reg_aux phi num_vars) (wEST_reg_aux psi num_vars) num_vars;
wEST_reg_aux (And_mltl phi psi) num_vars =
  wEST_and_simp (wEST_reg_aux phi num_vars) (wEST_reg_aux psi num_vars)
    num_vars;
wEST_reg_aux (Future_mltl phi a b) num_vars =
  wEST_future (wEST_reg_aux phi num_vars) a b num_vars;
wEST_reg_aux (Global_mltl phi a b) num_vars =
  wEST_global (wEST_reg_aux phi num_vars) a b num_vars;
wEST_reg_aux (Until_mltl phi psi a b) num_vars =
  wEST_until (wEST_reg_aux phi num_vars) (wEST_reg_aux psi num_vars) a b
    num_vars;
wEST_reg_aux (Release_mltl phi psi a b) num_vars =
  wEST_release (wEST_reg_aux phi num_vars) (wEST_reg_aux psi num_vars) a b
    num_vars;
wEST_reg_aux (Not_mltl True_mltl) num_vars = wEST_reg_aux False_mltl num_vars;
wEST_reg_aux (Not_mltl False_mltl) num_vars = wEST_reg_aux True_mltl num_vars;
wEST_reg_aux (Not_mltl (And_mltl phi psi)) num_vars =
  wEST_reg_aux (Or_mltl (Not_mltl phi) (Not_mltl psi)) num_vars;
wEST_reg_aux (Not_mltl (Or_mltl phi psi)) num_vars =
  wEST_reg_aux (And_mltl (Not_mltl phi) (Not_mltl psi)) num_vars;
wEST_reg_aux (Not_mltl (Future_mltl phi a b)) num_vars =
  wEST_reg_aux (Global_mltl (Not_mltl phi) a b) num_vars;
wEST_reg_aux (Not_mltl (Global_mltl phi a b)) num_vars =
  wEST_reg_aux (Future_mltl (Not_mltl phi) a b) num_vars;
wEST_reg_aux (Not_mltl (Until_mltl phi psi a b)) num_vars =
  wEST_reg_aux (Release_mltl (Not_mltl phi) (Not_mltl psi) a b) num_vars;
wEST_reg_aux (Not_mltl (Release_mltl phi psi a b)) num_vars =
  wEST_reg_aux (Until_mltl (Not_mltl phi) (Not_mltl psi) a b) num_vars;
wEST_reg_aux (Not_mltl (Not_mltl phi)) num_vars = wEST_reg_aux phi num_vars;

wEST_reg :: Mltl Nat -> [[[WEST_bit]]];
wEST_reg f = let {
               nnf_F = convert_nnf f;
             } in wEST_reg_aux nnf_F (wEST_num_vars f);

pad_WEST_reg :: Mltl Nat -> [[[WEST_bit]]];
pad_WEST_reg phi =
  let {
    unpadded = wEST_reg phi;
    complen = complen_mltl phi;
    num_vars = wEST_num_vars phi;
  } in map (\ l ->
             (if less_nat (size_list l) complen
               then pad l num_vars (minus_nat complen (size_list l)) else l))
         unpadded;

simp_pad_WEST_reg :: Mltl Nat -> [[[WEST_bit]]];
simp_pad_WEST_reg phi = wEST_simp (pad_WEST_reg phi) (wEST_num_vars phi);

}
