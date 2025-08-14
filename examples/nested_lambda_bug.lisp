;; Demonstrates a bug with multiple nested lambda definitions
;; This should return 23 but instead returns #<lambda>

(define test-function (lambda (x)
  (define helper1 (lambda (y) (+ y 1)))
  (define helper2 (lambda (y) (+ y 2)))
  (+ (helper1 x) (helper2 x))))

;; Expected: 25 (11 + 1 + 11 + 2 = 25)

(define result (test-function 11))
(if (= result 25)
    (print "Success!")
    (print "Fail!"))
result
