Songbird
========

Songbird is a set of stand alone libraries for C. The only requirements for
most of the headers is some kind of dynamic memory allocation (customizable).
These headers are compatible with C89.

Simple Libraries
 * array.h - A non-expanding array container.
 * buffer.h - A byte buffer and reader. Used to collect and dispatch bytes.
 * deque.h - A double ended array backed queue. Much faster then a linked or double linked list for the purpose.
 * vector.h - An automatically expanding array container.

Advanced Libraries
 * files.h - A simple file interaction library.
 * sockets.h - A simple socket lbirary

None of the header files rely on any of the other header files.
