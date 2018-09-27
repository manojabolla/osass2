#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
void signalHandler(int);

void signalHandler(int SIGVAL) {
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	if(SIGVAL == SIGINT) {
		fprintf(stderr, "%sCTRL- Signal interrupt is initiated.\n");
	}

	if(SIGVAL == SIGALRM) {
		fprintf(stderr, "%sMaster time out. Terminating remaining processes.\n");
	}
	kill(-getpgrp(), SIGQUIT);/*kills all the children of process*/
	exit(1);
	
}

int main (int argc, char *argv[]) {
	pid_t childpid = 0;
	int i,j;
	if (argc <1){ /* check for valid number of command-line arguments */
		fprintf(stderr, "Usage: %s processes\n", argv[0]);
		return 1;
	}	
	int c,n,s,h;
	signal(SIGALRM,signalHandler);	
	alarm(2);
	
	while ((c = getopt (argc, argv, "hn:s:")) != -1)
    	switch (c)
      	{
		 case 'h':
       			fprintf(stderr, "Usage: -h[HELP] -n [procescount] -s [childexists]\n", argv[0]);
        		return 1;
	
      		case 'n':
        		n = atoi(optarg);
			if(n > 20){
				n=20;	
				fprintf(stderr,"Only max of 20 child are allowed\n");
			}	
        		break;
      		case 's':
        		s = atoi(optarg);
        		break;
      		default:
      		fprintf(stderr, "Usage:\n", argv[0]);
      	}	
	
	// handle SIGNALS callback attached
		
 	signal(SIGINT, signalHandler);

  	int segment_id;
  	int segment_size;
	int buf[1];
 
  	/* Allocate a shared memory segment.  */
  	key_t key = ftok(".", 'x');
  	segment_id = shmget (key, sizeof(buf[1]), 0777| IPC_CREAT);
  	/* Attach the shared memory segment.  */
  	//printf("Shared memory segment ID is %d\n", segment_id);
  	int *shared_mem =  shmat (segment_id, 0, 0);
	shared_mem[0] =0 ;
        shared_mem[1] = 0;
        //fprintf(stderr, "Shared Memory copied \n"); 
  	char arg1[50];
  	char arg2[50];
  
 	snprintf(arg1,50,"%d",n);
  	snprintf(arg2,50,"%d",segment_id);
	int k;
	for(k = 0; k < s ; k++)
	{
		fprintf(stderr, "child Executed %d \n", k);	
  		if (fork() == 0) 
   		{
			execlp("./worker","./worker",arg1,arg2,(char *)NULL);// If we get here, exec failed
			fprintf(stderr,"%s failed to exec worker!\n",argv[0]);		
			exit(-1); 
		}	
	}
	int childrenSpawned = k;
	while(1){
		wait(NULL);
		if(childrenSpawned == n)
		break;
		else
		{
		//sleep(1);   Used for Timeout testing 
	 		if (fork() == 0)
        		{
				fprintf(stderr, "Child Executed %d \n", childrenSpawned);
               	 		execlp("./worker","./worker",arg1,arg2,(char *)NULL);// If we get here, exec failed
                		fprintf(stderr,"%s failed to exec worker!\n",argv[0]);
                		exit(-1);
       		 	}
		childrenSpawned++;
		}
	}

	while(wait(NULL)>0)
	{
	};

	fprintf(stderr,"Master : Milliseconds = %d, Seconds = %d \n",shared_mem[0],shared_mem[1]);
	/* Detach the shared memory segment.  */
  	shmdt ((void *) shared_mem);
  	shmctl (segment_id, IPC_RMID, NULL);
	
	return 0;
}

