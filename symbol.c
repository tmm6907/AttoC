#include "atto-c.h"

/*
Author: Terrence MOore
Date : May 12, 2017

main.c

Purpose: 
Serves as the driving function of the atto-c compiler.
Calls all the necessary functions to operate compiler.

*/

//data declarations
struct symbol_t symbol_table[MAXSYMBOL];
int symbol_stack_top;
struct symbol_t *symbol;


//pushes symbol onto stack table stack
struct symbol_t *push_symbol(char *name)
{
	symbol_stack_top++;
	strcpy(symbol_table[symbol_stack_top].name, name);
	if (symbol_table + symbol_stack_top == MAXSYMBOL)
	{
		printf("Symbol table stack overflow.");
		exit(2);
	}
	if (print_flag)
		printf("Adding symbol '%s', table index %d, line %d\n", name, symbol_stack_top, line_no);
	return symbol_table + symbol_stack_top;
}

//starts scope
int start_scope()
{
	if (print_flag)
		printf("Starting scope, table index %d, line %d\n",symbol_stack_top, line_no);
	return symbol_stack_top;
}

//pops scope off the symbol table stack
void end_scope(int prev_stack_top)
{
	if ((prev_stack_top >= 0) && (prev_stack_top <= symbol_stack_top))
	{
		symbol_stack_top = prev_stack_top;
		if (print_flag)
			printf("Ending scope, table index %d, line %d\n", symbol_stack_top, line_no);
	}
	else {
		printf("Previous stack top less than 0 or greater than current stack top");
		exit(2);
	}
}

//looks up identifier and returns either a pointer to the found symbol table entry or null if it is not found
struct symbol_t *symbol_lookup(char *name)
{
	int k;
	if (print_flag)
		printf("Searching for '%s', in symbol table, table index %d, line %d\n", name, symbol_stack_top, line_no);
	for (k = symbol_stack_top; k > 0; k--)
	{
		if (strcmp(name, symbol_table[k].name) == 0)
			return symbol_table + k;
	}
	return NULL;
}