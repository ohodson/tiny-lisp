(define reverse-list (lambda (lst)
  (define reverse-helper (lambda (lst acc)
    (if (null? lst)
        acc
        (reverse-helper (cdr lst) (cons (car lst) acc)))))
  (reverse-helper lst ())))

(define test-list '(1 2 3 4 5))
(reverse-list test-list)