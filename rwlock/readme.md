original found in 15-445 2017 project code base.

when a writer has entered waiting,
no further read lock is granted until there's no writer waiting.

the version with read upper limit is from rwmutex.h.
I write rwlock as a simpler version.