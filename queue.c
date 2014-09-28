#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <unistd.h>
#include <getopt.h>
#include "schedutils.h"

void push(Queue* queue, Instruction instr){
	
	Node* n = (Node*) malloc(sizeof(Node));
	n->pointer = instr;
	n->next = NULL;
	
	if(queue->head == NULL || n->pointer->priority > queue->head->pointer->priority){
		n->next = queue->head;
		queue->head = n;
		queue->size++;
	}
	else{
		Node* temp = queue->head;
		Node* p = temp->next;
		while(p != NULL){
			if(p->pointer->priority < n->pointer->priority){
				
				n->next = p;
				temp->next = n;
				queue->size++;
				return;
			}
			p = p->next;
			temp = temp->next;
		}
	
		n->next = NULL;
		temp->next = n;
		queue->size++;
	}
}

Instruction pop (Queue* queue){
	
	Node* head = queue->head;
	Instruction node = head->pointer;
	queue->head = head->next;
	queue->size--;
	
	free(head);
	return node;
}

Instruction peek(Queue* queue){
	Node* head = queue->head;
	if(head== NULL)
		return NULL;
	return head->pointer;
}

Instruction removeNode(Queue* queue, Instruction instr){
	Node* head = queue->head;
	if(head == NULL){
		return NULL;
	}
	else if(head->next == NULL && head->pointer!= instr)
		return NULL;
	else if(head->next == NULL && head->pointer == instr){
		queue->head = head->next;
		queue->size--;
		free(head);
		puts("remove success");
		return instr;
	}
	while(head->next != NULL){
		Instruction node = head->next->pointer;
		if(node == instr){
			Node* temp = head->next;
			head->next = temp->next;
			queue->size--;
			free(temp);
			puts("remove success");
			return node;
		}
		head = head->next;
	}
	return NULL;
}

Queue createQueue() {
	Queue queue;
	queue.size = 0;
	queue.head = NULL;
	queue.tail = NULL;
	queue.pop = &pop;
	queue.push = &push;
	queue.peek = &peek;
	queue.removeNode = &removeNode;
	return queue;
}
