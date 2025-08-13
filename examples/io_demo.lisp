;; I/O Demonstration
;; This file shows the basic I/O primitives in action

;; Print with newline
(print "Hello, World!")
(print 42)
(print '(a b c))

;; Display without newline, then add manual newline
(display "Display: ")
(display "concatenated ")
(display "output")
(newline)

;; Using I/O in expressions
(define result (+ (print 10) (print 20)))
(print result)

;; Newline by itself
(newline)
(print "Done!")