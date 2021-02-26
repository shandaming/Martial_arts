(define lat?
 (lambda (l)
  (cond
   ((null? l) #t)
   ((atom? (car l)) (lat? (car l)))
   (else #f))))

(define member?
 (lambda (a lat)
  (cond
   ((null? lat) #f)
   (else (or (eq? (car lat) a)
		  (member? a (cdr lat)))))))

(define rember
 (lambda (a lat)
  (cond
   ((null? lat) ((quote)))
   ((eq? (car lat) a) (cdr lat))
   (else (cons (car lat)
	(rember a (cdr lat)))))))

(define firsts
 (lambda (l)
  (cond
   ((null? l) quote())
   (else (cons (car (car l))
		  (firsts (cdr l)))))))

(define insert
 (lambda (new old lat)
  (cond
   ((null? lat) (quote ()))
   (else 
	(cond
	 ((eq? (car lat) old)
	  (cons new lat))
	  (else (cons (car lat) (insert new old (cdr lat)))))))))

(define rember*
  (lambda (a l)
    ((null? l) (quote())
     ((atom? (car l)) 
      (cond
       ((eq? (car l) a) (rember* a(cdr l))
	(else
	 (cons (car l) (rember* a(cdr l))))))
      (else(cons(car l) (rember* a(cdr l))(rember* a(cdr l))))))))

(define subst
 (lambda (new old lat)
  (cond
   ((null? lat) (quote ()))
   (else
	(cond
	 ((eq? (car lat) old)
	  (cons new (cdr lat)))
	 (else
	  (cons (car lat) (subst new old (cdr lat)))))))))

(define subst2
 (lambda (new o1 o2 lat)
  (cond
   ((null? lat) (quote ()))
   (else 
	(cond
	 ((or(eq? (car lat) o1) (eq? (car lat) o2)) (cons new (cdr lat)))
	 (else
	  (cons (car lat) (subst2 new o1 o2 (cdr lat)))))))))

(define multirember
 (lambda (a lat)
  (cond
   ((null? lat) (quote ()))
   (else 
	(cond
	 ((eq? (car lat) a) (multirember a (cdr lat)))
	 (else
	  (cons (car lat) (multirember a (cdr lat)))))))))

(define inc
 (lambda (n)
  (+ n 1)))

(define dec
 (lambda (n)
  (- n 1)))

(define addl
 (lambda (n m)
  (cond
   ((zero? m) n)
   (else
	(inc(addl n (dec m)))))))

(define subl
 (lambda (n m)
  (cond
   ((zero? m) n)
   (else
	(dec(subl n (dec m)))))))

(define addtup
 (lambda (tup)
  (cond
   ((null? tup) 0)
   (else
	(addl((car tup) (addtup (cdr tup))))))))

(define imul
 (lambda (n m)
  (cond
   ((zero? m) 0)()
   (else
	(addl(n (imul (dec m))))))))

(define tup+
 (lambda (tup1 tup2)
  (cond
   ((and (null? tup1)(null? tup2))(quote()))
   ((null? tup1) tup2)
   ((null? tup2) tup1)
   (else
	(cons (addl((car tup1) (car tup2) (tup+ ((cdr tup1) (cdr tup2))))))))))

(define len
 (lambda (lat)
  (cond
   ((null? lat) quote ())
   (else
	(inc(len (cdr lat)))))))

(define pick
 (lambda (n lat)
  (cond
   ((zero? dec(n)) (cdr lat))
   (else
	(pick (dec(n) (cdr lat)))))))

(define rempick
 (lambda (n lat)
  (cond
   ((zero? dec(n)) (cdr lat))
   (else
	(cons (car lat) (rempick (dec(n) (cdr lat))))))))

(define no-nums
 (lambda (lat)
  (cond
   ((null? lat) (quote()))
   (else
	(cond
	 ((number? (car lat) (cdr lat))
	  (no-num(cdr lat)))
	 (else
	  (cons (car lat) (no-nums (cdr lat)))))))))

(define all-nums
 (lambda (lat)
  (cond
   ((null? lat) (quote()))
   (else
	cond(
	((number? (car lat) (cons (car lat) (all-nums(cdr lat)))))
	(else
	 (all-nums(cdr lat))))))))

(define eqan?
 (lambda (a1 a2)
  (cond
   ((and(number? al)(number? a2) (= a1 a2)))
   ((or(number? a1)(number? a2))#f)
   (else
	(eq? a1 a2)))))

(define occur
 (lambda (a lat)
  (cond
   ((null? lat)0)
   (else
	(cond
	((eq? (car lat) a) (inc a(occur (cdr lat))))
	(else
	 (occur a(cdr lat)))))))))

(define one?
  (lambda (n)
    (= n 1)))

(define rempick
  (lambda (n lat)
    (cond
     ((one? n) (cdr lat))
     (else
      (cons(car lat) rempick(dec(n) (cdr lat)))))))

(define insertr
  (lambda (new old l)
    ((null? l) (quote ())
     ((atom? (car l)) 
      (cond
       ((eq? (car l) old)
	(cons old (cons new insertr new old(cdr l)) ))
       (else
	(cons (car l) insertr new old (cdr l))))))
    (else
     (cons (insertr new old (car l)) (insertr new old (cdr l))))))

(define occur*
 (lambda (a l)
  (cond
   ((null? l) 0)
   ((atom? (car l)) 
	(cond
	 ((eq? (car l) a) inc(occur* a (cdr l)))
	 (else (occur* a (cdr l)))))
   (else
	(addl((occur* a (car l)) (occur* a (cdr l))))))))

(define subst
 (lambda (new old l)
  (cond
   ((null? l) (quote)())
   ((atom? (car l))
	(cond
	 ((eq? (car l) old) (cons new subst*( new old (cdr l))))
	 (else (cons (car l) subst*(new old (cdr l))))))
   (else
	(cons (subst* new old (car l) (subst* new old (cdr l))))))))

(define insertl
 (lambda (new old l)
  (cond
   ((null? l) (quote()))
   ((atom? (car l)
	 (cond
	  ((eq? (car l) old)
	   (cons new (cons (car l) insertl(new old (cdr l)))))
	  (else
	   (cons (car l) insertl(new old (cdr l)))))))
   (else
	(cons (insertl (new old (car l))) (insertl(new old (cdr l))))))))

(define member*
 (lambda (a l)
  (cond
   ((null? l) (#f))
   ((atom? (car l)
	 ( or (eq ? (car l) a))
	  ((member* a(cdr l)))))
   (else (or (member* a (car l) (member* a (cdr l))))))))

(define leftmost
 (lambda (l)
  (cond
   ((atom? (car l)) (car l))
   (else (leftmost (car l))))))

(define eqlist?
 (lambda (l1 l2)
  (cond
   ((and (null? l1) (null? l2)) #t)
   ((or (null? l1) (null? l2) #f))
   ((and (atom? (car l1)) (atom? (car l2))) 
    (and (eqan?(car l1) (car l2)) (eqlist? (cdr l1) (cdr l2))))
   ((or (atom? (car l1)) (atom? l2))) #f)
   (else
	(and (eqlist? (car l1) (car l2)) (eqlist? (cdr l1) (cdr l2)))))))

(define equal?
  (lambda (s1 s2)
    (cond
     ((and (atom? s1) (atom? s2)) (eqan? s1 s2))
     ((or (atom? s1) (atom? s2)) #f)
     (else
      (eqlist? s1 s2)))))

(define eqlist?
  (lambda (l1 l2)
    (cond
     ((and (null? l1) (null? l2))) #t)
    ((or (null? l1) (null? l2))) #f)
  (else
   (and(eqal? (car l1) (car l2)) (eqlist? (cdr l1) (cdr l2)))))

(define rember
  (lambda (s l)
    (cond
     ((null? l)) (quote ()))
    ((equal? (car l) s) (cdr l))
    (else (cons (car l) (rember s (cdr l))))))


(defube numbered?
(lambda (aexp)
  (cond
   ((atom? aexp) (number? aexp))
   (else
    ((and (numbered? (car aexp)) (numbered? (car(cdr (cdr aexp))))))))))

(define 1st-sub-exp
  (lambda (aexp)
    (car aexp)))

(define 2st-sub-exp
  (lambda (aexp)
    (car(cdr aexp))))

(define value
  (lambda (nexp)
    (cond
     ((atom? nexp) nexp))
    ((eq? (car nexp)(quote+))
     (addl(value(1st-sub-exp nexp)) (value(2st-sub-exp nexp))))
    ((eq? (car nexp)(quote*))
     (X(value(1st-sub-exp nexp)) (value(2st-sub-exp nexp))))
    (else
     (!(value(1st-sub-exp nexp))(value(2st-sub-exp nexp))))))

(define set?
  (lambda (lat)
    (cond
     ((null? lat) #t))
    ((member? (car lat) (cdr lat)) #f)
    (else
     (set? (cdr lat)))))

(defome makeset
(lambda (lat)
  (cond
   ((null?) (quote()))
   ((member? (car lat) (cdr lat))
    (makeset (cdr lat)))
   (else
    (cons (car lat) makeset (cdr lat))))))

(define makeset
  (lambda (lat)
    (cond
     ((null? lat) (quote()))
     ((else
       (cons (car lat) (makeset (multirember (car lat) (cdr lat)))))))))

(define subset?
  (lambda (set1 set2)
    (cond
     ((null? set1) #t))
    ((and (member? (car set1) set2) (subset? (cdr set1) (set2))))))

(define eqset?
  (lambda (set1 set2)
    (and(subset? set1 set2) (subset? set2 set1))))

(define intersect?
  (lambda (set1 set2)
    (cond
     ((null? set1) #f))
    (else
    (or (member? (car set1) set2) #t)
     ((intersect? (cdr set1) set2)))))

(define intersect
  (lambda (set1 set2)
    (cond
     ((null? set1) (quote()))
     ((member? (car set1) set2) 
      (const (car set1) (intersect (cdr set1) set2))))
    (else
     (intersect (cdr set1) set2))))

(define union 
  (lambda (set1 set2)
    (cond
     ((null? set1) (quote()))
     ((member? (car set1) set2)
      (union(cdr set1) set2))
     (else
      (cons (car set1) (union(cdr set1) set2))))))

(define intersectall
  (lambda (set)
    (cond
    ((null? (cdr set) ) (car set))
    (else
     (intersect(car set) intersectall(cdr set))))))

(define a-pair?
 (lambda (x)
  (cond
   ((null? x) #f)
   ((atom? x) #f)
   ((null? (cdr x))#f)
   ((null? (cdr (cdr x))) #t)
   (else #f))))

(define first
 (lambda (p)
  (car p)))

(define second
 (lambda (p)
  (car (cdr p))))

(define build
 (lambda (s1 s2)
  (cons s1 (cons s2 (quote ())))))

(define fun?
 (lambda (rel)
  (set? (first rel))))

(define revpair
 (lambda (pair)
  (build (second pair) (first pair))))

(define revrel
 (lambda (rel)
  (cond
   ((null? rel) (quote()))
   (else
	(cons (revpaur (car rel) (revrel (cdr rel))))))))

(define rember-f
 (lambda (test? a l)
  (cond
   ((null? l) (quote()))
   ((test? (car l) a) (cdr l))
   (else
	(cons (car l) (rember-f test? a (cdr l)))))))

(define rember-f
 (lambda (test?)
  (lambda (a l)
   (cond
	((null? l) (quote()))
	((test? (car l) a) (cdr l))
	(else
	 (cons (car l) ((rember-f test?) a (cdr l))))))))

(define insterl
 (lambda (test?)
  (lambda (new old l)
   (cond
	((null? l) (quote ()))
	((test? (car l) old) (cons new (cons (cdr l))))
	(else
	 (cons (car l) ((insterl test?) new  old (cdr l))))))))

(define insertr
 (lambda (test?)
  (lambda (new old l)
   (cond
	((null? l) (quote()))
	((test? (car l) old)
	 (cons old (cons new (cdr l))))
	(else
	 (cons (car l) ((insertr test?) new old (cdr l))))))))

(define seql
 (lambda (new old l)
  (cons new (cons old l))))

(define seqr
 (lambda (new old l)
  (cons old (cons new l))))

(define insertg
 (lambda (seq?)
  (lambda (new old l)
   (cond
   ((null? l) (quote()))
   ((seq? (car l) old) (seq new old (cdr l)))
   (else
	(cons (car l) ((insertg test?) new old (cdr l))))))))

(define inserl (insertg seql))

(define insertl
 (insertg
  (lambda (new old l)
   (cons new (cons old l)))))

(define atom_to_func
 (lambda (x)
  (cond
  ((eq? x(quote +)) +)
  ((eq? x(quote X)) X)
  (else
    /))))

(define value
 (lambda (nexp)
  (cond
   ((atom? nexp) nexp)
   (else
	((atom_to_func(operator nexp)) 
	 value(1st-sub-exp nexp) 
	 value(2st-sub-exp nexp))))))

(define multirember-f
 (lambda (test?)
  (lambda (a lat)
   (cond
	((null? lat) (quote)())
	((test? (car lat) a)
	 ((multirember-f test?) a (cdr lat)))
	(else
	 (cons (car lat) ((multrirember-f test?) a (cdr lat))))))))

(define multrember-eq?
 (multirember-f test?))

(define eq?-tuna
 (eq? -c (quote tuna)))

(define multiremberT
 (lambda (test? lat)
  (cond
   ((null? lat)(quote()))
   ((test? (car lat))
	(multiremberT test? (cdr lat)))
   (else
	(cons (car lat) (multiremberT test? (cdr lat)))))))

(define multiinsertlrco
 (lambda (new oldl oldr lat col)
  (cond
   ((null? lat) (quote 0 0))
   ((eq ? (car lat) oldl)
	(multiinsertlrco new oldl oldr (cdr lat) 
	 (lambda (newlat l r)
	  (col (cons new (cons oldl newlat)) (inc l) r))))
   ((eq? (car lat) oldr)
	(multiinsertlrco new oldl oldr (cdr lat) 
	 (lambda (newlat l r)
	  (col (cons oldr (cons new newlat)) l (inc r)))))
   (else
	(multiinsertlrco new oldl oldr (cdr lat) 
	 (lambda (new lat l r)
	  (col (cons (car lat) newlat) l r)))))))

(define events-only
 (lambda (l col)
  (cond
   ((null? l) (quote()))
   ((atom? (car l))
	(cond
	((event? (car l)) 
	 (events-only (cdr l) 
	  (lambda (newl p s) 
	   (col (cons (car l) newl)
		(x (car l) p )s)))
	 (else
	  (events-only (car l) 
	   (lambda (newl p s)
		(col newl p (+ (car l) s))))))))
   (else
	(event?-only (cdr l)
	 (lambda (al ap as)
	  (events-onlyu (cdr l)
	   (lambda (dl dp ds)
		(col (cons al dl)
		 (X ap dp)
		 (+ as ds))))))))))

(define keep-looking
 (lambda (a sorn lat)
  (cond
   ((number? sorn)
	(keep-looking a (pick sorn lat) lat))
   (else
	(eq? sorn a)))))

(define looking
 (lambda (a lat)
  (keep-looking a (pick 1 lat) lat)))

(define shift
 (lambda (pair)
  (buld (first(first pair)) (build(second(first pair)) (second pair)))))

(define algin
 (lambda (pora)
  (cond
   ((atom? pora) pora)
   ((a-pair? (first para)) (algin (shift para)))
   (else
	(build (first pora) (align (second pora)))))))

(define length
 (lambda (pora)
  (cond
   ((atom? pora)1)
   (else
	(+ (length (first pora)) (length (second pora)))))))

(define weight
 (lambda (pora)
  (cond
   ((atom? pora) 1)
   (else
	(+ ( * (weight (first pora)) 2) (weight (second pora)))))))

(define suffle
 (lambda (pora)
  (cond
   ((null? pora) pora)
   ((a-pair? (first pora))
	(suffle (revpair pora)))
   (else
	(build (first pora) (suffle (second pora)))))))

(define a
 (lambda (n m)
  (cond
  ((zero? n) (inc m))
  ((zero? m) (a (dec(n)) 1))
  (else
   (a (dec n) (a n (dec m)))))))

(lambda (mk-len) (mk-len eternity)
 (lambda (len)
  (lambda (l)
   (cond
	((null? l) 0)
	(else
	 (inc (len (cdr l))))))))

((lambda (mk-len) (mk-len mk-len))
 (lambda (mk-len)
  (lambda (l)
   (cond
	((null? l) 0)
	(else
	 (inc (mk-len (cdr l))))))))

(lambda (mk-len) (mk-len mk-len)
  (lambda (mk-len)
    (lambda (l)
      (cond
       ((null? l) 0))
      (else
       (inc (mk-len mk-len (cdr l)))))))

(lambda (le)
  ((lambda (mk-len) (mk-len mk-len)
     (lambda (mk-len)
       (le(lambda (x) ((mk-len mk-len) x))))
     (lambda (len)
       (lambda (l)
	 (cond
	  ((null? l)0)
	  (else
	   (inc(len(cdr l))))))))))

(define lookup-in-entr-help
  (lambda (name names values entry-f)
    (cond
     ((null? names) (entry-f name))
     ((eq? (car names) name) (car values))
     (else
      (lookup-in-entry-help name (cdr names) (cdr values) entry-f)))))

(define lookup-in-entry
  (lambda (name entry entry-f)
    (lookup-in-entry-help name (first entry) (second entry) entry-f)))

(define lookup-in-talbe
  (lambda (name table table-f)
    (cond
     ((null? table) (table-f name))
     (else
      (lookup-in-entry name (car table) (lambda (name) (lookup-in-table name (cdr table) table-f)))))))

(define atom-to-action
  (lambda (e)
    (cond
     ((number? e) const)
     ((eq? e #t) const)
     ((eq? e #f) const)
     ((eq? e (quote cons)) const)
     ((eq? e (quote car)) const)
     ((eq? e (quote cdr)) const)
     ((eq? e (quote null?)) const)
     ((eq? e (quote eq?)) const)
     ((eq? e (quote atom?)) const)
     ((eq? e (quote zero?)) const)
     ((eq? e (quote inc)) const)
     ((eq? e (quote dec)) const)
     ((eq? e (quote number?)) const)
     (else (identifier)))))

(define list-to-action
  ((lambda (e)
     (cond
      ((atom? (car e))
       (cond
	((eq? (car e) (quote quote)) quote)
	((eq? (car e) (quote lambda)) lambda)
	((eq? (car e) (quote cond)) cond)
	(else application)))
      (else application)))))

(define meaning 
  (lambda (e table)
    ((expression-t-action e) e table)))

(define value
  (lambda (e)
    (meaning e (quote()))))

(define const
  (lambda (e table)
    (cond
     ((number? e) e)
     ((eq? e #t) #t)
     ((eq? e #f) #f)
     (else
      (build (quote primitive) e)))))

(define quote
  (lambda (e table)
    (text-of e)))

(define text-of second)

(define identifier
  (lambda (e table)
    (lookup-in-table e table initial-table)))

(define initial-table
  (lambda (name)
    (car (quote ()))))

(define table-of first)
(define formals-of second)
(define body-of third)

(define else?
 (lambda (x)
  (cond ((atom? x) (eq? x (quote else)))
   (else #f))))

(define question-of first)
(define answer-of second)

(define evcon
 (lambda (lines table)
  (cond
   ((else? (question-of (car lines)))
	((meaning (answer-of (car lines)) table))
	((meaning (question-of (car lines)) table)
	 (meaning (ansewer-of (car lines)) table))
	(else (evcon (cdv lines) table))))))

(define cond
 (lambda (e table)
  (evcon (cond-lines-of e) table)))
(define cond-lines-of cdr)

(define evlis
 (lambda (args table)
  (cond ((null? args) (quote()))
   (else
	(cons (meaning (car args) table)
	 (evlis (cdr args) table))))))

(define application
 (lambda (e table)
  (apply
   (meaning (function-of e) table)
   (evlis(arguments-of e) table))))

(define function-of car)
(define arguments-of cdr)

(define primitive?
 (lambda (x)
  (eq? (first l) (quote primitive))))

(define non-primitive?
 (lambda (x)
  (eq? (first l) (quote non-primitive))))

(define apply
 (lambda (fun vals)
  (cond
   ((primitive? fun)
	(apply-primitive (second fun) vals))
   ((non-primitive? fun)
	(apply-closure (second fun) vals)))))
