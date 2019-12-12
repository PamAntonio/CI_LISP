#Author: Deanne Pamela Antonio

#Project: ciLisp 

Through the course of this project, the author will implement an interpreter for *CI LISP*, a *Lisp*-like language.


A. Task 1 - Done

 Tokenization, Parsing and Evaluation of arithmetics in Cambridge Polish Notation
 
 Finished all TODO tasks.

	/Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/Pamela_Antonio_CILISP/cmake-build-debug/cilisp

	> (mult 1 2)
	Data: 2.000000
	Type: Double
	
	> (div 5 6)
	Data: 0.833333
	Type: Double
	
	> (add(div 3 4) 4)
	Data: 4.750000
	Type: Double
	
	> quit

	Process finished with exit code 0 

B. Task 2 - Done

 Implemented evalSymbolNode, createSymbolTableNode, and let token

	/Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/Pamela_Antonio_CILISP/cmake-build-debug/cilisp

    > (add ((let (abcd 1)) (sub 3 abcd)) 4)
    Data: 6
    Type: Integer

    > (mult ((let (a 1) (b 2)) (add a b)) (sqrt 2))
    Data: 4.242641
    Type: Double
    
    > (add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))
    Data: 5.398346 
    Type: Double
    
    > quit

	Process finished with exit code 0 
	
C. Task 3 - Done
 
 Made some changes to the eval function for the symbol type to check for precision loss
 
 	/Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/Pamela_Antonio_CILISP/cmake-build-debug/cilisp
 
    > ((let (int a 1.25))(add a 1))
    WARNING: Warning: Precision loss for assignment 'a'
    Data: 2 
    Type: Integer
    
    > quit

	Process finished with exit code 0 
	
D. Task 4 - Done

 Implemented print at eval function, separated the printRetVal method to two methods (val and type)
 
  	/Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/Pamela_Antonio_CILISP/cmake-build-debug/cilisp
  	
  	> (print (mult 1 2) (add 2.0 9))
    Type: Integer
    
    2 
    
    Type: Double
    
    11.00 
    
    > (pow 3 4)
    81 
    
    > quit
    
    Process finished with exit code 0

E. Task 5 - Done
 
 Developed eval function to allow for arbitrary parameters, had trouble with printRetVal
 
   	/Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/Pamela_Antonio_CILISP/cmake-build-debug/cilisp
   	
    > (add 1 2 3 4 5)
    15 
    
    > (add 1)
    ERROR: need more parameters: add1 
    
    > (print 1 2 3)
    1 
    Type: Integer
    2 
    Type: Integer
    3 
    Type: Integer
    
    3
    
    > quit
        
    Process finished with exit code 0

F. Task 6 - Done

 Fixed retVal issue
 Fixed read function in evalFuncNode
 
    /Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/CI_LISP/cmake-build-debug/cilisp
    
    > ((let (int a (read)) (double b (read)) (c (read)) (d (read))) (print a b c d))
    read :=3
    read :=5.0
    read :=10
    read :=5.175
    print: 3, 5.00, 10, 5.17,
    
    This is the last item, printed
    
    Type: Double 
    5.175000
    
    > ((let (a 0)) (cond (less (rand) 0.5) (add a 1) (sub a 1)))
    Type: Integer 
    -1
    
    > ((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))
    read :=4
    print: 0.46,
    Type: Double 
    0.458650

G. Task 7 - Done

 Let function extended to take in lambda


    /Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/CI_LISP/cmake-build-debug/cilisp
    
    > ((let (double myFunc lambda (x y) (mult (add x 5) (sub y 2)))) (sub (myFunc 3 5) 2))
    Type: Integer 
    22
    
    > ((let (f lambda (x y) (add x y)))(f (sub 5 2) (mult 2 3))) 
    Type: Integer 
    9
    
    > quit
    
    Process finished with exit code 0
    
 
H. Task 8 - Done
    
    /Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/CI_LISP/cmake-build-debug/cilisp
    
    > ((let (int a 1)(f lambda (x y) (add x y)))(f 2 (f a 3)))
    Type: Integer 
    6
    
    > quit
    
    Process finished with exit code 0

I. Task 9 - Done

    /Users/carelessluck/Desktop/Pamela_Antonio_CI_LISP/CI_LISP/cmake-build-debug/cilisp
    
    > ((let (countdown lambda (x) (cond (greater x 0) (countdown (print (sub x 1))) (print x)))) (countdown 10))
    print: 9,
    print: 9,
    print: 8,
    print: 8,
    print: 7,
    print: 7,
    print: 6,
    print: 6,
    print: 5,
    print: 5,
    print: 4,
    print: 4,
    print: 3,
    print: 3,
    print: 2,
    print: 2,
    print: 1,
    print: 1,
    print: 0,
    print: 0,
    print: 0,
    Type: Integer 
    0
    
    > ((let (gcd lambda (x y) (cond (greater y x) (gcd y x) (cond (equal y 0) (x) (gcd y (remainder x y)))))) (gcd 95 55))
    Type: Integer 
    5
    
    > quit
    
    Process finished with exit code 0