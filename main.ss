;;; Requires csocket.so, built from csocket.c.
(load-shared-object "./csocket.so")

;;; routines defined in csocket.c

(define bytes-ready?
  (foreign-procedure "bytes_ready" (int)
                     boolean))

(define c-get-error
  (foreign-procedure "get_error" (int)
                     string))

(define c-read
  (foreign-procedure "c_read" (int u8* int)
                     int))

(define c-write
  (foreign-procedure "c_write" (int u8* int)
                     int))

(define c-close
  (foreign-procedure "c_close" (int)
                     int))

(define c-set-read-timeout
  (foreign-procedure "set_read_timeout" (int int)
                     int))

(define c-set-write-timeout
  (foreign-procedure "set_write_timeout" (int int)
                     int))

(define c-dial
  (foreign-procedure "dial" (string int)
                     int))

(define check
 ; signal an error if status x is negative, using c-error to
 ; obtain the operating-system's error message
  (lambda (who x)
    (if (< x 0)
        (error who (c-get-error x))
        x)))

; Example: (bytevector-slice #vu8(1 2 3 4 5) 3 2) => #vu8(4 5)
(define (bytevector-slice v start n)
  (let ([slice (make-bytevector n)])
    (bytevector-copy! v start slice 0 n)
    slice))

(define (write-string socket s)
  (let ([v (string->bytevector s (current-transcoder))])
    (check 'write (c-write socket v (bytevector-length v)))))

(define (read-string socket)
  (let* ([buffer-size 1024]
         [buf (make-bytevector buffer-size)]
         [n (check 'read (c-read socket buf buffer-size))])
    (if (not (= n 0))
        (bytevector->string (bytevector-slice buf 0 n)
                            (current-transcoder))
        (eof-object))))

(define (close-socket socket)
  (check 'close (c-close socket)))

(define (main)
  (define socket (check 'dial (c-dial "irc.freenode.net" 6667)))
  ;(c-set-read-timeout socket 300)
  (write-string socket "PASS *\n")
  (write-string socket "NICK ping_test23852\n")
  (write-string socket "USER ping 8 * :ping\n")
  (write-string socket "JOIN #42!\n")
  (write-string socket "PRIVMSG #42! ping\n")
  (write-string socket "QUIT\n")
  (do ([msg (read-string socket) (read-string socket)])
      ((eof-object? msg) (display "DONE!\n"))
    (display msg))
  (close-socket socket))
(main)
