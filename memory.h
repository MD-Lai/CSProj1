
/**
 * Comp30023 Computer Systems
 * Semester 1 2017
 * 
 * Marvin Der Hann Lai 754672
 * 
 * Submission for Project 1 - Memory Management and Scheduling
*/
#include "list.h"

/* Representation of a process */
typedef struct process_t {
	int timeCreated;
	int pid;
	int memSize;
	int jobTime;

	int diskTime;
	int memTime;
	int quantum;
} Process;

/* segment of memory that holds a process. */
typedef struct segment_t{
	int start;			/* adress of start of memory block */
	int size;			/* size of memory block */
	Process* process;	/* process stored in memory block */
} Segment;

void swapSchedule(List* d, List* m, List* r,int q, int t, List (*f)(List m, Process* p));

/* gives list node of first fitting memory segment */
List first(List mem, Process* process);

/* gives list node of best fitting memory segment */
List best(List mem, Process* process);

/* gives list node of worst fitting memory segment */
List worst(List mem, Process* process);

/* loads processes from given file */
List loadProcesses(char* file, int q);

/* initiates a completely empty main */
List initiateMain(int size);

/* inserts process to tail of round robin queue (RRQ) or disk */
void insertDRRQ(List* l, Process* p);

/* Insert a process into a given location of main */
/* Inserts it in list node specified by memory */
void insertMain(List memory, Process* newProcess);

/* Removes a process with given pid from memory and returns it */
/* can choose to ignore it if so desired */
Process* remProcessM(List memory, int pid);

/* Removes a process with given pid from disk or RRQ and returns it */
Process* remProcessDR(List* dr, int pid);

/* checks if a segment in memory is empty */
int segmentEmpty(Segment* memBlock);

/* returns the process that's been on disk for the longest */
Process* getLongestDisk(List d);

/* returns the process that's been in memory longest */
Process* getLongestMem(List m);

/* checks if memory is empty */
int memoryEmpty(List m);

/* checks if insufficient memory space for a given process size */
int insufficientMem(List m, int s);

/* increments disk time of given list */
void addDT(List l);

/* increments memory time of given memory list */
void addMT(List l);

/* runs first process in round robin queue */
void runProcess(List r, int* e1, int* e2, int* e3);

/* prints required output lines */
void printStatus(List mem, Process* p, int t);
