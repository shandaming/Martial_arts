(define (cons1 x y)
 (define (dispatch m)
  (cond
   ((= m 0) x)
   ((= m 1) y)
   (else
	('()))))
 dispatch)

(define (car1 x) (x 0))
(define (cdr1 x) (x 1))

(define (cons2 x y)
 (lambda (m) (m x y)))

(define (car2 z)
 (z (lambda (p q) p)))

(define (cdr2 z)
 (z (lambda (p q) q)))
