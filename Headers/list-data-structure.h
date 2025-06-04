#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0

/**
 * Enumeration for menu choices to improve readability.
 */
typedef enum {
	INSERT_FIRST = 1,
	INSERT_LAST,
	INSERT_SPECIFIC,
	DELETE_FIRST,
	DELETE_LAST,
	DELETE_SPECIFIC,
	MEMBER_CHECK,
	REVERSE,
	DISPLAY,
	GET_LENGTH,
	EXIT_PROGRAM
} MenuChoices;

/**
 * Node structure for singly linked list.
 */
typedef struct node {
	int info;
	struct node* next;
} node;

/**
 * LinkedList is a pointer to the first node in the list.
 */
typedef node* LinkedList;

/* Function prototypes */
void insert_first(LinkedList* pstart);
void insert_last(LinkedList* pstart);
void insert_specific(LinkedList* pstart);
void delete_first(LinkedList* pstart);
void delete_last(LinkedList* pstart);
void delete_specific(LinkedList* pstart);
void member(LinkedList start);
void reverse(LinkedList* pstart);
void display(LinkedList start);
int get_length(LinkedList start);
