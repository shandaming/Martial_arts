(define (make_point x y) (cons x y))

(define (x_point p) (car p))
(define (y_point p) (cdr p))

(define (make_segment s e)
 (cons s e))

(define (start_segment s)
 (car s))
(define (end_segment s)
 (cdr s))

(define (mid_point_segment s)
 (make_point 
  (/ 
   (- (x_point (end_segment s)) 
				 (x_point (start_segment s))) 2)
 (/ 
  (- (y_point (end_segment s)) 
	 (y_point (start_segment s))) 2)))

(define (print_point p)
 (newline)
 (display "(")
 (display (x_point p))
 (display ",")
 (display (y_point p))
 (display ")"))
