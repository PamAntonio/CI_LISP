#Author: Deanne Pamela Antonio
#Project: ciLisp 

Through the course of this project, the author will implement an interpreter for *CI LISP*, a *Lisp*-like language.

The project is divided into tasks (which should be done sequentially).

A. Task 1 - Done

 Tokenization, Parsing and Evaluation of arithmetics in Cambridge Polish Notation

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

 Implemented evalSymbolNode, createSymbolTable

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

G. Task 7 - Todo
    
 
H. Task 8 - Todo


I. Task 9 - Todo