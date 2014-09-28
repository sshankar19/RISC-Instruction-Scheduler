#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <unistd.h>
#include <getopt.h>
#include "schedutils.h"



//find lowest cycle count, edit the spot in the LL and then make/add to new list
void sort(Instruction head){
	Instruction traverser = head;
	Instruction saver = head;
	int i = 0;
	int lowest;
	int numInstr = head->numInstructions;
	while(i < numInstr){
		lowest = 999999999;
		saver = NULL;
		traverser = head;
		while(traverser != NULL){
			if(traverser->cycle < lowest && traverser->found != 1){
				lowest = traverser->cycle;
				saver = traverser;
			}
			traverser = traverser->next;
		}
		if(saver != NULL){
			printNode(saver);
			saver->found = 1;
		}
		i++;
	}
}

		

void helperLL(Instruction head, int extra, int isAnti){
	if(head != NULL){
		// Check successors to make sure this isn't an anti dependency, before adding latency
		int index;
		int priority;

		// For each dependence in temp track it back
		if(isAnti == 0 ){
			priority = head->latency+extra;
			if(head->priority < priority){
				head->priority = priority;
				head->delay = head->latency;
			}
			for(index = 1; index < 5; index++){
				//go through all possible dependencies
				if(head->depends[index] != NULL){
					helperLL(head->depends[index]->restricts, head->priority, head->depends[index]->isAnti);
				}
			}
		}
		else{ //is anti but not mem.
			priority = 1+extra;
			if(head->priority < priority){
				head->priority = priority;
				head->delay = 1;
			}

			for(index = 1; index < 5; index++){
				//go through all possible dependencies
				if(head->depends[index] != NULL){
					helperLL(head->depends[index]->restricts, head->priority, head->depends[index]->isAnti);
				}
			}
		}
	}
}

void longlatency(Instruction head){
	Instruction temp = head;
	//printAllDeps(head);
	while(temp != NULL){
		if(temp->priority==0){
			helperLL(temp, 0, 0);
		}
		temp = temp->prev;
	}
}

void highlatency(Instruction head){
	Instruction temp = head;
	while(temp!=NULL){
		temp->priority = temp->latency;
		temp = temp->prev;
	}
}

void choice(Instruction head){
	Instruction temp = head;
	int index = 0;
	while(temp!=NULL){
		index = 0;
		while(index < head->numInstructions && temp->successors[index] != NULL){
			index++;
		}
		temp->priority = temp->latency*index*index; //Factoring both latency and number of successors, with the latter weighted more. This way I wanted to try and improve on highest latency by adding a bonus to those nodes that had more successors
		temp = temp->prev;
	}
}

int notreadyMemCheck(Instruction node){
	int i = 0;
	if(node->ismem != 0)
		return 0;
	else{
		for(i = 0; i < node->numInstructions; i++){
			if(node->memoryDepends[i] != NULL && node->memoryDepends[i]->isDone != 1 && node->memoryDepends[i]->isActive == 0){
				//puts("stuck here");
				return 1;
			}
		}
		return 0;
	}
}	


void schedule(Instruction head){
	int cycle = 1;
	Instruction temp = head;
	int numInstr = temp->numInstructions;
	int activeSize = 0;	

	Queue ready = createQueue();
	Queue q = createQueue();
	Instruction active[numInstr];
	int flag = 0;	
	
	//adding leaves to Ready Queue
	while(temp != NULL){
		if(temp->numdepends == 0){
			ready.push(&ready, temp);
			temp->isReady = 1;
			temp->isLeaves = 1;
		}
		temp = temp->next;
	}
	//Initializing indices of active to NULL
	int index = 0;
	while(index < numInstr){
		active[index] = NULL;
		index++;
	}
	index = 0;

	//Meat of the program
	Instruction readyInstr = NULL;
	while(ready.size+activeSize > 0){
		readyInstr = NULL;
		if(ready.size > 0){
			flag = 1;
			while(ready.peek(&ready)!=NULL && ready.peek(&ready)->isLeaves ==1){
				//puts("Here");
				Instruction nd1 = ready.peek(&ready);
				if(notreadyMemCheck(nd1)){ // Check if it needs to make sure for a memory dependency that all are either done or active
					flag = 0;
					q.push(&q, ready.pop(&ready));
				}
				else{ // We reached one where it was good to go!
						//printNode(nd1);
					flag = 1;
					break;
				}
			}
			if(flag == 1){
				readyInstr = ready.pop(&ready);
				readyInstr->cycle = cycle;
				//printNode(readyInstr);
				index = 0;
				while(active[index] != NULL && index < numInstr){
					index++;
				}
				active[index] = readyInstr;
				readyInstr->isActive = 1;
				activeSize++;

				while(q.peek(&q)!= NULL){
					ready.push(&ready, q.pop(&q));
				}
			}
		}
		cycle++;
		//Removing from active and adding new to Ready
		index =0;
		while(activeSize > 0 && index < numInstr){
			if(active[index] != NULL){
				Instruction node = active[index];
				//printNode(node);
				if(node->cycle+node->latency <= cycle){
					//remove from active and add to ready
					addReady(&ready, active, node, 0);
					//Add any successors whoa re done
					activeSize--;
					active[index] = NULL;
					node->isActive = 0;
				}
				else if(node->cycle+1 <= cycle && node->hasAnti == 1){ //isAnti
					//add anti stuff but no others
					addReady(&ready, active, node, 1); 
				}
			}
			index++;
		}
	}
}
void addReady(Queue* ready, Instruction active[], Instruction node, int index){
	int sindex = 0;
	int numInstr = node->numInstructions;
	Instruction temp = NULL;
	if(index == 0){ //removal as standard
		node->isDone = 1;
		while(sindex < numInstr){
			temp = node->successors[sindex];

			if(temp != NULL && readyCheck(temp)){
				node->successors[sindex] = NULL;
				ready->push(ready, temp);
				temp->isReady = 1;
			}
			sindex++;
		}
	}
	else if(index == 1){ //is an anti dependency
		while(sindex < numInstr){
			temp = node->successors[sindex];
			if(temp != NULL && temp->depends[3]->restricts == node && temp->depends[3]->isAnti == 1){
				temp->depends[3]->depAntiDone = 1;
				if(readyCheck(temp)){ //checking for anti for r3
					ready->push(ready, temp);
					temp->isReady = 1;
					node->successors[sindex]= NULL;
				}
			}
			else if(temp != NULL && temp->depends[4]->restricts == node && temp->depends[4]->isAnti == 1){
				temp->depends[4]->depAntiDone = 1;
				if(readyCheck(temp)){ //checking for anti for r4
					ready->push(ready, temp);
					temp->isReady = 1;
					node->successors[sindex]= NULL;
				}
			}
			sindex++;
		}
	}
}

int readyCheck(Instruction node){
	if(node->isReady ==1)
		return 0;
	else{
		if(notreadyMemCheck(node))
			return 0;
		if(node->depends[1]->restricts != NULL && node->depends[1]->restricts->isDone == 0)
			return 0;
		if(node->depends[2]->restricts != NULL && node->depends[2]->restricts->isDone ==0)
			return 0;
		if(node->depends[3]->restricts != NULL && node->depends[3]->restricts->isDone ==0 && node->depends[3]->depAntiDone == 0)
			return 0;
		if(node->depends[4]->restricts != NULL && node->depends[4]->restricts->isDone ==0 && node->depends[4]->depAntiDone == 0)
			return 0;
		return 1;
	}
}

int main(int argc, char** argv){
	Instruction head= NULL;
	int aflag = 0;
	int bflag = 0;
	int cflag = 0;
	int c;
	
	// Get the option;
	while((c = getopt(argc, argv, "abc")) != -1)
		switch(c)
		{
			case 'a':
				aflag = 1;
				break;
			case 'b':
				bflag = 1;
				break;
			case 'c':
				cflag = 1;
				break;
			case '?':
				printf("Unknown option entered.");
				return 1;
			default:
				printf("No valid option entered.");
				return 1;
		}
	// Take the stuff from stdin and make internal representation.

	Instruction first= NULL;
	head = NULL;
	create(&head);
	while(head->next!= NULL){
		head = head->next;
	}
	first = head;
	while(first->prev != NULL){
		first = first->prev;
	}

	buildDeps(head);
	// Now that we have the options, select the right one
	if(aflag){
		longlatency(head);
		//printAllDeps(head);
	}
	else if(bflag){
		highlatency(head);
	}
	else if(cflag){
		//choice(head);
	}
	else{
		puts("Something went wrong with the options, Returning 1");
		return 1;
	}

	schedule(first);
	sort(first);
	
	destroyRep(first);
	//free(head);
	return 0;
}


