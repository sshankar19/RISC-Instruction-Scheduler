Readme
Shashank Shankar
2/25/2014

Local Instruction Scheduling - Code

Project Description:

I wrote three local instruction schedulers (three different heuristics) based on forward list scheduling. My scheduler was able to accept the following ILOC instructions (with their latencies next to them in parentheses):

no operation: nop (1)
arithmetic: addI (1), add (1), subI (1), sub (1), mult (3), div (3)
memory: load (5), loadI (1), loadAO (5), loadAI (5), store (5), storeAO (5), storeAI (5)
I/O: output (1)

The code itself works with anti dependencies and true dependencies as part of it's implementation.

Implementation:

This program was written in C, and the memory management and data structures were personally done. The instructions were stored as a doubly linked list with pointers to the next and previous instruction parsed. They also had pointers to an array of successor Instructions, and an array of Instructions that were memory dependent, if the Instruction was involved in memory or I/O. These Instructions also had pointers to Dependancy pointers which pointed to true or anti dependencies of the Instruction. I also implemented a Priority Queue as the Ready Set for the scheduler.

Heuristics:

I used three different heuristics:

1) longest latency-weighted path to root - Using the critical path to find the best nodes to add to the ready set first and process

2) highest latency instructions - The instructions with the heaviest latency would be processed first

3) A combination of highest latency and most number of successors - The priority is calculated as latency * (number of successors)^2. This was to see if I could improve on the heaviest latency algorithm by picking the Instructions with the most amount of successor nodes

Usage:

This program takes in an input file to scan via stdin and prints to stdout. You can compile this program using the makefile included ('make').
However, you also need to specify an option for the heuristic:
-a - Longest-latency
-b - Highest-Latency
-c - My choice heuristic based on highest latency and most number of successors. It's nickname is Fat Father of Many Heuristic

So an example input to the program may be:

./scheduler -a < bench01.iloc > schedule.out

