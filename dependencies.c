#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "schedutils.h"


// NEED TO CHECK IF ANTIS ARE SAME AS TRUE, so if R1 dep instr  or R2 = O1 dep instr or O2 dep instr Can't find anti dependencies for this yet

void addIn(Instruction temp, Instruction head, int index){

	//Checking for memory dependancies

	
	//Checking for repetitive adding of dependancies

	if(index >= 2) //if 2nd or 3rd or 4th is dependant on memory instruction and it's the same, but case 1 takes care of this
		if((head->depends[1]->restricts != NULL && head->depends[1]->restricts == temp)){
			head->depends[index]->restricts = NULL;
			return;
		}
	if(index >= 3) //if 3rd or 4th register is dependant on instruction but it's the same as instruction of 2nd reg.
		if((head->depends[2]->restricts!= NULL && head->depends[2]->restricts==temp)){
			head->depends[index]->restricts = NULL;
			return;
		}
	if(index > 3)
		if((head->depends[3]->restricts!= NULL && head->depends[3]->restricts==temp)){
			head->depends[index]->restricts = NULL;
			return;
		}
	
	//Setting the dependencies up
	head->depends[index]->restricts = temp;
	if(index > 2){
		head->depends[index]->isAnti = 1;
		temp->hasAnti = 1;
	}
	else
		head->depends[index]->isAnti = 0;

	int i = 0;
	while(temp->successors[i] != NULL && i < temp->numInstructions){
		i++;
	}
	if(temp->numInstructions > i)
		temp->successors[i] = head;
	head->numdepends++;
}


void deps(Instruction head){
	Instruction temp = head;

	// FOR TRUE DEPENDENCIES RAW (FIRST REGISTER)
	if(head->firstReg != NULL && head->firstReg->isRegister == 0){ //It's a register and we have to find true dependencies for this one.
		while(temp->prev != NULL){ //If RAR, then don't add in, break;
			temp = temp->prev;
			if(temp->outputReg1 != NULL && temp->outputReg1->isRegister == 0 && temp->outputReg1->value == head->firstReg->value){
				addIn(temp, head, 1);
				break;
			}
			else if(temp->outputReg2 != NULL && temp->outputReg2->isRegister == 0 && temp->outputReg2->value == head->firstReg->value){
				addIn(temp, head, 1);
				break;
			}
		}
	}
	temp = head;

	// RAW (SECOND REGISTER)
	if(head->secondReg != NULL && head->secondReg->isRegister == 0){ // Second reg and we have to find true dependencies for these ones.
		while(temp->prev != NULL){
			temp = temp->prev;
			if(temp->outputReg1 != NULL && temp->outputReg1->isRegister == 0 && temp->outputReg1->value == head->secondReg->value){
				addIn(temp, head, 2);
				break;
			}	
			else if(temp->outputReg2 != NULL && temp->outputReg2->isRegister == 0 && temp->outputReg2->value == head->secondReg->value){
				addIn(temp, head, 2);
				break;
			}
		}
	}
	temp=head;

	// FOR ANTI DEPENDENCIES WAR (FIRST OUTPUT REGISTER)
	if(head->outputReg1 != NULL && head->outputReg1->isRegister == 0){ //It's a register and we have to find true dependencies for this one.
		while(temp->prev != NULL){
			temp = temp->prev;
			if(temp->firstReg != NULL && temp->firstReg->isRegister == 0 && temp->firstReg->value == head->outputReg1->value){
				addIn(temp, head, 3);
				break;
			}
			else if(temp->secondReg != NULL && temp->secondReg->isRegister == 0 && temp->secondReg->value == head->outputReg1->value){
				addIn(temp, head, 3);
				break;
			}
		}
	}
	temp = head;

	// WAR (SECOND OUTPUT REGISTER)
	if(head->outputReg2 != NULL && head->outputReg2->isRegister == 0){ // Second reg and we have to find true dependencies for these ones.
		while(temp->prev != NULL){
			temp = temp->prev;
			if(temp->firstReg != NULL && temp->firstReg->isRegister == 0 && temp->firstReg->value == head->outputReg2->value){
				addIn(temp, head, 4);
				break;
			}
			else if(temp->secondReg != NULL && temp->secondReg->isRegister == 0 && temp->secondReg->value == head->outputReg2->value){
				addIn(temp, head, 4);
				break;
			}
		}

	}
	
	
}


void memory_deps(Instruction head, int index){
	Instruction temp = head;
	//Malloc for all the other memory dependences
	int i = 0; int j = 0;
	i = 0; j = 0;
	while(temp->prev != NULL){
		temp = temp->prev;
		//printNode(temp);
		if(index == 0){ // Is OUtput
			if(temp->ismemwrite == 1 && temp->ismemread != 1){
				while(i < temp->numInstructions && temp->successors[i] != NULL){
					i++;
				}
				temp->successors[i] = head;
				while(j< temp->numInstructions && head->memoryDepends[j] != NULL){
					j++;
				}
				head->memoryDepends[j] = temp;
			}
		}
		else if(index == 1){ //is write
			if(temp->ismemread == 1){
				while(i < temp->numInstructions && temp->successors[i] != NULL){
					i++;
				}
				temp->successors[i] = head;
				while(j< temp->numInstructions && head->memoryDepends[j] != NULL){
					j++;
				}
				head->memoryDepends[j] = temp;
			}
		}
		else{ //is a read
			if(temp->ismemwrite == 1&& temp->ismemread != 1){
				while(i < temp->numInstructions && temp->successors[i] != NULL){
					i++;
				}
				temp->successors[i] = head;
				while(j< temp->numInstructions && head->memoryDepends[j] != NULL){
					j++;
				}
				head->memoryDepends[j] = temp;
			}
		}
	}
	//puts(" ");
}

void buildDeps(Instruction head){
	// If operation is nop, then no dependencies
	while(head != NULL){
		if(head->opcode != NOP) // If an operation other than 'no op'
		{
			if(head->ismemread ==1 && head->ismemwrite !=1){ //Is a read (LOAD)
				memory_deps(head, 2);
			}
			else if(head->ismemwrite==1 && head->ismemread != 1){ //Is a write (STORE)
				memory_deps(head, 1);
			}
			else if(head->ismem ==0){ //is I/O
				memory_deps(head, 0);
			}
			deps(head);
		}
		head = head->prev;
	}
}
