#include<stdio.h>
#include <sys/shm.h>


void main(int argc, char *argv[])
{
	int i;
	int n = atoi(argv[1]);
	int id = atoi(argv[2]);
	int *shared_mem = shmat(id,0,0);
     	for(i = 0; i < n * 1000000; i++)
        {
		shared_mem[0] += 1;
		if(shared_mem[0] >= 1000)
		{
			shared_mem[0] = 0;
  			shared_mem[1] += 1; 
		}
        }
	printf("Miliseconds : %d, Seconds : %d\n",shared_mem[0],shared_mem[1]);

}
