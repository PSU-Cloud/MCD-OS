# MCD-OS
MCD-OS (MemCacheD with Object Sharing) is an in-memory key-value store with object sharing accountability across proxies.

Memcached is a high performance multithreaded event-based key/value cache
store intended to be used in a distributed system.
See: https://memcached.org/about
A fun story explaining usage: https://memcached.org/tutorial


## Dependencies
* libevent, https://www.monkey.org/~provos/libevent/ (libevent-dev)
* libseccomp, (optional, experimental, linux) - enables process restrictions for
  better security. Tested only on x86_64 architectures.




## Installation and Instructions
1. To install MCD-OS, follow the instructions provided memcached's public repository
```
https://github.com/memcached/memcached.git
```
2. optional: set the number of clients by modifying the following line in the memcached.h file (re-compile the MCD-OS when changing the value)
```
#define NUM_WORKERS 9
```

3. optional: set the allocations per client in thread.c:memcached_thread_init()

# TODOs
* Implement S-LRU


## Refrences

* https://www.memcached.org
* Technical Report in the repository (main2-TR.pdf)
