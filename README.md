# Å - Another Programming Language
Yep, it's yet another programming language, which probably has nothing new to add.
## What's it like?
The language is heavily influenced by the C languages (and it's also written in C++).
One of the goals with the language is to introduce a more clean and, hopefully, more intuitive language.
### The state of the language (With Small Examples):
#### Arithmetic and variables
It's currently possible to write statements like these:
```
{ 
    var x = (5+5)*5-2; 
    var y = 20; 
    x - y; 
}
```
Which will be evaluated to 28 (Because the language does abide by a standard order of operations ruleset). Note the implicit return from the last statement. While there's a simple var keyword for variable declerations, we can directly specify the type.
```
{ 
    int x = 5; 
    x = x + 5; 
    x; 
}
```
Above code will, as expected, output 10. A small note for the current state of the language; types are not enforced (but checked at compile time). Additionally, variable scopes are not properly defined at the moment. Meaning the language is currently using a dynamic scope system.
#### Flow-Control
The language also supports flow control elements like `if`, `else if`, and `else` controls as well as loop controls such as `for`, `while`, and `do-while`. Examples of if-statements can be seen in the recursion showcase. Loops in the language looks:
```
int foo() {
    var x = 0;
    for (int i = 0; i < 10; i = i + 1) {
        x = x + i * 2;
    }
    x;
}
foo();
```
[For-Loop.aa](examples/for-loop.aa)  
```
int foo() {
    var i = 0;
    while (i < 10){
        i = i + 1;
    }
    i;
}
foo();
```
[While-Loop.aa](examples/while-loop.aa)  
```
int foo() {
    var i = 0;
    do {
        i = i + 1;
    } while (i < 10)
    i;
}
foo();
```
[Do-While-Loop.aa](examples/dowhile-loop.aa)  
The three examples above are the only three loops currently supported. A `foreach` loop is planned, but is only really useful when arrays and iterators have been implemented.
#### Functions
```
int test() {
    var x = 0;
    x = 28;
    x;
}
test();
```
Functions can be defined as expected. Since there's no real conecpt of files, access modifiers are currently not applicable (but planned). The code sample above will run the test function and return 28. We note that the language currently works with a loose concept of a global execution scope. Functions are excluded from this scope, meaning the `test();` statement is considered to be part of the global scope and is executed during runtime. Meanwhile the definition of test is NOT considered part of the global execution scope and will, therefore, not execute by itself.
```
int par(int x, int y) { 
    x - y; 
} 
par(10, 5);
```
The `par` function defined above takes two parameters `x` and `y` and then returns the resulting subtraction of `y` from `x`. We note again that the return statement is implied. Lastly, recursion is possible with the language.
```
int factorial(int n) {
    if (n <= 1) {
        1;
    } else {
        n * factorial(n - 1);
    }
}
factorial(4);
```
[Recursion.aa](examples/recursion.aa)  
We can here see, using if-statements that it's possible to calculate the factorial of a number through recursion.
## Why?
Because designing programming languages is a fun and learnful experience.
## How does it work?
Currently the language is parsed from a string into a parse tree, which is then compiled into Å's bytecode system (Using bytes to represent operations and arguments in a simple format).
That code can then be executed by the AAVM, taking the bytecode as input.