# Operating Systems
Course Assignments for CS 3600  
Summer 2017  
Dr. Steven Beaty  
Metropolitan State University of Denver

## Assignments
### 01 processes
The first assignment consists of two programs: the first program takes an integer *n* as a parameter and loops that many times, displaying "Process: [process id] [iteration]" for each loop. It also returns *n* as its exit status; the second program creates a child process which executes the first program passing an argument of 5. It then waits for the child to exit and reports its status.
### 02 signals
The second assignment is a single program which handles the sending and receiving of signals. It forks itself and then has the child send signals to the parent with support for three different types of signals.
### 03 scheduling
The third assignment requires the modification of existing code to add support for interrupts and context switches between different processes. More information can be found in: `/03 scheduling/instructions.cc`.
### 04 mutex
The fourth assignment provides code with an unresolved race condition present which requires the introduction of a mutex lock around the critical section causing the race condition. More information can be found in: `/04 mutex/race.cc`.
