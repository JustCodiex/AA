# Å - Another Programming Language
Yep, it's yet another programming language, which probably has nothing new to add.
## What's it like?
The language is heavily influenced by the C languages (and it's also written in C++).
One of the goals with the language is to introduce a more clean and, hopefully, more intuitive language.
### Small Examples:
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
#### Functions
```
int test() {
    var x = 0;
    x = 28;
    x;
}
test();
```
Functions can be defined as expected. Since there's no real conecpt of files, access modifiers are currently not applicable (but planned). The code sample above will run the test function and return 28. We note that the language currently works with a loose concept of a global scope. Functions are excluded from this scope, meaning the `test();` statement is considered to be part of the global scope and is executed during runtime. Meanwhile the definition of test is NOT considered part of the global scope and will therefore not execute by itself.
```
int par(int x, int y) { 
    x - y; 
} 
par(10, 5);
```
The `par` function defined above takes two parameters `x` and `y` and then returns the resulting subtraction of `y` from `x`. We note again that the return statement is implied.
#### Main.cpp examples
For more examples of what's currently possible in the language, look inside the [Main.cpp](Main.cpp) file, as it contains more examples of what's possible.
## Why?
Because designing programming languages is a fun and learnful experience.
## How does it work?
Currently the language is parsed from a string into a parse tree, which is then compiled into Å's bytecode system (Using bytes to represent operations and arguments in a simple format).
That code can then be executed by the AAVM, taking the bytecode as input.
## State of the language
Currently, the language can only parse and calculate arithmetic expressions as well as work with (mutable) variables. Additionally, the boolean type has been introduced, but is currently not very useful.