# Å - Another Programming Language
Yep, it's yet another programming language, which probably has nothing new to add.
## What's it like?
The language is heavily influenced by the C languages (and it's also written in C++).
One of the goals with the language is to introduce a more clean and, hopefully, more intuitive language.
### Small Examples:
It's currently possible to write statements like these:
```
{ 
    var x = (5+5)*5-2; 
    var y = 20; 
    x - y; 
}
```
Which will be evaluated to 28. Note the implicit return from the last statement. While there's a simple var keyword for variable declerations, we can directly specify the type.
```
{ 
    int x = 5; 
    x = x + 5; 
    x; 
}
```
Above code will, as expected, output 5. A small note for the current state of the language; types are not enforced (but checked at compile time). Additionally, variable scopes are not properly defined at the moment. Meaning the language is currently using a dynamic scope system.
For more examples of what's currently possible in the language, look inside the Main.cpp file, as it contains more examples of what's possible.
## Why?
Because designing programming languages is a fun and learnful experience.
## How does it work?
Currently the language is parsed from a string into a parse tree, which is then compiled into Å's bytecode system (Using bytes to represent operations and arguments in a simple format).
That code can then be executed by the AAVM, taking the bytecode as input.
## State of the language
Currently, the language can only parse and calculate arithmetic expressions as well as work with (mutable) variables. Additionally, the boolean type has been introduced, but is currently not very useful.