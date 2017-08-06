(define (type x)
  (cond
   ((symbol? x) "symbol")
   ((boolean? x) "boolean")
   ((integer? x) "integer")
   ((real? x) "real")
   ((number? x) "number")
   ((char? x) "char")
   ((string? x) "string")
   ((null? x)  "null")
   ((list? x) "list")
   ((pair? x) "pair")
   (else "object")))