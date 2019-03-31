threads
=======

Learning about multithreading in C++11. I made a ReadWriteLock class that allows unlimited number of simultaneous reads and a single write at a time. Implemented in two different ways (with 2 mutexes preferring reads, and again with 1 mutex and a condition variable preferring writes).
