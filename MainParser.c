#include "atto-c.h"
/*
Author: Terrence MOore
Date : May 12, 2017

Purpose:
The main parser is a top-down predictive parser that implements grammar 
rules using the tokens on the right side of the rules to call the
corresponging grammar procedure.

*/

//Loop Stack setup
#define LOOPS_MAX 10000
int loop_stack[LOOPS_MAX];
int loop_stacktop;

int return_target;

//prototypes
void program(void);
void external_definition(void);
void function_definition(struct symbol_t *s);
void parameter_list(void);
void data_definition(struct symbol_t *s);
void data_definition_2(void);  
void declaration_list(void);
void compound_statement(void);
void statement_list(void);
void statement(void);
void expression_statement(void);
void if_statement(void);
void while_statement(void);
void do_statement(void);
void continue_statement(void);
void break_statement(void);
void return_statement(void);
void else_statement(void);

//variables
int lookahead;	//the lookahead
int local_offset;

//handles calling main parser, expression parser and lexer.
void program()
{
	int symbol_spot;

	if (print_flag)
		printf("Entering program, line %d.\n", line_no);
	symbol_spot = start_scope();
	lookahead = lexer();  // Get the first token of the program.
	while (lookahead != EOF_TOK)
		external_definition();
	end_scope(symbol_spot);
	if (print_flag)
		printf("Leaving program, line %d.\n", line_no);
}

//checks for int and identifier for data-definition and function-definition
void external_definition()
{
	if (print_flag)
		printf("Entering external definition, line %d.\n", line_no);
	struct symbol_t *s;

	if (lookahead != Int_TOK)
	{
		printf("Missing type specifier for external definition. Line %d. \n", line_no);
		exit(2);
	}
	lookahead = lexer();
	if (lookahead != IDENT_TOK) {
		printf("Missing identifier for external definition. Line %d. \n", line_no);
		exit(2);
	}
	s = push_symbol(lexeme);
	lookahead = lexer();
	if (lookahead == LeftParenth_TOK)
	{
		s->symbol_kind = FUNCTION;
		sprintf(s->asm_name, "_%s", s->name);
		function_definition(s);
	}
	else {
		data_definition(s);
	}
	if (print_flag)
		printf("Leaving external definition, line %d.\n", line_no);
}

/*

checks for left and right parenthesis and calls parameter-list and compound-statement 
then sends assembly to assembly file to begin a new stack frame.

*/
void function_definition(struct symbol_t *s)
{

	if (print_flag)
		printf("Entering function definition, line %d.\n", line_no);

	fprintf(asmfile, "\nPUBLIC %s\n", s->asm_name);
	fprintf(asmfile, ";    COMDAT %s\n", s->asm_name);

	fprintf(asmfile, "\n_TEXT    SEGMENT\n");

	fprintf(asmfile, "%s    PROC          ; COMDAT\n\n", s->asm_name);

	fprintf(asmfile, "   push  ebp\n");
	fprintf(asmfile, "   mov   ebp,esp\n");
	fprintf(asmfile, "   sub   esp,%s_locals$\n", s->asm_name);

	fprintf(asmfile, "   push  ebx\n");
	fprintf(asmfile, "   push  esi\n");
	fprintf(asmfile, "   push  edi\n");


	
	int symbol_spot;

	symbol_spot = start_scope();

	if (lookahead != LeftParenth_TOK)
	{
		printf("Missing left parenthesis for function definition. Line %d. \n", line_no);
		exit(2);
	}
	lookahead = lexer();
	parameter_list();
	local_offset = -4;

	return_target = jump_serial;
	jump_serial++;

	compound_statement();

	fprintf(asmfile, "$return@%d:\n", return_target);

	if (print_flag)
		printf("Leaving function definition, line %d.\n", line_no);
	lookahead = lexer();
	end_scope(symbol_spot);
	
	fprintf(asmfile, "   pop   edi\n");
	fprintf(asmfile, "   pop   esi\n");
	fprintf(asmfile, "   pop   ebx\n");

	fprintf(asmfile, "   mov esp, ebp\n");

	fprintf(asmfile, "   pop   ebp\n");

	fprintf(asmfile, "   ret   0\n");

	fprintf(asmfile, "%s   ENDP\n", s->asm_name);
	fprintf(asmfile, "_TEXT    ENDS\n");
	fprintf(asmfile, "%s_locals$ = %d\n", s->asm_name, -local_offset - 4);
}

/*

Receives Int_tok and identifier in order to push onto stack 
and use as parameters for the set stack frame

*/
void parameter_list()
{
	if (print_flag)
		printf("Entering parameter list, line %d.\n", line_no);
	struct symbol_t*ss;
	int offset = 8;

	while (lookahead != RightParenth_TOK)
	{
		if (lookahead == Int_TOK)
		{
			lookahead = lexer();
			if (lookahead != IDENT_TOK)
			{
				printf("Missing identifier for parameter list. Line %d.", line_no);
				exit(2);
			}
			ss = push_symbol(lexeme);
			sprintf(ss->asm_name, "_%s$[ebp]", ss->name);
			ss->symbol_kind = FUNCTION_PARAMETER;
			fprintf(asmfile, "_%s$ = %d\n", ss->name, offset);
			offset += 4;
			
			lookahead = lexer();
			if (lookahead == Comma_TOK)
			{
				lookahead = lexer();
			}
		}
	}
	lookahead = lexer();

	if (print_flag)
		printf("Leaving parameter list, line %d.\n", line_no);
}

/*

Handles all global variables in external definition. Puts variables into the Data Segment rather than stack.

*/
void data_definition(struct symbol_t *s)    // see note g below
{
	if (print_flag)
		printf("Entering data definition, line %d.\n", line_no);
	struct symbol_t*ss;

	fprintf(asmfile, "\n_DATA    SEGMENT\n");
	s->asm_name[0] = '_';
	strncpy(s->asm_name + 1, s->name, IDENT_MAX_LENGTH);
	fprintf(asmfile, "COMM  %s:DWORD\n", s->asm_name);
	s->symbol_kind = GLOBAL_VARIABLE;
	while (lookahead != SemiColon_TOK)
	{
		if (lookahead != Comma_TOK)
		{
			printf("Missing comma in data definition. Line %d.", line_no);
			exit(2);
		}
		else {
			lookahead = lexer();
		}
		if (lookahead != IDENT_TOK)
		{
			printf("Missing identifier in data definition. Line %d.\n", line_no);
			exit(2);
		}
		ss = push_symbol(lexeme);

		sprintf(ss->asm_name, "COMM %s$%d[ebp]", ss->name, local_offset / (-4));
		ss->symbol_kind = LOCAL_VARIABLE;
		fprintf(asmfile, "%s$%d = %d\n", ss->name, local_offset / (-4), local_offset);
		local_offset -= 4;

		lookahead = lexer();
	}
	fprintf(asmfile, "_DATA    ENDS\n");
	if (print_flag)
		printf("Leaving data definition, line %d.\n", line_no);
	lookahead = lexer();
}

/*

Handles all local variables in compound statement. Pushes variables onto the stack.

*/

void data_definition_2()
{
	if (print_flag)
		printf("Entering data definition 2, line %d.\n", line_no);
	struct symbol_t*ss;

	if (lookahead != Int_TOK)
	{
		printf("Missing type specifier in data definition 2. Line %d.", line_no);
		exit(2);
	}
	lookahead = lexer();
	while (lookahead != SemiColon_TOK)
	{
		if (lookahead != IDENT_TOK)
		{
			printf("Missing  identifier for data-definition 2. Line %d.\n", line_no);
			exit(2);
		}
		ss=push_symbol(lexeme);

		ss->asm_name[0] = '_';
		sprintf(ss->asm_name, "_%s$%d[ebp]", ss->name, local_offset / (-4));
		ss->symbol_kind = LOCAL_VARIABLE;
		fprintf(asmfile, "_%s$%d = %d\n", ss->name, local_offset / (-4), local_offset);
		local_offset -= 4;

		lookahead = lexer();

		if (lookahead == Comma_TOK) 
		{
			lookahead = lexer();
		}
		else if(lookahead!=IDENT_TOK)
		{
			if (lookahead != SemiColon_TOK)
			{
				printf("Missing  semicolon for data-definition 2. Line %d.\n", line_no);
				exit(2);
			}
		}
	}
	if (print_flag)
		printf("Leaving data defintion 2, line %d.\n", line_no);
}

/*

Procedure to insure that multiple variable definitions can occur witohut error.

*/

void declaration_list()
{
	if (print_flag)
		printf("Entering declaration list, line %d.\n", line_no);
	while (lookahead == Int_TOK)
	{
		data_definition_2();
		lookahead = lexer();
	}
	if (print_flag)
		printf("Leaving declaration list, line %d.\n", line_no);
}

/*

Recieves left curly brackets then starts a new scope in order to receive 
declarations and statements.

*/

void compound_statement()
{
	if (print_flag)
		printf("Entering compound statement, line %d.\n", line_no);
	
	int symbol_spot;

	symbol_spot = start_scope();

	if (lookahead != LeftCurly_TOK)
	{
		printf("Missing left curly brace for compound statement. Line %d.\n", line_no);
		exit(7);
	}
	lookahead = lexer();  // eat left curly brace
	declaration_list();// do any local variable declarations
	statement_list();
	if (lookahead != RightCurly_TOK)
	{
		printf("Missing right curly brace at end of compound statement. Line %d.",line_no);
		exit(2);
	}

	end_scope(symbol_spot);

	if (print_flag)
		printf("Leaving compound statement, line %d.\n", line_no);
}

/*

Handles all statements made within a compound statement and does not end until a right curly 
bracket is passed to the function.

*/

void statement_list()
{
	if (print_flag)
		printf("Entering statement list, line %d.\n", line_no);
	while (lookahead != RightCurly_TOK)
		statement();
	if (print_flag)
		printf("Leaving statement list, line %d.\n", line_no);
}

/*

Procedure that determines the type of statement to enter based on the lookahead
token received from lexer.

*/

void statement()
{
	if (print_flag)
		printf("Entering statement, line %d.\n", line_no);
	if (lookahead == LeftCurly_TOK)
	{
		compound_statement();
		lookahead = lexer();
	}
	else if (lookahead == If_TOK)
	{
		lookahead = lexer();
		if_statement();
	}
	else if (lookahead == While_TOK)
	{
		lookahead = lexer();
		while_statement();
	}
	else if (lookahead == Do_TOK)
	{
		lookahead = lexer();
		do_statement();
	}
	else if (lookahead == Continue_TOK)
	{
		lookahead = lexer();
		continue_statement();
	}
	else if (lookahead == Break_TOK)
	{
		lookahead = lexer();
		break_statement();
	}
	else if (lookahead == Return_TOK)
	{
		lookahead = lexer();
		return_statement();
	}
	else if (lookahead == Else_TOK)
	{
		lookahead = lexer();
		else_statement();
	}
	else if ((lookahead == IDENT_TOK ) || (lookahead==INTEGER_TOK )|| (lookahead==LeftParenth_TOK) || (lookahead == Minus_TOK) || (lookahead == Negation_TOK))
	{
		expression_statement();
	}
	if (lookahead == SemiColon_TOK)
	{
		if (print_flag)
			printf("Leaving statement, line %d.\n", line_no);
		lookahead = lexer();
	}
}

/*

Handles all expression and calls expression parser.

*/

void expression_statement()
{
	if (print_flag)
		printf("Entering expression statement, line %d.\n", line_no);
	expr_parser();
	if ((lookahead != RightParenth_TOK)&&(lookahead!=SemiColon_TOK))
	{
		printf("Missing semicolon or right parenthesis in expression statement. Line %d.\n", line_no);
		exit(2);
	}
	if (print_flag)
		printf("Leaving expression statement, line %d.\n", line_no);
}

/*

Checks lookahead to insure proper if statement grammar is being applied in input code
and produces if statement in assembly for the assembly file.

*/
void if_statement()
{
	if (print_flag)
		printf("Entering if statement, line %d.\n", line_no);

	int if_target;
	int else_target;

	if (lookahead != LeftParenth_TOK)
	{
		printf("Missing left parenthesis for if statement. Line %d.\n", line_no);
		exit(2);
	}
	lookahead = lexer();
	expression_statement();

	lookahead = lexer();

	fprintf(asmfile, "   cmp eax, 0\n");
	fprintf(asmfile, "   je  $if@%d\n", jump_serial);
	if_target = jump_serial;
	jump_serial++;

	statement();
	if (lookahead == Else_TOK)
	{
		lookahead = lexer();
		fprintf(asmfile, "   jmp  $if@%d\n", jump_serial);
		else_target = jump_serial;
		jump_serial++;
		fprintf(asmfile, "$if@%d: \n", if_target);
		statement();
		fprintf(asmfile, "$if@%d: \n", else_target);
	}
	else {
		fprintf(asmfile, "$if@%d: \n", if_target);
	}
	if (print_flag)
		printf("Leaving if statement, line %d.\n", line_no);
}

/*

Checks lookahead to insure proper while statement grammar is being applied in input code
and produces while statement in assembly for the assembly file.

*/

void while_statement()
{
	if (print_flag)
		printf("Entering while statement, line %d.\n", line_no);

	int while_target;
	int while_target2;
	int continue_target;

	if (lookahead != LeftParenth_TOK)
	{
		printf("Missing left parenthesis for if statement. Line %d.\n", line_no);
		exit(2);
	}

	fprintf(asmfile, "$while@%d: \n", jump_serial);
	while_target = jump_serial;
	jump_serial++;
	while_target2 = jump_serial;
	jump_serial++;
	continue_target = jump_serial;
	fprintf(asmfile, "$cont@%d: \n", continue_target);
	loop_stack[++loop_stacktop] = jump_serial;
	jump_serial++;

	lookahead = lexer();
	expression_statement();
	lookahead = lexer();

	fprintf(asmfile, "   cmp eax, 0\n");
	fprintf(asmfile, "   je  $while@%d\n", while_target2);
	statement();
	fprintf(asmfile, "   jmp  $while@%d\n",while_target);
	fprintf(asmfile, "$break@%d: \n", continue_target);

	fprintf(asmfile, "$while@%d: \n", while_target2);

	loop_stacktop--;
	if (print_flag)
		printf("Leaving while statement, line %d.\n", line_no);
}

/*

Checks lookahead to insure proper do statement grammar is being applied in input code
and produces do statement in assembly for the assembly file.

*/

void do_statement()
{
	if (print_flag)
		printf("Entering do statement, line %d.\n", line_no);

	int do_target;
	int continue_target;
	
	fprintf(asmfile, "$do@%d: \n", jump_serial);
	do_target = jump_serial;
	jump_serial++;

	continue_target = jump_serial;
	jump_serial++;
	loop_stack[++loop_stacktop] = continue_target;
	statement();
	if (lookahead != While_TOK)
	{
		printf("Missing while statement for do statement. Line %d.", line_no);
		exit(2);
	}
	fprintf(asmfile, "$cont@%d: \n", continue_target);

	lookahead = lexer();
	expression_statement();
	fprintf(asmfile, "   cmp eax, 0\n");
	fprintf(asmfile, "   jne  $do@%d\n", do_target);
	fprintf(asmfile, "$break@%d: \n", continue_target);

	if(lookahead==LeftParenth_TOK)
		lookahead = lexer();
	if (lookahead != SemiColon_TOK)
	{
		printf("Missing semicolon for do statement. Line %d.", line_no);
		exit(2);
	}
	loop_stacktop--;
	if (print_flag)
		printf("Leaving do statement, line %d.\n", line_no);
}

/*

Checks lookahead to insure proper continue statement grammar is being applied in input code
and produces continue statement in assembly for the assembly file.

*/

void continue_statement()
{
	if (print_flag)
		printf("Entering continue statement, line %d.\n", line_no);
	if (lookahead != SemiColon_TOK)
	{
		printf("Missing semicolon in continue statement. Line %d.\n", line_no);
		exit(2);
	}
	if (loop_stacktop <= 0)
	{
		printf("CONTINUE not in a loop. Line %d\n", line_no);
		exit(46);
	}
	fprintf(asmfile, "   jmp $cont@%d\n", loop_stack[loop_stacktop]);
}

/*

Checks lookahead to insure proper break statement grammar is being applied in input code
and produces break statement in assembly for the assembly file.

*/
void break_statement()
{
	if (print_flag)
		printf("Entering break statement, line %d.\n", line_no);
	if (lookahead != SemiColon_TOK)
	{
		printf("Missing semicolon in break statement. Line %d.\n", line_no);
		exit(2);
	}
	if (loop_stacktop <= 0)
	{
		printf("BREAK not in a loop. Line %d\n", line_no);
		exit(47);
	}
	fprintf(asmfile, "   jmp $break@%d\n", loop_stack[loop_stacktop]);
}

/*

Checks lookahead to insure proper return statement grammar is being applied in input code
and produces return statement in assembly for the assembly file.

*/

void return_statement()
{
	if (print_flag)
		printf("Entering return statement, line %d.\n", line_no);
	if ((lookahead == IDENT_TOK) || (lookahead == INTEGER_TOK) || (lookahead == LeftParenth_TOK) || (lookahead == Minus_TOK) || (lookahead == Negation_TOK))
	{
		statement();
	}
	else if (lookahead != SemiColon_TOK)
	{
		printf("Missing semicolon in return statement. Line %d. \n", line_no);
		exit(2);
	}
	fprintf(asmfile, "   jmp $return@%d\n", return_target);
}

/*

Checks lookahead to insure proper else statement grammar is being applied in input code
and produces else statement in assembly for the assembly file.

*/

void else_statement()
{
	if (print_flag)
		printf("Entering else statement, line %d.\n", line_no);
	if (lookahead == LeftCurly_TOK)
	{
		statement_list();
	}
	statement();
	if (print_flag)
		printf("Leaving else statement, line %d.\n", line_no);
}