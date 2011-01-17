;Simple loop
(loop 
  (princ "type something")
  (force-output)
  (read)) ;loops forever!

;do
(loop for i below 5 do (print i)) ;loops 0-4

;repeat
(loop repeat 5
      do (print "Prints 5 times!"))

;return
(loop for i below 10
      when (= i 5)
      return 'leaving-early
      do (print i)) ; loops until i = 5 then returns.

;initially
(loop initially
      (print 'loop-begin)
      for x below 3
      do (print x))

;finally
(loop for x below 3
      do (print x)
      finally (print 'loop-end)); NOTE -- finally does not effect that loop returns nil.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Column 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;named
;return-from
(loop named outer
      for i below 10
      do
      (progn ;NOTE PROGN!
	(print "outer")
	(loop named inner
	      for x below i
	      do (print "**inner")
	      when (= x 2)
	      do (return-from outer 'kicked-out-all-the-way)
)))

;while
(loop for i in '(0 2 4 555 6)
      while (evenp i) ; the same as breaking when condition is true.
      do (print i))

;until
(loop for i
      from 0
      do (print i)
      until (> i 3)) ; Similar to do...while



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Columns 3 & 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;with
(loop with x = (+ 1 2)
      repeat 5
      do (print x))

(defparameter salary (make-hash-table))
(setf (gethash 'bob salary) 80)
(setf (gethash 'john salary) 90)

;using
(loop 
 for person being each hash-key of salary 
 using (hash-value amt) 
 do (print (cons person amt)))

;being
;the (same as each)
;hash-key (hash-keys)
(loop for person being the hash-keys of salary do (print person))

;hash-value (hash-values)
(loop for amt being the hash-values of salary do (print amt))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Columns 5 & 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;for (as)
(loop for i
      from 0
      do (print i)
      when (= i 5)
      return 'zuchini)

;in (on)
(loop 
 for i
 in '(10 20 30 40)
 sum i)

;by
;from
(loop for i
      from 6
      to 8 by 2
      sum i)

;then
(loop repeat 5
      for x = 10.0
      then (/ x 2)
      collect x); x/(2^(0..4))

;upfrom
(loop for i
      upfrom 6
      to 8
      sum i)

;downfrom
(loop for i
      downfrom 10
      to 7
      do (print i));NOTE: This includes both 7 and 10.

; This will just be ()
(loop for i
      downfrom 1
      to 7
      do (print i))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Column 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;across
; This is similar to for i in but it works with arrays.
(loop for i
      across
      #(100 20 10)
      sum i)

;to
(loop for i
      from 6
      to 8
      sum i)

;upto
(loop for i
      from 6
      upto 8
      sum i)

;downto
(loop for i
      from 10
      downto 7
      do
      (print i))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Column 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;into
(loop for i
      in '(3 8 73 4 -5)
      minimize i
      into lowest
      maximize i
      into biggest
      finally
      (return
       (cons lowest biggest)))

;always
(loop for i
      in '(0 2 4 6)
      always (evenp i))

(loop for i
      in '(0 2 4 5)
      always (evenp i))

;never
(loop for i
      in '(0 2 4 6)
      never (oddp i))

(loop for i
      in '(0 2 4 5)
      never (oddp i))

;theris
(loop for i
      in '(0 2 4 5)
      thereis (oddp i))

(loop for i
      in '(0 2 4 6)
      thereis (oddp i))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Column 9
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;if
(loop for i
      below 5
      if (oddp i)
      do (print i))


;when
(loop for i
      below 7
      when (oddp i)
      do (print i) ; printed only when odd.
      when (evenp i)
      do (print "!") ; printed only when even.
      do (print "yep")) ; Always printed

;unless
(loop for i
      below 4
      unless (oddp i)
      do (print i))

;and
(loop for x below 5
      when (= 3 x)
      do (print "I'm at three!") ; Surprised PROGN isn't preferred method here.
      and
      do (print "Aren't you proud?")
      do (print "YAY!!!"))

;else
(loop for i
      below 5
      if (oddp i)
      do (print i)
      else
      do
      (print "!"))

;end see http://www.gigamonkeys.com/book/loop-for-black-belts.html
(loop for i
      below 10
      when (oddp i)
      do (print i)
      end
      do (print "y"))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Column 10
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;count
(loop for i
      in '(1 1 1 1)
      count i)

;sum
(loop for i
      below 5
      sum i)

;minimize
(loop for i
      in '(3 2 1 2 3)
      minimize i)

;maximize
(loop
 for i
 in '(3 1 2 3 2 1)
 maximize i)

;append
(loop for i
      below 5
      append
      (list 'z i))

;nconc -- see http://www.lispworks.com/documentation/HyperSpec/Body/f_nconc.htm
(loop for i
      below 5
      nconc
      (list 'Z i))
