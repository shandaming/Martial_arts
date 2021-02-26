; 构造一颗叶子 ：符号leaf 叶中符号 权重
(define (make_leaf symbol weight)
 (list 'leaf symbol weight))

(define (leaf? obj)
 (eq? (car obj) 'leaf))

(define (symbol_leaf x) (cadr x))
(define (weight_leaf x) (caddr x))

(define (left tree) (car tree))
(define (right tree) (cadr tree))
(define (sysmbols tree)
 (if (leaf? tree)
  (list (symbol_leaf tree))
  (caddr tree)))

(define (weight tree)
 (if(leaf? tree)
  (weight_leaf tree)
  (cadddr tree)))

; 归并2个节点
(define (make_code_tree left right)
 (list left right 
  (append (symbols left) (symbols right))
  (+ (weight left) (weight right))))

; 解码过程
(define (decode bits tree)
 (define (choose_branch bit branch)
  (cond
   ((= bit 0) (left branch))
   ((= bit 1) (right branch))
   (else (error "bad bit -- CHOOSE-BRANCE" bit))))
 (define (decode1 bits current_branch)
  (if (null? bits) '()
   (let ((next_branch (choose_branch (car bits) current_branch)))
	(if(leaf? next_branch)
	 (cons (symbol_leaf next_branch) (decode1 (cdr bits) tree))
	 (decode1 (cdr bits) next_branch)))))
 (decode1 bits tree))

(define (adjoin_set x set)
 (cond
  ((null? set) (list x))
  ((< (weight x) (weight (car set))) (cons x set))
  (else
   (cons (car set) (adjoin_set x (cdr set))))))

(define (make_leaf_set pairs)
 (if (null? pairs) '()
  (let ((pair (car pairs)))
   (adjoin_set (make_leaf (car pair) (cadr pair))
	(make_leaf_set (cdr pairs))))))
