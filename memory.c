/**
 * Comp30023 Computer Systems
 * Semester 1 2017
 * 
 * Marvin Der Hann Lai 754672
 * 
 * Submission for Project 1 - Memory Management and Scheduling
*/

#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

/* where the "magic" happens, manages swapping processes from disk to memory
   and scheduling processes in round robin queue */
void swapSchedule(List* d, List* m, List* r,int q, int t, List (*f)(List m, Process* p)){
	
	Process* p = NULL;
	Process* pr = NULL;
	Process* removed = NULL;

	/* f is one of first/best/worst algorithms */
	/** swap */
	/* get a process from disk */
	/* load into memory according to algorithm */
		/* if insufficient space swap processes to disk until there's a space */
			/* also remove it from rrq if swapped to disk*/
			/* remember to reset diskTime, quantum, and memTime when removing */

		/* if memory empty, put process into memory and head of rrq */
		/* if memory not empty, place according to algorithm, and end of rrq */
	/* if there is something in round robin queue*/
	if(*r){
		/* if it's complete, delete it, otherwise pull it out */
		pr = list_pop(r);
		if(pr->jobTime){
			/*
			printf("q: moving ");
			printProcess(pr);
			*/
		}
		pr->quantum = q;

		/* remove it if it's finished */
		if(!pr->jobTime){
			remProcessM(*m, pr->pid);
			free(pr);
			pr = NULL;
		}
	}

	/* if disk is not empty */
	if(*d){
		/* load a process into memory */

		/* picks the process on disk for the longest */
		p = getLongestDisk(*d);
		p->diskTime = 0;
		/* remove it from disk for now */
		remProcessDR(d, p->pid);

		/* while memory is full, remove and put back to disk, resetting mem time and quantum*/
		/* aka while the algorithm can't find a space to fit selected process */
		/* if it exists on memory it'll exist on rrq */
		/* issue is unloading entirety of mem*/
		
		while(insufficientMem(*m, p->memSize)){
			
			removed = getLongestMem(*m);
			removed->diskTime = 0;
			removed->memTime = 0;
			removed->quantum = q;


			/* put it back to disk */
			/* job should be guaranteed to be unfinished */
			remProcessM(*m, removed->pid);
			remProcessDR(r, removed->pid);
			insertDRRQ(d, removed);
			if(pr && removed->pid == pr->pid){
				pr = NULL;
			}
		}
	
		/** schedule */
			/* if a process's jobTime finishes, delete it */
			/* if a process's quantum expired remove from rrq with list_pop, keep for now */
			/* place process obtained from swap if one exists, put into end of rrq */
			/* put process with expired quantum at end of rrq, reset quantum */
		/* p is latest process from disk */
		/* place p in rrq first then pr */
		insertDRRQ(r, p);

		/* if there is a process to stick at end of rrq */
		if(pr){
			insertDRRQ(r, pr);
		}

		insertMain(f(*m, p), p);
	}

	/* if disk is finished but still contains a process to run */
	/*
	*/
	else if(!*d && pr){
		insertDRRQ(r, pr);
	}
	printStatus(*m, p, t);
}

/* first fit */
/* returns first empty space that fits */
List first(List mem, Process* process){

	/* if no space available, returns null */
	do{
		if(segmentEmpty(mem->data) && ((Segment*)mem->data)->size >= process->memSize){
			return mem;
		}
		
	} while((mem = mem->next));

	return mem;
}

/* best fit */
/* returns empty space with best fit */
List best(List mem, Process* process){

	List best = NULL;

	do{
		if(((Segment*)mem->data)->size >= process->memSize && segmentEmpty(mem->data)){
			if(!best){
				best = mem;
			}
			else {
				if(((Segment*)best->data)->size > ((Segment*)mem->data)->size){
					best = mem;
				}
			}
		}
	} while((mem = mem->next));

	return best;
}

/* worst fit */
/* returns empty space with worst fit */
List worst(List mem, Process* process){
	List worst = NULL;
	do{
		if(((Segment*)mem->data)->size >= process->memSize && segmentEmpty(mem->data)){
			if(!worst){
				worst = mem;
			}
			else {
				if(((Segment*)worst->data)->size < ((Segment*)mem->data)->size){
					worst = mem;
				}
			}
		}
	} while((mem = mem->next));

	return worst;
}

/* returns a pointer to the head of the list */
List loadProcesses(char* file, int q) {

	FILE* fp = fopen(file, "r");
	if(!fp){
		fprintf(stderr, "file not found\n");
		exit(1);
	}

	List processes = NULL;
	int new = 1;
	Process tempProc;
	Process* nextProc;

	while(fscanf(fp, "%d %d %d %d\n", 
		&tempProc.timeCreated, 
		&tempProc.pid, 
		&tempProc.memSize, 
		&tempProc.jobTime) != EOF){
		
		tempProc.diskTime = 0;
		tempProc.memTime = 0;
		tempProc.quantum = q;

		if((nextProc = malloc(sizeof(Process)))){
			*nextProc = tempProc;	
		}
		else{
			fprintf(stderr,"malloc failed in loadProcesses\n");
			exit(1);
		}


		if(new){
			processes = list_push(NULL, nextProc);
			new = 0;
		}
		else{
			list_insert(&processes, nextProc);
		}

	}

	fclose(fp);

	return processes;
}

/* initiates a completely empty main */
List initiateMain(int size){
	List memory;
	Segment* mainSeg;
	if((memory = malloc(sizeof(List))) && (mainSeg = malloc(sizeof(Segment)))){

		mainSeg->start = size - 1;
		mainSeg->size = size;
		mainSeg->process = NULL;

		memory = list_push(NULL, mainSeg);
	}
	else{
		fprintf(stderr, "malloc failed in initiateMain\n");
		exit(1);
	}

	return memory;
}

/* insert to tail of disk or rrq */
void insertDRRQ(List* l, Process* p){
	list_insert(l, p);
}

/* Foundation allowing insertion of process into memory segment... 
make sure to use external func to check available space*/
/* if you fill up memory there will be a -1 chunk at the end. work around it */
/* ###memory is the pointer to a free chunk of memory### */
void insertMain(List memory, Process* newProcess){
	List newList;
	Segment* newSegment;
	Segment* freeSegment = (Segment*)memory->data;
	freeSegment->process = newProcess;

	/* makes sure to not split "0" spaces if perfectly fit*/
	if(freeSegment->size == newProcess->memSize){
		freeSegment->process = newProcess;
		return;
	}

	/*new segment is now free segment*/
	if((newList = malloc(sizeof(List)))){
		if((newSegment = malloc(sizeof(Segment)))){
			
			newList->next = memory->next;
			memory->next = newList;

			newSegment->process = NULL;
			newSegment->size = freeSegment->size - newProcess->memSize;
			newSegment->start = freeSegment->start - newProcess->memSize;
			
			freeSegment->size = newProcess->memSize;

			newList->data = newSegment;
		}
	}

	else{
		fprintf(stderr, "malloc failed in addProcess\n");
		exit(1);
	}
}


/* removes a process with the given pid from memory */
Process* remProcessM(List memory, int pid){
	/* Conditions
	3 situations, NULL count as full
	1. space in front is empty, back full
	2. space behind is empty, front full
	3. in front and behind is empty
	4. in front and behind is full
	*/
	List prev = NULL;
	Process* returnProcess = NULL;
	
	Segment* currSeg;
	Segment* nextSeg;
	Segment* prevSeg;

	while(memory){
		currSeg = ((Segment*)memory->data);

		/* case when memory is exactly full */
		if((Segment*)memory->next){
			nextSeg = (Segment*)memory->next->data;
		}
		else{
			nextSeg = NULL;
		}

		prevSeg = prev == NULL ? NULL : ((Segment*)prev->data);

		if(currSeg->process && currSeg->process->pid == pid){
			returnProcess = ((Segment*)memory->data)->process;
			currSeg->process = NULL;


			if(segmentEmpty(prevSeg) && !segmentEmpty(nextSeg)){
				/* merge prev and memory */
				prevSeg->size += currSeg->size;
				prev->next = memory->next;

			}
			else if(!segmentEmpty(prevSeg) && segmentEmpty(nextSeg)){
				/* merge memory and next */
				/* so this works fine and dandy */
				currSeg->size += nextSeg->size;
				memory->next = memory->next->next;

				

			}
			else if(segmentEmpty(prevSeg) && segmentEmpty(nextSeg)){
				/* merge prev and memory and next */
				prevSeg->size += currSeg->size + nextSeg->size;
				prev->next = memory->next->next;
				
			}
			else{
				/* nothing, just remove the process in that segment */

			}
			break;
		}
		
		prev = memory;
		memory = memory->next;
	}

	return returnProcess;
}

/* removes a process with the given pid from disk or rrq
since they're both a List of processes.
to pretend this is a queue, use remProcessDR(rrq, rrq->data->pid);*/
/* need to pass in address of dr since it can change where the head is */
Process* remProcessDR(List* dr, int pid){
	Process* p = NULL;
	List prev = NULL;
	List head = *dr;
	
	if(!*dr){
		return NULL;
	}
	if(((Process*)(*dr)->data)->pid == pid){
		/* first element matches */
		return (Process*)list_pop(dr);
	}
	prev = *dr;
	/* otherwise something else matches */
	while(*dr){
		
		if(((Process*)(*dr)->data)->pid == pid){
			p = ((Process*)(*dr)->data);
			prev->next = (*dr)->next;

		}
		prev = *dr;
		*dr = (*dr)->next;
	}
	*dr = head;
	return p;
}

/* checks if a segment of memory is empty */
int segmentEmpty(Segment* memBlock){
	
	if(memBlock == NULL){
		return 0;
	}
	if(memBlock->start < 0){
		return 0;
	}
	return memBlock->process == NULL ? 1 : 0;
}

/* returns the process that's been on disk for the longest */
Process* getLongestDisk(List d){
	Process* longest = NULL;
	Process* current = NULL;

	if(!d){
		return NULL;
	}

	longest = ((Process*)d->data);

	do{
		current = ((Process*)d->data);
		if(longest->diskTime < current->diskTime){
			longest = current;
		}
		else if(longest->diskTime == current->diskTime){
			if(longest->pid > current->pid){
				longest = current;
			}
		}

	} while((d = d->next));

	return longest;
}

/* returns the process that's been in memory longest */
Process* getLongestMem(List m){
	Process* longest = NULL;
	Process* current = NULL;
	
	do{
		current = ((Segment*)m->data)->process;
		if(current){
			if(!longest){
				longest = ((Segment*)m->data)->process;
			}
			else{
				if(longest->memTime < current->memTime){
					longest = current;
				}
			}
		}
	} while((m = m->next));

	return longest;
}

/* checks if memory is empty */
int memoryEmpty(List m){
	do{
		if(((Segment*)m->data)->process){
			return 0;
		}
	}while((m = m->next));
	return 1;
}

/* checks if insufficient memory space for a given process size */
int insufficientMem(List m, int s){
	do{
		/* return 0 if sufficient memory */
		if(((Segment*)m->data)->size >= s && segmentEmpty(((Segment*)m->data))){
			return 0;
		}
	} while((m = m->next));
	/* return 1 otherwise (insufficient space for process) */
	return 1;
}

/* "ticks" time for item in disk */
void addDT(List l){
	if(l){
		do{
			++((Process*)l->data)->diskTime;
		} while((l = l->next));
	}
}

/* "ticks" time for item in memory */
void addMT(List l){
	do{
		if(((Segment*)l->data)->process){
			((Segment*)l->data)->process->memTime++;
		}

	} while((l = l->next));
}

/* runs first process in round robin queue */
void runProcess(List r, int* e1, int* e2, int* e3){
	Process* p;
	if(!r){
		return;
	}
	p = ((Process*)r->data);
	p->jobTime--;
	p->quantum--;

	*e1 = 0;

	if(p->quantum <= 0){
		*e2 = 1;
	}
	if(p->jobTime <= 0){
		*e3 = 1;
	}
}

/* prints required output lines */
void printStatus(List mem, Process* p, int t){
	int nProcesses = 0;
	int nHoles = 0;
	int totalMem = 0;
	int totalUsed = 0;
	float usePercent = 0;

	while(mem){
		if(!segmentEmpty(((Segment*)mem->data))){
			nProcesses++;
			totalUsed += ((Segment*)mem->data)->size;
		}
		else{
			nHoles++;
		}

		totalMem += ((Segment*)mem->data)->size;

		mem = mem->next;
	}

	usePercent = (100.0f*totalUsed)/totalMem;
	usePercent = usePercent + !!(usePercent-(int)usePercent);

	if(p){
		printf("time %d, %d loaded, numprocesses=%d, numholes=%d, memusage=%d%%\n",
			t, p->pid, nProcesses, nHoles, (int)usePercent);
	}
}
