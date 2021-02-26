; 递增
(define (inc n) (+ n 1))

; 递减
(define (dec n) (- n 1))

(define (A x y)
 (cond
  ((= y 0) 0)
  ((= x 0) (* 2 y))
  ((= y 1) 2)
  (else
   (A(- x 1) (A x(- y 1))))))

(define (cc amount kinds)
 (cond
  ((= amount 0) 1)
  ((or (< amount 0) (= kinds 0)) 0)
  (else
   (+ (cc amount (- kinds 1))
	(cc(- amount (first kinds)) kinds)))))

(define (first kinds)
 (cond
  ((= kinds 1) 1)
  ((= kinds 2) 5)
  ((= kinds 3) 10)
  ((= kinds 4) 25)
  ((= kinds 5) 50)))

(define hello
 (lambda (name)
  (string-append "hello " name "!")))

(define (short_hello name)
 (string-append "hello " name "!"))

(define (len l)
 (if(null? l) 0
  (inc(len (cdr l)))))

(define (sum l)
 (if (null? l) 0
  (+ (car l) (sum (cdr l)))))

(define sublat
 (lambda (l x)
 (cond
  ((null? l) '())
  ((= (car l) x) (cdr l))
  (else
   (sublat (cdr l) x)))))

(define factorial
 (lambda (n)
  (fact 1 1 n)))

(define fact
 (lambda (product counter max_count)
  (cond
   ((> counter max_count) product)
   (else
	(fact (* product counter) (+ counter 1) max_count)))))

(define (fib n)
 (cond
  ((= n 0) 0)
  ((= 1 n) 1)
  (else
   (+ (fib (- n 1)) (fib (- n 2))))))

(define (fib n)
 (fib_ 1 0 n))

(define (fib_ a b count)
 (cond
  ((= count 0) b)
  (else
   (fib_(+ a b) a (- count 1)))))

(define (expt1 b n)
 (cond
  ((= n 0) 1)
  (else
   (* b (expt1 b (- n 1))))))

(define (expt2 b n)
 (expt3 b n 1))

(define (expt3 b count product)
 (cond
  ((= count 0) product)
  (else
   (expt3 b (- count 1) (* product b)))))

; 最大公约数
(define (gcd a b)
 (if (= b 0) a
  (gcd b (remainder a b)))) ; 秋玉树

(define (cube n)
 (* n n n))

(define (sum_cubes a b)
 (if (> a b) 0
  (+ (cube a) (sum_cubes (+ a 1) b))))

; 第归
(define (sum term a next b)
 (if (> a b) 0
  (+ (term a) (sum term (next a) next b))))

; 迭代
(define (sum1 term a next b)
 (define (iter a result)
  (if (> a b) (+ 0 result)
   (iter (next a) (+ result (term a)))))
   (iter a 0))

(define (sum_ a b)
 (sum1 cube a inc b))

(define (find low high)
 (let((mid (/ (- high low) 2)))
  (if (= mid 5) 5
   (cond
	((< mid 5) (find mid high))
	((> mid 5) (find low mid))))))

(define (avg f)
 (lambda (x) (+ x (f x))))

(define (list_ref l n)
 (if (= n 0) (car l)
  (list_ref (cdr l) (- n 1))))

(define (reverse1 l)
 (cond
  ((null? l) '())
  (else
   (cons (reverse1 (cdr l)) (car l)))))

(define (reverse2 l) (iter2 l '()))
(define (iter2 l  res)
 (if (null? l) res
  (iter (cdr l) (cons (car l) res))))

(define (square x) (* x x))

(define (square_list l)
 (if(null? l) '()
  (cons (square(car l)) (square_list (cdr l)))))

(define (square_list1 l)
 (map square l))

(define (square_list3 l)
 (define (iter item res)
  (if(null? item) 
   res
   (iter (cdr item) 
	(cons (square (car item)) 
	 res))))
 (iter (reverse l) '()))

(define (for_each func l)
 (cond
  ((null? l) '())
  (func (car l))
  (for_each func (cdr l))))

(define (count_leaves l)
 (cond
  ((null? l) 0)
  ((not (pair? l)) 1)
  (else
   (+ (count_leaves (car l)) (count_leaves (cdr l))))))

(define (deep-reverse l)
 (cond
  ((null? l) 
   '())
  ((not (pair? l))
   l)
  (else
   (reverse (list (deep-reverse (car l))
			 (deep-reverse (cdr l)))))))

(define (tree_reverse l)
 (define (iter remained_items res)
  (if(null? remained_items) res
   (iter (cdr remained_items) 
	(cons (if (pair? (car remained_items))
		   (tree_reverse (car remained_items))
		   (car remained_items))
	 res))))
 (iter l '()))

(define (fringe l)
 (cond
  ((null? l) '())
  ((not (pair? l)) (list l))
  (else
  (append (fringe (car l)) 
   (fringe (cdr l))))))

(define (scale_tree tree factor)
 (cond
  ((null? tree) '())
  ((not (pair? tree)) (* tree factor))
  (else
   (cons (scale_tree (car tree) factor)
	(scale_tree (cdr tree) factor)))))

(define (scale_tree1 tree factor)
 (map (lambda (sub_tree)
	   (if(pair? sub_tree) 
		(scale_tree1 sub_tree factor)
		(* sub_tree factor))) tree))

(define (tree_map func tree)
 (cond
  ((null? tree) '())
  ((not (pair? tree)) (func tree))
  (else
   (cons (tree_map func (car tree)) 
	(tree_map func (cdr tree))))))

(define (square_tree tree)
 (tree_map square tree))

(define (subsets s)
 (if(null? s) (list '())
  (let ((rest (subsets (cdr s))))
   (append rest (map (lambda (x) 
					  (cons (car s) x))
				 rest)))))
