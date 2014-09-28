#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "schedutils.h"

void init(Instruction* head){
	Instruction h =  malloc(sizeof(struct Command));
	h->opcode = 0;
	h->next = NULL;
	h->prev = NULL;
	h->ismem = -1;
	h->firstReg = NULL;
	h->secondReg = NULL;
	h->outputReg1 = NULL;
	h->outputReg2 = NULL;
	h->successors = NULL;
	h->numInstructions = 0;
	h->priority = 0;
	h->numdepends = 0;
	h->cycle = 0;
	h->delay = -1;
	h->found = 0;
	h->isDone = 0;
	h->hasAnti = 0;
	h->isReady = 0;
	h->hasMemTrue = 0;
	h->isActive = 0;
	h->isLeaves = 0;
	h->ismemread = 0;
	h->ismemwrite = 0;
	h->memoryDepends = NULL;
	*head = h;
}

// Fix this up later
void destroy(Instruction node){
	if(node->firstReg != NULL)
		free(node->firstReg); // have to free the register stuff if they have stuff inside
	if(node->secondReg != NULL)			
		free(node->secondReg);
	if(node->outputReg1 != NULL)
		free(node->outputReg1);
	if(node->outputReg2 != NULL)
		free(node->outputReg2);
	free(node->successors);
	free(node->memoryDepends);
	int i = 0;
	for(i = 0; i <5; i++){
		if(node->depends[i] != NULL)
			free(node->depends[i]);
	}
	
	free(node);
}

// Fix this up later
void destroyRep(Instruction head){
	//Instruction previous = head->prev;
	Instruction temp = head;
	//while(previous != NULL){
	//	previous = previous->prev;
	//	destroy(previous->next);
	//}
	while(temp != NULL){
		Instruction next = temp->next;
		destroy(temp);
		temp = next;
		//destroy(temp->prev);
	}
}

void create(Instruction* node){
	Instruction head = NULL;
	head = malloc(sizeof(struct Command));
	head->opcode = 0;
	head->next = NULL;
	head->prev = NULL;
	head->prev = NULL;
	head->ismem = -1;
	head->firstReg = NULL;
	head->secondReg = NULL;
	head->outputReg1 = NULL;
	head->outputReg2 = NULL;
	head->successors = NULL;
	head->numInstructions = 0;
	head->priority = 0;
	head->numdepends = 0;
	head->cycle = 0;
	head->delay = -1;
	head->found = 0;
	head->isDone = 0;
	head->hasAnti = 0;
	head->isReady = 0;
	head->hasMemTrue = 0;
	head->isActive = 0;
	head->isLeaves = 0;
	head->ismemread = 0;
	head->ismemwrite  = 0;
	head->memoryDepends = NULL;
	char line [200];
	int numlines = 0;
	int index = 0; 
	int rc = 0;
	char instr[20], r1[30], r2[30], r3[30], r4[30];
	instr[0] = '\0';
	r1[0] = '\0';
	r2[0] = '\0';
	r3[0] = '\0';
	r4[0] = '\0';
	while( fgets ( line, sizeof line, stdin) != NULL){
		//puts(line);
		rc = sscanf(line, "%s %s %s %s %s", instr,r1,r2,r3,r4);
		chooseOp(head, instr);
		createReg(head, r1, 0);
		
		if(rc == 4){
			createReg(head, r3, 2);
		}
		else{
		     if(r2[0] != '=' && r2[0] != '\0'){
			createReg(head, r2, 1);
			createReg(head, r4, 3);
		     }
		     else if(r3[0] != '=' && r3[0] != '\0'){
			createReg(head, r3, 2);
			createReg(head, r4, 3);
			}
		}
		Instruction next = NULL;
		init(&next);
		numlines++;
		head->next = next;
		next->prev = head;
		head = next;
		//Point prevs's next to new head, and that heads prev to the previous head.		
		rc = 0;
		r1[0] = '\0';
		r2[0] = '\0';
		r3[0] = '\0';
		r4[0] = '\0';
	}

	head=head->prev; // Head now points to last instruction
	free(head->next);
	head->next = NULL;
	//at the end, head should point to the prev of last instruction.
	//In which case it is the 'null' instr, move back one pointer and free this one.

	// Initializing the dependancy arrays for each instruction
	Instruction temp = head;
	while(temp != NULL){
		temp->numInstructions = numlines;
		while(index < 5){
			temp->depends[index] = malloc(sizeof(struct Restriction));
			temp->depends[index]->restricts = NULL;
			temp->depends[index]->isAnti = 0;
			temp->depends[index]->bothTrueMem = 0;
			temp->depends[index]->depMemDone = 0;
			temp->depends[index]->depAntiDone = 0;
			//temp->depends[index]->restricts = malloc(sizeof(struct Command));
			//temp->depends[index]->restricts->isDone= 0;
			index++;
		}
		temp->successors = (Instruction *) malloc(sizeof(Instruction)*numlines);
		index = 0;
		while(index < numlines){
			temp->successors[index] = NULL;
			//temp->successors[index] = malloc(sizeof(struct Command));
			//temp->successors[index]->isDone = 0;
			index++;
		}
		index = 0;
		temp->memoryDepends = malloc(sizeof(Instruction)*numlines);
		for(index = 0; index < numlines; index++)
			temp->memoryDepends[index] = NULL;
		index = 0;
		temp = temp->prev;
	}
	//puts("here");
	*node = head;
}


void chooseOp(Instruction head, char* token){
//Simple function to find the opcode responsible for this instruction
	//printf("%s",token);
	if(strcasecmp("nop", token) == 0){
		head->opcode= NOP;
		head->ismem=1;
		head->latency = 1;
	}
	else if(strcasecmp("addI", token) == 0){
		head->opcode= ADDI;
		head->ismem=1;
		head->latency = 1;
	}
	else if(strcasecmp("add", token) == 0){
		head->opcode= ADD;
		head->ismem=1;
		head->latency = 1;
	}
	else if(strcasecmp("subI", token) == 0){
		head->opcode= SUBI;
		head->ismem=1;
		head->latency = 1;
	}
	else if(strcasecmp("sub", token) == 0){
		head->opcode= SUB;
		head->ismem=1;
		head->latency = 1;
	}
	else if(strcasecmp("mult", token) == 0){
		head->opcode= MULT;
		head->ismem=1;
		head->latency = 3;
	}
	else if(strcasecmp("div", token) == 0){
		head->opcode= DIV;
		head->ismem=1;
		head->latency = 3;
	}
	else if(strcasecmp("load", token) == 0){
		head->opcode= LOAD;
		head->ismem=0;
		head->ismemread= 1;
		head->latency = 5;
	}
	else if(strcasecmp("loadI", token) == 0){
		head->opcode= LOADI;
		head->ismem=0;
		head->ismemread= 1;
		head->latency = 1;
	}
	else if(strcasecmp("loadAO", token) == 0){
		head->opcode= LOADAO;
		head->ismem=0;
		head->ismemread= 1;
		head->latency = 5;
	}
	else if(strcasecmp("loadAI", token) == 0){
		head->opcode= LOADAI;
		head->ismem=0;
		head->ismemread= 1;
		head->latency = 5;
	}
	else if(strcasecmp("store", token) == 0){
		head->opcode= STORE;
		head->ismem=0;
		head->ismemwrite = 1;
		head->latency = 5;
	}
	else if(strcasecmp("storeAO", token) == 0){
		head->opcode= STOREAO;
		head->ismem=0;
		head->ismemwrite= 1;
		head->latency = 5;
	}
	else if(strcasecmp("storeAI", token) == 0){
		head->opcode= STOREAI;
		head->ismem=0;
		head->ismemwrite= 1;
		head->latency = 5;
	}
	else if(strcasecmp("output", token) == 0){
		head->opcode= OUTPUT;
		head->ismem=0; //Need to ask prof about this
		head->ismemread= 1;
		head->ismemwrite= 1;
		head->latency = 1;
	}
}

void createReg(Instruction head, char* token, int index){
	Register reg = (Register) malloc(sizeof(struct Values));

	if(token[0] != 'r' && token[0] != 'R'){
		reg->isRegister = 1;
		reg->value = atoi(token);
	}
	else{
		reg->isRegister = 0;
		reg->value = atoi(&token[1]);
	}
	switch (index) {
	 	case 0:
			head->firstReg = reg;
			break;
		case 1:
			head->secondReg = reg;
			break;
		case 2:			
			head->outputReg1 = reg;
			break;
		case 3:			
			head->outputReg2 = reg;
			break;
		default:
			break;
	}
}


// Test to see if there is no file existing for stdin?


void printNode(Instruction head){
		Register r1 = head->firstReg;
		Register r2 = head->secondReg;
		Register o1 = head->outputReg1;
		Register o2 = head->outputReg2;
		switch(head->opcode){
			case NOP:
				fprintf(stdout, "nop\n");
				break;
			case ADD:
				fprintf(stdout, "add r%d, r%d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case SUB:
				fprintf(stdout, "sub r%d, r%d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case MULT:
				fprintf(stdout, "mult r%d, r%d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case DIV:
				fprintf(stdout, "div r%d, r%d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case LOADAO:
				fprintf(stdout, "loadAO r%d, r%d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case STOREAO:
				fprintf(stdout, "storeAO r%d	=> r%d, r%d\n", r1->value, o1->value, o2->value);
				break;
			case ADDI:
				fprintf(stdout, "addI r%d, %d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case SUBI:
				fprintf(stdout, "subI r%d, %d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case LOADAI:
				fprintf(stdout, "loadAI r%d, %d	=> r%d\n", r1->value, r2->value, o2->value);
				break;
			case LOAD:
				fprintf(stdout, "load r%d	=> r%d\n", r1->value, o1->value);
				break;
			case STORE:
				fprintf(stdout, "store r%d	=> r%d\n", r1->value, o1->value);
				break;
			case LOADI:
				fprintf(stdout, "loadI %d	=> r%d\n", r1->value, o1->value);
				break;
			case STOREAI:
				fprintf(stdout, "storeAI r%d	=> r%d, %d\n", r1->value, o1->value, o2->value);
				break;
			case OUTPUT:
				fprintf(stdout, "output %d\n", r1->value);
				break;
			default: //nop
				fprintf(stdout, "nop\n");
	}
}
void debugPrint(Instruction head){
	while(head != NULL){
		printNode(head);
		head = head->prev;
	}
}
void printAllDeps(Instruction head){
	Instruction temp = head;
	while(temp != NULL){
		printDeps(temp);
		temp = temp->prev;
	}
}

void printDeps(Instruction head){
	printNode(head);
	int i = 0;
	while(i < 5){
		printf("Dependency %d is: \n", i);
		if(head->depends[i]->restricts!=NULL){
			printNode(head->depends[i]->restricts);
		}
		i++;
	}
	i = 0;
	if(head->ismem==0){
		while(i < head->numInstructions){
			if(head->memoryDepends[i] != NULL){
				printf("Memory Dependency %d is: \n", i);
				printNode(head->memoryDepends[i]);
			}
			i++;
		}
	}
	puts(" ");
		
}

void print(Instruction head){
	//sort(head);
	while(head != NULL){
		printNode(head);
		printf("head cycle is: %d\n", head->cycle);
		head = head->next;
	}
}
