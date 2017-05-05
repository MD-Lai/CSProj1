/**
 * Comp30023 Computer Systems
 * Semester 1 2017
 * 
 * Marvin Der Hann Lai 754672
 * 
 * Submission for Project 1 - Memory Management and Scheduling
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* printer included memory.h */
/* printer is mostly for debug crap */
/* once done debugging replace it with memory.h */
#include "memory.h" 

/* Extern declarations: */

extern  int     optind;
extern  char    *optarg;



/* PLACEHOLDER */
int main(int argc, char** argv){

	char input;

	/* input variables */
	char* file = "\0";
	List (*algo)(List m, Process* p);
	int memSize = 500;
	int q = 25;

	/* algorithm to use */

	int t = 0;
	int arrived = 0;
	int e1, e2, e3;

	/* Acts as "inbound" where processes are stored before their load time */
	List inbound;
	/* Acts as "disk" where processes are stored when not in main memory */
	List disk;
	/* Main memory */
	List memory;
	/* Round Robin Queue */
	List rrq;

	if(argc <= 1){
		fprintf(stderr, "No inputs given\n");
		fprintf(stderr, "Usage: "\
			"-f file location\n"\
			"       -a algorithm to use (\"first\", \"best\", or \"last\")\n"\
			"       -m memory size\n"\
			"       -m quantum length\n");

		exit(1);
	}

	while ((input = getopt(argc, argv, "f:a:m:q:")) != EOF){
		switch(input){
			case 'f':
				file = optarg;
				break;

			case 'a':
				if(strcmp(optarg, "first") == 0){
					algo = first;
				}
				else if(strcmp(optarg, "best") == 0){
					algo = best;
				}
				else if(strcmp(optarg, "worst") == 0){
					algo = worst;
				}
				else{
					fprintf(stderr, "Unknown algorithm: %s\n", optarg);
					exit(1);
				}
				break;

			case 'm':
				memSize = atoi(optarg);
				break;

			case 'q':
				q = atoi(optarg);
				break;

			default:
				break;
		}

	}

	/* initiates required data structures */
	/* everything except memory is a list of processes */
	inbound = loadProcesses(file, q);
	disk = NULL;
	rrq = NULL;
	
	/* memort is a list of segments, holds information about a "chunk" of memory */
	memory = initiateMain(memSize);

	/* continues while there are items inbound, on disk, or in the round robin queue */
	while((inbound || disk || rrq)){
		
		/* loads all processes that are created at time t */
		while(inbound && ((Process*)inbound->data)->timeCreated == t){
			insertDRRQ(&disk, remProcessDR(&inbound, ((Process*)inbound->data)->pid));
			arrived = 1;
		}

		e1 = arrived && memoryEmpty(memory);	  /* memory empty and new process arrived */
		e2 = rrq && ((Process*)rrq->data)->quantum == 0; /* quantum expired */
		e3 = rrq && ((Process*)rrq->data)->jobTime == 0; /* process finished */

		if(e1 || e2 || e3){
			swapSchedule(&disk, &memory, &rrq, q, t, algo);
		}
		
		addMT(memory);
		addDT(disk);
		runProcess(rrq, &e1, &e2, &e3);

		t += (inbound || disk || rrq); 
	}
	printf("time %d, simulation finished.\n", t);

	return 0;

}




