#include "atto-c.h"
/* exprParse.c

A simple expression parser.

Author: Terrence Moore
Date: Feb. 16, 2017

A Finite State machine is used to recognize strings consisting
of digits followed by a decimal point followed by digits.
There must be at least one digit before the decimal point.
The decimal point and digits after the decimal point are optional.

Usage: Launch.  A console window will appear.  You may type in
strings followed by ENTER, and they will either be accepted by
the Finite State Machine and print ACCEPT, or not and print REJECT.
The lexeme (the characters of the token) will also be printed.
To exit the program, type CTRL-Z and ENTER.

*/

//Global Variables
int jump_serial;
char *strings[STRINGS_MAX];
int string_serial;

/*

Procedure used to compare precedence of expressions in atto-c. It receives
the token type from lexer and compares its value to the lookahead token.
Will return the f precedence value for each expression token.

*/

int f(int token_type) //will return the f precedence value for each expression token
{	switch(token_type)
	{	case DOLLAR_TOK: return 2;
		case INTEGER_TOK: return 85;
		case IDENT_TOK: return 90;
		case RightParenth_TOK: return 85;
		case LeftParenth_TOK: return 10;
		case Negation_TOK: return 80;
		case Asterisk_TOK: return 70;
		case Plus_TOK: return 60;
		case LessThan_TOK: return 50;
		case Equal_TOK: return 40;
		case And_TOK: return 30;
		case Or_TOK: return 20;
		case Assign_TOK: return 15;
		case Comma_TOK: return 13;

		case Division_TOK: return 70;
		case Minus_TOK: return 60;
		case GreaterThan_TOK: return 50;
		case LessEqual_TOK: return 50;
		case GreaterEqual_TOK: return 50;
		case NotEqual_TOK: return 40;
		case UNIARY_MINUS_TOK: return 80;

		case Quote_TOK: return 85;
		


		default:
			printf("Expression parser f: Illegal token type: %d\n", token_type);
			exit(1);
	}
}

/*

Procedure used to compare precedence of expressions in atto-c. It receives
the token type from lexer and compares its value to the lookahead token.
Will return the g precedence value for each expression token.

*/

int g(int token_type)
{	switch (token_type)
	{	case DOLLAR_TOK: return 1;
		case INTEGER_TOK: return 85;
		case IDENT_TOK: return 90;
		case RightParenth_TOK: return 10;
		case LeftParenth_TOK: return 90;
		case Negation_TOK: return 81;
		case Asterisk_TOK: return 69;
		case Plus_TOK: return 59;
		case LessThan_TOK: return 49;
		case Equal_TOK: return 39;
		case And_TOK: return 29;
		case Or_TOK: return 19;
		case Assign_TOK: return 90;	
		case Comma_TOK: return 12;

		case Division_TOK: return 69;
		case Minus_TOK: return 59;
		case GreaterThan_TOK: return 49;	
		case LessEqual_TOK: return 49;
		case GreaterEqual_TOK: return 49;
		case NotEqual_TOK: return 39;
		case UNIARY_MINUS_TOK: return 81;

		case SemiColon_TOK: return 0;
		case Quote_TOK: return 85;

		default: printf("Expression parser f: Illegal token type: %d\n", token_type);
			exit(1);
	}
}

/*

Sets up stack for parser in order to save various information about the input items.

*/

struct stack_t {
	int token; //type of token in this stack position
	int expr_after; //1 for nonterminal following, 0 for not.
	int integer_value; // for the value of an integer token
	struct symbol_t *symbol;
};

//Stack set up
#define STACKMAX 1000
struct stack_t stack[STACKMAX];
int stacktop; //index of the current top of the stack

/*

will call lexer when it needs next lookahead token. Uses the lookahead's f and g 
values to determine its order of parsing. Prints to assembly file for each valid parsing.

*/

int expr_parser()
{
	fprintf(asmfile, "; Line %d\n", line_no);

	int eax_occupied = 0;

	// initialize stack
	stack[0].token = DOLLAR_TOK;
	stack[0].expr_after = 0;
	stacktop = 0;
	if (lookahead == 0) {

		lookahead = lexer();	//recieves lookahead from lexer return
	}
	while (1)
	{
		if (((lookahead == SemiColon_TOK)||(lookahead==RightParenth_TOK))&& (stacktop == 0))
			return lookahead;

		if (lookahead == Minus_TOK && !stack[stacktop].expr_after)
			lookahead = UNIARY_MINUS_TOK;

		if (lookahead == UNIARY_MINUS_TOK && stack[stacktop].expr_after)
			lookahead = Minus_TOK;

		if (f(stack[stacktop].token) <= g(lookahead))
		{
			//shift
			stack[stacktop].integer_value = atoi(lexeme);

			if (print_flag)
				printf("Shift \n");
			stacktop++;

			if (stacktop >= STACKMAX)
			{
				printf("Stack over flow.\n");
				exit(2);
			}
			if (lookahead == IDENT_TOK)
			{
				stack[stacktop].symbol = symbol_lookup(lexeme);
				if (stack[stacktop].symbol == NULL)
					stack[stacktop].symbol = push_symbol(lexeme);
			}
			if (lookahead==INTEGER_TOK)
				stack[stacktop].integer_value = atoi(lexeme);
			if (lookahead == Quote_TOK)
				stack[stacktop].integer_value = add_string(lexeme);

			stack[stacktop].token = lookahead;	//adds lookahead to stack
			stack[stacktop].expr_after = 0;	
			lookahead=lexer();
		}
		else
		{
			//reduce
			if (print_flag)
				printf("Reduce \n");
			switch (stack[stacktop].token)
			{

				case DOLLAR_TOK:
					if (stack[stacktop].expr_after)
					{
						return lookahead;
					}
					else
					{
						printf("DOLLAR without following expression, line%d \n", line_no);
					}
				case INTEGER_TOK:
					if (stack[stacktop].expr_after)
					{
						printf("INTEGER with following expression, line %d\n", line_no);
						exit(1);
					}
					if (stack[stacktop - 1].expr_after)
					{
						printf("INTEGER with preceding expression, line %d\n", line_no);
						exit(1);
					}
					
					if (eax_occupied)
						fprintf(asmfile, "    push eax\n");
					fprintf(asmfile, "    mov  eax,%d\n", stack[stacktop].integer_value);
					eax_occupied = 1;
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case IDENT_TOK:
					if (stack[stacktop].expr_after)
					{
						printf("IDENTIFIER with following expression, line %d\n", line_no);
						exit(1);
					}
					if (stack[stacktop - 1].expr_after)
					{
						printf("IDENTIFIER with preceding expression. line %d\n", line_no);
						exit(1);
					}

					if (stack[stacktop].symbol->symbol_kind == 0)
					{
						printf("Undeclared identifier '%s', line %d\n", stack[stacktop].symbol->name, line_no);
						exit(1);
					}
					if (stack[stacktop].symbol->symbol_kind == FUNCTION)
					{
						printf("Function name '%s' used as a variable, line %d\n", stack[stacktop].symbol->name, line_no);
						exit(1);
					}

					if (eax_occupied)
						fprintf(asmfile, "    push eax\n");
					fprintf(asmfile, "    mov  eax, DWORD PTR %s\n", stack[stacktop].symbol->asm_name);
					eax_occupied = 1;
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case RightParenth_TOK:
					if (stack[stacktop - 2].token != IDENT_TOK)
					{
						if (stack[stacktop - 1].token != LeftParenth_TOK)
						{
							printf("RIGHT PARENTHESIS without preceding LEFT PARENTHESES, line %d\n", line_no);
							exit(1);
						}
						if (!stack[stacktop - 1].expr_after)
						{
							printf("RIGHT PARENTHESIS without preceding expression. line %d\n", line_no);
							exit(1);
						}
						stacktop--; stack[stacktop].expr_after = 1;
						break;
					}
					if (stack[stacktop - 3].expr_after)
					{
						printf("RIGHT PARENTHESIS with expression after stack[stacktop-3] . line %d\n", line_no);
						exit(1);
					}
					if (stack[stacktop - 2].symbol->symbol_kind == 0)
					{ // must be an external function, such as printf
						stack[stacktop - 2].symbol->symbol_kind = FUNCTION;
						sprintf(stack[stacktop - 2].symbol->asm_name, "_%s", stack[stacktop - 2].symbol->name);
						fprintf(asmfile, "EXTRN %s:PROC\n", stack[stacktop - 2].symbol->asm_name);
					}
					int n;
					if (stack[stacktop - 1].expr_after >= 1)
						fprintf(asmfile, "   push eax\n");
					for (n = 1; n < stack[stacktop - 1].expr_after; n++)
						fprintf(asmfile, "  push DWORD PTR [esp+%d]\n", 8 * n - 4);
					fprintf(asmfile, "   call %s \n", stack[stacktop - 2].symbol->asm_name);
					if (stack[stacktop - 1].expr_after >= 1)
						fprintf(asmfile, "   add  esp,%d\n", 4 * (2 * stack[stacktop - 1].expr_after - 1));
					stacktop -= 3; stack[stacktop].expr_after = 1;
					break;
				case LeftParenth_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("LEFT without following expression, line %d\n", line_no);
						exit(1);
					}
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case Negation_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("NOT without preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "	cmp eax,0\n");
					fprintf(asmfile, "    jne  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial+1);
					jump_serial += 2;
			
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case Asterisk_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("Asterisk without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("Asterisk without preceding expression. line %d\n", line_no);
						exit(1);
					}
				
					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    imul  eax, ebx\n");

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case Plus_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("PLUS without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop-1].expr_after)
					{
						printf("PLUS without preceding expression. line %d\n", line_no);
						exit(1);
					}
					
					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    add  eax, ebx\n");

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case LessThan_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("LESS THAN without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("LESS THAN without preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp ebx,eax\n");
					fprintf(asmfile, "    jl  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case Equal_TOK:
					if(!stack[stacktop].expr_after)
					{
						printf("EQUAL without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("EQUAL without preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp eax,ebx\n");
					fprintf(asmfile, "    je  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case And_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("AND without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("AND without preceding expression. line %d\n", line_no);
						exit(1);
					}
	

					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp eax,0\n");
					fprintf(asmfile, "    je  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    cmp ebx,0\n");
					fprintf(asmfile, "    je SHORT jumper@%d\n", jump_serial );
					fprintf(asmfile, "    mov eax, 1\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case Or_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("AND without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("AND without preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp eax,0\n");
					fprintf(asmfile, "    jne  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    cmp ebx,0\n");
					fprintf(asmfile, "    jne SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial+1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case Assign_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("ASSIGN without following expression, line %d\n", line_no);
						exit(1);
					}
					if (stack[stacktop - 1].expr_after==IDENT_TOK)
					{
						printf("ASSIGN with preceding identifier. line %d\n", line_no);
						exit(2);
					}
					fprintf(asmfile, "    mov DWORD PTR %s,eax\n", stack[stacktop - 1].symbol->asm_name);

					stacktop-=2; stack[stacktop].expr_after = 1;
					break;
				case Comma_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("COMMA without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("COMMA without preceding expression. line %d\n", line_no);
						exit(1);
					}
					if (stack[stacktop - 1].token == LeftParenth_TOK)
						stack[stacktop - 1].expr_after++;
					else
						stack[stacktop - 1].expr_after = 1;
					stacktop--;
					break;
				case Division_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("DIVISION without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("DIVISION without preceding expression. line %d\n", line_no);
						exit(1);
					}
				
					fprintf(asmfile, "    mov  ebx, eax\n");
					fprintf(asmfile, "    pop eax\n");
					fprintf(asmfile, "    cdq \n");
					fprintf(asmfile, "    idiv ebx\n");

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case Minus_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("MINUS without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("MINUS without preceding expression. line %d\n", line_no);
						exit(1);
					}
					
					fprintf(asmfile, "    mov  ebx, eax\n");
					fprintf(asmfile, "    pop eax\n");
					fprintf(asmfile, "    sub eax, ebx\n");

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case GreaterThan_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("GREATER THAN without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("GREATER THAN without preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp ebx,eax\n");
					fprintf(asmfile, "    jg  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case LessEqual_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("LESS THAN EQUAL without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("LESS THAN EQUAL without preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp ebx,eax\n");
					fprintf(asmfile, "    jle  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case GreaterEqual_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("GREAATER THAN EQUAL without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("GREATER THAN EQUAL without preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp ebx,eax\n");
					fprintf(asmfile, "    jge  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;

					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case NotEqual_TOK: 
					if (!stack[stacktop].expr_after)
					{
						printf("NOT EQUAL without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("NOT EQUAL without preceding expression. line %d\n", line_no);
						exit(1);
					}
					fprintf(asmfile, "    pop ebx\n");
					fprintf(asmfile, "    cmp eax,ebx\n");
					fprintf(asmfile, "    jne  SHORT jumper@%d\n", jump_serial);
					fprintf(asmfile, "    mov eax,0\n");
					fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
					fprintf(asmfile, "jumper@%d:\n", jump_serial);
					fprintf(asmfile, "    mov eax,1\n");
					fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
					jump_serial += 2;
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case UNIARY_MINUS_TOK:
					if (stack[stacktop - 1].expr_after)
					{
						printf("UNIARY MINUS with preceding expression. line %d\n", line_no);
						exit(1);
					}

					fprintf(asmfile, "    neg eax\n");
					stacktop--; stack[stacktop].expr_after = 1;
					break;
				case SemiColon_TOK:
					if (!stack[stacktop].expr_after)
					{
						printf("SEMICOLON without following expression, line %d\n", line_no);
						exit(1);
					}
					if (!stack[stacktop - 1].expr_after)
					{
						printf("SEMICOLON without preceding expression. line %d\n", line_no);
						exit(1);
					}
				case Quote_TOK:
					if (stack[stacktop - 1].expr_after)
					{
						printf("Illegal string following expression, line %d\n", line_no); exit(1);
					}
					if (eax_occupied)
						fprintf(asmfile, "   push eax\n");
					fprintf(asmfile, "   mov  eax, OFFSET string@%d\n", stack[stacktop].integer_value);
					eax_occupied = 1;
					stacktop--; stack[stacktop].expr_after = 1;
					break;

					//always have a default in a switch
				default:					
					printf("Bad stacktop %d\n", stack[stacktop].token);
					exit(2);
				

			}
		}
	}
}
//string length
#define STRINGS_MAX 100000
char *strings[STRINGS_MAX];

/*
Procedure add_string()
Purpose:  Add string to the list for putting in CONST segment.
Returns the serial number for the string.
*/

int add_string(char *string)
{
	if (string_serial >= STRINGS_MAX)
	{
		printf("Too many strings.\n");
		exit(33);
	}
	strings[string_serial] = (char*)calloc(strlen(string) + 1, sizeof(char));
	strcpy(strings[string_serial], string);
	return string_serial++;
}