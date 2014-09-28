#ifndef SCHEDUTILS
#define SCHEDUTILS

#include <stdlib.h>
// The different possible instructions

typedef enum { NOP=1, ADDI, ADD, SUBI, SUB, MULT, DIV, LOAD, LOADI, LOADAO, LOADAI, STORE, STOREAO, STOREAI, OUTPUT } CodeType;

struct Values {
	int isRegister; // tells whether if register or not
	int value; // if it is a register, tells the register number. It may also tell the number value 
	int dependencyType; // 0 if RAW (true), 1 if WAR (anti)
};
typedef struct Values* Register; 

struct Restriction {
	struct Command* restricts;
	int isAnti; // set to 1 if it's anti. Set to 50 if it's a memory, default to 0
	int bothTrueMem; //set to 1 if the memory link is a true one
	int depAntiDone;//set to 1 if the restriction is done processing (dependency dead)
	int depMemDone;//set to 1 if the restriction is done proceessing (dependency mem dead)
};
typedef struct Restriction* Dependency;
	
struct Command {
	CodeType opcode; // What type of instruction this holds
	int latency;
	struct Command* next;
	struct Command* prev;
	int ismem; // Flag to determine whether this instruction is movable or not
	int ismemread;
	int ismemwrite;
	Register firstReg; // Will always have something
	Register secondReg; // May be null
	Register outputReg1; // May be null
	Register outputReg2; // May be null
	Dependency depends[5];// 0 is for mem Read, 1 through 4  are same
	struct Command** memoryDepends;
	int numdepends;
	struct Command** successors; // successors allocate index = # of instructions
	int numInstructions;
	int priority;
	int cycle;
	int delay;
	int isDone;
	int hasAnti;
	int isReady;
	int isActive;
	int isLeaves;
	int hasMemTrue;
	int found;
};

typedef struct Command* Instruction;

typedef struct Node {
	Instruction pointer;
	struct Node* next;
} Node;

typedef struct Queue {
	Node* head;
	Node* tail;

	void (*push) (struct Queue*, Instruction);
	Instruction (*pop) (struct Queue*);
	Instruction (*peek) (struct Queue*);
	Instruction (*removeNode) (struct Queue*, Instruction);
	int size;
} Queue;


int notreadyMemCheck(Instruction node);

void addReady(Queue* ready, Instruction* active, Instruction node, int index);
 
void push(Queue* queue, Instruction instr);

Instruction pop (Queue* queue);

Instruction peek(Queue* queue);

Instruction removeNode(Queue* queue, Instruction instr);

Queue createQueue();

void chooseOp(Instruction head, char* token);

void init(Instruction* head);

void destroy(Instruction node);

void createReg(Instruction head, char* token, int index);

void create(Instruction* head);

void destroyRep(Instruction head);

void buildDeps(Instruction head);

void longlatency(Instruction head);

void highlatency(Instruction head);

void choice(Instruction head);

void memory_deps(Instruction head, int index);

void deps(Instruction head);

void sort(Instruction head);

void helperLL(Instruction head, int extra, int isAnti);

void print(Instruction head);

void printDeps(Instruction head);

void printAllDeps(Instruction head);

void schedule(Instruction head);

void addIn(Instruction temp, Instruction head, int index);

int readyCheck(Instruction node);

void debugPrint(Instruction head);

void printNode(Instruction head);

#endif


