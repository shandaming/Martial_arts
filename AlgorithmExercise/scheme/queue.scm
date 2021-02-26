; 制作一个空队列
(define (make_queue) (cons '() '()))

; 指向队列头的指针
(define (front_ptr queue) (car queue))
; 指向队列尾的指针
(define (rear_ptr queue) (cdr queue))

(define (empty_queue? queue) (null? front_ptr queue))

; 设置指针数值
(define (set_front_ptr! queue item) (set-car! queue item))
(define (set_rear_ptr! queue item) (set-cdr! queue item))

; 取队列前端数据
(define (pop queue)
 (if(empty_queue? queue) '()
  (car (front_ptr queue))))

(define (insert_queueu queue item)
 (let ((new_pair (cons item '())))
  (cond((empty_queue? queue)
		(set_front_ptr! queue new_pair)
		(set_rear_ptr! queue new_pair)) ; 2个指针都指向new_pair
   (else(set-cdr! (rear_ptr queue) new_pair)
	(set_rear_ptr! queue new_pair)
	queue))))

(define (delete_queue queue)
 (cond
  ((empty_queue? queue) '())
  (else
   (set_front_ptr! queue (cdr (front_ptr queue))) ; 将第一个参数设置为空
   queue)))
