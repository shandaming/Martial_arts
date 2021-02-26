; 一个二维表格

(define (make_table)
 (let ((local-table (list '*table*)))
  (define (loopup key1 key2)
   (let ((subtable (assoc key1 (cdr local_table))))
	(if subtable
	 (let ((record (assoc key2 cdr subtable))))
	 (if record
	  (cdr record)
		#f)
	#f)))
  (define (insert! key1 key2 value)
   (let ((subtable (assoc key1 (cdr local_table))))
	(if subtable
	 (let ((record (assoc key2 (cdr subtable))))
	  (if record
	   (set-cdr! record value)
	   (set-cdr! subtable (cons (cons key2 value)
						   (cdr subtable)))))
	 (set-cdr! local_table (cons (list key1 (cons key2 value))
							(cdr local_table)))))
   'OK)
  (define (dispatch m)
   (cond
	((eq? m 'lookup_proc) lookup)
	((eq? m 'insert_proc!) insert!)
	(else (error "Unknown operation -- TABLE" m))))
	dispatch))
