; ----------------- 不重复的集和 -------------------------
; 是否在集和里
(define (element_of_set? x set)
 (cond
  ((null? set) #f)
  ((equal? x (car set)) #t)
  (else
   (element_of_set? x (cdr set)))))

; 邻阶集 插入到集和
(define (adjoin_set x set)
 (if (element_of_set? x set)
  set
  (cons x set)))

; set交集
(define (intersection_set set1 set2)
 (cond
  ((or (null? set1) (null? set2)) 
   '())
  ((element_of_set? (car set1) set2) 
   (cons (car set1) (intersection_set (cdr set1) set2)))
  (else
   (intersection_set (cdr set1) set2))))

; 并集
(define (union_set set1 set2)
 (define (iter input result)
  (if (null? input)
   (reverse result)
   (let ((current_element (car input))
		 (remain_element (cdr input)))
	(if (element_of_set? current_element result)
	 (iter remain_element result)
	 (iter remain_element (cons current_element result))))))
 (iter (append set1 set2) '()))


;---------------------------- 重复的集和 --------------------------
(define (element_of_multiset? x set)
 (element_of_set? x set))

(define (adjoin_multiset x set)
 (cons x set))

(define (intersection_multiset set1 set2)
 (define (iter set result)
  (if (or (null? set) (null? set2))
   (reverse result)
   (let ((current_element (car set))
		 (remain_element (cdr set)))
	(if (and (element_of_multiset? current_element set2)
		 (not (element_of_multiset? current_element result)))
	 (iter remain_element (const current_element result))
	 (iter remain_element result)))))
 (iter set1 '()))

(define (union_multiset set1 set2)
 (union_set set1 set2))

;----------------------------- 已排序的集合 ------------------------
(define (element_of_set? x set)
 (cond
  ((null? set) #f)
  ((= x (car set)) #t)
  ((< x (car set)) #f)
  (else
  (element_of_set? x (cdr set)))))

(define (intersection_set set1 set2)
 (if(or (null? set1) (null? set2)
	 '()
	 (let ((x1 (car set1)) (x2 (car set2)))
	  (cond
	   ((= x1 x2)
		(cons x1 (intersection_set (cdr set1) (cdr set2))))
	   ((< x1 x2)
		(intersection_set (cdr set1) set2))
	   ((< x2 x1)
		(intersection_set set1 (cdr set2))))))))

(define (adjoin_set x set)
 (union_set (list x) set))

(define (union_set set another)
 (cond ((and (null? set) (null? another)) '())
  ((null? set) another)
  ((null? another) set)
  (else
   (let ((x (car set)) (y (car another)))
	(cond 
	 ((= x y)
	  (cons x (union_set (cdr set) (cdr another))))
	 ((< x y)
	  (cons x (union_set (cdr set) another)))
	 ((> x y)
	  (cons y (union_set set (cdr another)))))))))

;------------------------------- 用树表示集合 ----------------------
(define (entry tree) (car tree)) ; 取节点

(define (left tree) (cadr tree))
(define (right tree) (caddr tree))

(define (make_tree entry left right)
 (list entry left right))

(define (element_of_set? x set)
 (cond
  ((null? set) #f)
  ((= x (entry set)) #t)
  ((< x (entry set))
   (element_of_set? x (left set)))
  ((> x (entry set))
   (element_of_set? x (right set)))))

(define (adjoin_set x set)
 (cond
  ((null? set) (make_tree x '() '()))
  ((< x (entry set))
   (make_tree (entry set) (adjoin_set x (left set)) (right set)))
  ((> x (entry set))
   (make_tree (entry set) (left set) (adjoin_set x (right set))))))

(define (tree_to_list1 tree)
 (if (null? tree) '()
  (append (tree_to_list (left tree))
   (cons (entry tree) (tree_to_list (right tree))))))

(define (tree_to_list tree)
 (define (copy_to_list tree res)
  (if (null? tree)
   res
   (copy_to_list (left tree) 
	(cons (entry tree) (copy_to_list (right tree) res))))))

(define (list_to_tree2 elements)
 (define (partial_tree elts n)
  (if (= n 0)
   (cons '() elts)
   (let ((left_size (quotient (- n 1) 2)))
	(let ((letf_result (partial_tree elts left_size)))
	 (let ((left_tree (car left_result))
		   (non_left_elts (cdr left_result))
		   (right_size (- n (+ left_size 1))))
	  (let ((this_entry (car non_left_elts))
			(right_result (partial_tree (cdr non_left_elts) right_size)))
	   (let ((right_tree (car right_result))
			 (remaining_elts (cdr right_result)))
		(cons (make_tree this_entry left_tree right_tree) remaining_elts))))))))
 (car (partial_tree elements (length elements))))

(define (intersection_tree another)
 (list_to_tree (intersection_set (tree_to_list2 tree)
				(tree_list2 another))))

(define (union_tree tree another)
 (list_to_tree (union_set (tree_to_list2 tree) (tree_to_list2 another))))

; 查找数据
(define (lookup given_key tree_records)
 (if (null? tree_records) #f
  (let ((entry_key (key (entry tree_records))))
   (cond ((= given_key entry_key)
		  (entry tree_records))
	((> given_key entry_key)
	 (lookup given_key (right tree_records)))
	((< given_key entry_key)
	 (lookup given_key (left tree_records)))))))
