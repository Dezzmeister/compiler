NOTE: This was written before we learned that we didn't have to use C for the project.
I'm keeping the repo up because these data structures might be useful.

                           ========== Compiler (WIP) ==========

Folders:
    /lib - contains generic data structures that we will need
    /tests - contains unit tests and a script to run them

There isn't much else yet because the semester just started.


                                ========== Lib ===========

Generics in C are hard, but somewhat doable. See generics.txt and lib/structures.h for
better explanations of how generic data structures are implemented here and can be used.
So far we have a LinkedList, Vec, and HashMap. The LinkedList can be used as a stack,
queue, or deque.

The file lib/structures.h contains some type declarations for data structures storing
primitive types. If you need an IntLinkedList, please use the one in lib/structures.h
instead of making one yourself because this will likely cause name collisions. Make
specialized data structures yourself when you have a custom type that doesn't really
belong in structures.h. Even then, take care to make sure that you're reusing the same
specialization instead of declaring it multiple times.

                            ========== Architecture ==========

The project is currently coupled to gcc because it uses statement expressions and the
typeof operator, which are both GNU C compiler extensions. This doesn't have to be
the case though, and if we need to we can rewrite the small bits of code that use
these extensions or just omit some of the code altogether.

                               ========== Tests ==========

We will definitely need unit tests, especially as the project grows and we build parts
of the compiler. I have set up a tests directory with some example unit tests and a script
to run them through Valgrind. Valgrind helps us find memory leaks or unsafe uses of memory,
so if Valgrind reports an error, that's something we definitely need to fix.
