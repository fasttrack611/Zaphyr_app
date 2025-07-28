This example creates two threads, each with its own thread-local variable. 
The threads modify and print their own copies of the variable independently.


Each thread will print its own tls_counter value independently:

Main thread started
Thread A: tls_counter = 1
Thread B: tls_counter = 1
Thread A: tls_counter = 2
Thread B: tls_counter = 2
...


This confirms that each thread has its own isolated copy of tls_counter.