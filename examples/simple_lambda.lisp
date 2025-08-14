(define test-function (lambda (x)
  (define helper1 (lambda (y) (+ y 1)))
  (helper1 x)))

;; Expected: 4 (3 + 1 = 4)
(test-function 3)
