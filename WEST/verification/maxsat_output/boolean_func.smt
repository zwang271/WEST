(declare-fun t0 (Int) Bool)
(declare-fun t1 (Int) Bool)
(declare-fun t2 (Int) Bool)
(assert (and (t2 0) (t1 0)))
(assert (= (t1 0) (and true true )))
(assert (= (t2 0) (and false false )))
(check-sat)
