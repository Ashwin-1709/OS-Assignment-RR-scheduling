## Uniprocessor Scheduler Simulation

The assignment aims to simulate a Round Robin Scheduling Algorithm with various quanta values and study the effect of varying numbers of threads on multiple workloads and the impact of quanta values on waiting time, turnaround time, and time wasted due to context switches.

### Working : 

The scheduler process S is executed first; it spawns and execs two children, P1 and P2. P1 spawns a certain number of threads to read matrices from two input files and uses IPC mechanisms such as shared memory to send those matrices to P2. P2 spawns threads for matrix multiplication computation and writes the result into an output file. S uses signals to simulate a uniprocessor scheduler. That is, it suspends P1 and lets P2 execute, and vice versa. </br> </br>
![image](https://user-images.githubusercontent.com/98446038/209423843-48cfde48-88ae-417b-9852-558a1e6eda71.png)
</br>
</br>
#### How to run : 
1. Grant executable permission to the runner bash script using ```chmod +x runner.sh```
2. Run the bash script using with the appropriate arguments ```./runner.sh N M K matrix1.txt matrix2.txt output.txt``` where N x M is the size of first matrix and M x K is the size of second matrix.
