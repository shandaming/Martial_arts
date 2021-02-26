; 有理数表示
(define (make_rat n d)
 (let ((g (gcd n d)))
  (cons (/ n g) (/ d g))))

; 取分子
(define (number n) (car n))

; 取分母
(define (denom n) (cdr n))

; 推迟创建有理数
(define (make_rat1 x y) (cons x y))

(define (number1 x)
 (let ((g (gcd (car x) (cdr x))))
  (/ (car x) g)))

(define (denom x)
 (let ((g (gcd (car x) (cdr x))))
  (/ (cdr x) g)))

; 打印有理数
(define (print_rat n)
 (display newline)
 (display (number n))
 (display "/")
 (display (denom n)))

; 有理数+ - * /
(define (add_rat x y)
 (make_rat (+ (* (number x) (denom y))
			(* (number y) (denom x)))
  (* (denom x) (denom y))))

(define (sub_rat x y)
 (make_rat (- (* (number x) (denom y))
			(* (number y) (denom x)))
  (* (denom x) (denom y))))

(define (mul_rat x y)
 (make_rat (* (number x) (number y))
  (* (denom x) (denom y))))

(define (div_rat x y)
 (make_rat (* (number x) (denom y))
  (* (denom x) (number y))))

(define (equal_rat x y)
 (= (* (number x) (denom y))
  (* (number y) (denom x))))

(define one_half (make_rat 1 2))
