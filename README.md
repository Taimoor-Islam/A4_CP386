# A4_CP386
This repository contains the code for Assignment 4 in CP386 for the Spring 2022 term

## Features
This code simulates the creation of a memory space of size MAX (defined by command line inputs).<br>
Using the RQ command, it is possible to allocate memory to simulated processes.

> RQ P0 10000 F

The command above would request the creation of a process called P0 with a size of 10000 bytes, using the First Fit allocation algorithm.<br>
The other available allocation algorithms are Best Fit (used by indicating a B instead of an F), and Worst Fit (use W instead).

It is also possible to release these processes using the RL command.

> RL P0

This command would release the memory for P0 (if it exists). <br>

The Status command can be used to see the state of the memory space, with all processes and holes accounted for.

> Status

The C command can be used to compact the processes in the simulated memory space. This will push all the processes to the lower memory locations and create one large hole in the higher memory.

> C

Finally, the Exit command can be used to exit the program.

> Exit

## License
There is no license on this code. If you want to use it, you are free to do so.

## About the developer
I am a 3rd year student in the Computer Science and Psychology program. I am taking this course during the spring term because I want to try and stay on track of my courses while I do COOP. I completed the assignments for this course on my own.
