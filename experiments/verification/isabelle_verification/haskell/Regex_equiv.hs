{-# LANGUAGE EmptyDataDecls, RankNTypes, ScopedTypeVariables #-}

module Regex_equiv(WEST_bit, Set, naive_equivalence) where {

import Prelude ((==), (/=), (<), (<=), (>=), (>), (+), (-), (*), (/), (**),
  (>>=), (>>), (=<<), (&&), (||), (^), (^^), (.), ($), ($!), (++), (!!), Eq,
  error, id, return, not, fst, snd, map, filter, concat, concatMap, reverse,
  zip, null, takeWhile, dropWhile, all, any, Integer, negate, abs, divMod,
  String, Bool(True, False), Maybe(Nothing, Just), Show, Read);
import qualified Prelude;

data WEST_bit = Zero | One | S;

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

instance Eq WEST_bit where {
  a == b = equal_WEST_bit a b;
};
-- Define read and show for WEST_bit
instance Show WEST_bit where {
  show Zero = "0";
  show One = "1";
  show S = "S";
};
instance Read WEST_bit where {
  readsPrec _ ('0' : t) = [(Zero, t)];
  readsPrec _ ('1' : t) = [(One, t)];
  readsPrec _ ('S' : t) = [(S, t)];
};


data Set a = Set [a] | Coset [a];

fold :: forall a b. (a -> b -> b) -> [a] -> b -> b;
fold f (x : xs) s = fold f xs (f x s);
fold f [] s = s;

foldr :: forall a b. (a -> b -> b) -> [a] -> b -> b;
foldr f [] = id;
foldr f (x : xs) = f x . foldr f xs;

removeAll :: forall a. (Eq a) => a -> [a] -> [a];
removeAll x [] = [];
removeAll x (y : xs) = (if x == y then removeAll x xs else y : removeAll x xs);

membera :: forall a. (Eq a) => [a] -> a -> Bool;
membera [] y = False;
membera (x : xs) y = x == y || membera xs y;

inserta :: forall a. (Eq a) => a -> [a] -> [a];
inserta x xs = (if membera xs x then xs else x : xs);

insert :: forall a. (Eq a) => a -> Set a -> Set a;
insert x (Coset xs) = Coset (removeAll x xs);
insert x (Set xs) = Set (inserta x xs);

member :: forall a. (Eq a) => a -> Set a -> Bool;
member x (Coset xs) = not (membera xs x);
member x (Set xs) = membera xs x;

bot_set :: forall a. Set a;
bot_set = Set [];

sup_set :: forall a. (Eq a) => Set a -> Set a -> Set a;
sup_set (Coset xs) a = Coset (filter (\ x -> not (member x a)) xs);
sup_set (Set xs) a = fold insert xs a;

less_eq_set :: forall a. (Eq a) => Set a -> Set a -> Bool;
less_eq_set (Coset []) (Set []) = False;
less_eq_set a (Coset ys) = all (\ y -> not (member y a)) ys;
less_eq_set (Set xs) b = all (\ x -> member x b) xs;

equal_set :: forall a. (Eq a) => Set a -> Set a -> Bool;
equal_set a b = less_eq_set a b && less_eq_set b a;

flatten_list :: forall a. [[a]] -> [a];
flatten_list l = foldr (\ a b -> a ++ b) l [];

enumerate_list :: [WEST_bit] -> [[WEST_bit]];
enumerate_list [] = [[]];
enumerate_list (One : t) = map (\ a -> One : a) (enumerate_list t);
enumerate_list (Zero : t) = map (\ a -> Zero : a) (enumerate_list t);
enumerate_list (S : t) = enumerate_list (Zero : t) ++ enumerate_list (One : t);

enumerate_trace :: [[WEST_bit]] -> [[[WEST_bit]]];
enumerate_trace [] = [[]];
enumerate_trace (h : t) =
  flatten_list (let {
                  enumerate_H = enumerate_list h;
                  a = enumerate_trace t;
                } in map (\ ta -> map (\ ha -> ha : ta) enumerate_H) a);

enumerate_sets :: [[[WEST_bit]]] -> Set [[WEST_bit]];
enumerate_sets [] = bot_set;
enumerate_sets (h : t) = sup_set (Set (enumerate_trace h)) (enumerate_sets t);

naive_equivalence :: [[[WEST_bit]]] -> [[[WEST_bit]]] -> Bool;
naive_equivalence a b = equal_set (enumerate_sets a) (enumerate_sets b);

}
