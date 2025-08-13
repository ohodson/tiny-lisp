(define bubble-sort (lambda (lst)
  (define swap-needed? (lambda (lst)
    (if (null? (cdr lst))
        #f
        (if (> (car lst) (car (cdr lst)))
            #t
            (swap-needed? (cdr lst))))))
  
  (define bubble-pass (lambda (lst)
    (if (null? (cdr lst))
        lst
        (if (> (car lst) (car (cdr lst)))
            (cons (car (cdr lst)) (bubble-pass (cons (car lst) (cdr (cdr lst)))))
            (cons (car lst) (bubble-pass (cdr lst)))))))
  
  (if (swap-needed? lst)
      (bubble-sort (bubble-pass lst))
      lst)))

(define test-numbers '(64 34 25 12 22 11 90))
(bubble-sort test-numbers)