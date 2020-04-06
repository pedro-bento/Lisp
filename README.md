# Open Lisp
Lisp Interpreter

## Example use
```Lisp
> (define l (list 1 2 3))
L

> (reverse l)
(3 2 1)

> (reverse (list 1 2 3))
(3 2 1)

> (map + l l)
(2 4 6)

> (define (incr x) (+ x 1))
INCR

> (map incr l)
(2 3 4)

> (define (fact x) (if (= x 0) 1 (* x (fact (- x 1)))))
FACT

> (fact 10)
3628800
```
