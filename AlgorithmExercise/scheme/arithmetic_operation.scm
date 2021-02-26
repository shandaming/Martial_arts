; 自增
(define (inc n)
 (if (number? n) (+ n 1)
  '()))

; 自减
(define (dec n)
 (if (number? n) (- n 1)
  '()))

; 加
(define (add n1 n2)
 (if (and (number? n1) (number? n2)) (+ n1 n2)
  '()))

; 减
(define (sub n1 n2)
 (if (and (number? n1) (number? n2)) (- n1 n2)
  '()))

; 乘
(define (imul n1 n2)
 (if (and (number? n1) (number? n2)) (* n1 n2)
  '()))

; 除
(define (div n1 n2)
 (cond
  ((= n2 0) display "Divisor is 0!")
  ((and (number? n1) (number? n2)) (/ n1 n2))
  (else '())))
