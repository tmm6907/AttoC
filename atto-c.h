#pragma once

/*atto-c.h

A modified lexical analyzer for our overall compiler program.

Author: Terrence MOore
Date: Feb. 07, 2017

Purpose:
Program that holds all of the global declarations that the rest of 
the parser needs in order to run. 
*/

//Included Libraries
#include <stdio.h>		// standard input-output declarations: printf, stdin
#include <stdlib.h>		// standard library declarations: exit
#include <ctype.h>		// character type test declarations:  isdigit, isalpha, isalnum
#include <string.h>		// used for strcmp function

//Length Stuff
#define MAX_LEXEME 100000000
#define IDENT_MAX_LENGTH 50
#define NUMBER_MAX_LENGTH 10

#define MAXSYMBOL 1000

// Symbol classification
#define GLOBAL_VARIABLE    1
#define LOCAL_VARIABLE     2
#define FUNCTION_PARAMETER 3
#define FUNCTION           4

#define STRINGS_MAX 100000


//external variables
extern int print_flag; extern char lexeme[MAX_LEXEME]; //The characters of the token
extern int line_no;	//line number in current input file
extern int lookahead;  // The lookahead token
extern int jump_serial;

extern FILE *sourcefile;
extern FILE *asmfile;

extern struct symbol_t symbol_table[MAXSYMBOL];
extern int symbol_stack_top;

extern char *strings[STRINGS_MAX];
extern int string_serial;

extern void output_strings(void);


// Token Types
#define INTEGER_TOK				1001
#define DECIMAL_FRACTION_TOK	1002

// More Token Types
#define LeftParenth_TOK			1003
#define RightParenth_TOK		1004
#define	Comma_TOK				1005
#define IDENT_TOK				1006
#define	SemiColon_TOK			1007
#define LeftCurly_TOK			1008
#define RightCurly_TOK			1009
#define Minus_TOK				1010
#define Negation_TOK			1011
#define Asterisk_TOK			1012
#define Plus_TOK				1013
#define LessThan_TOK			1014
#define	Assign_TOK				1015
#define Equal_TOK				1016
#define And_TOK					1017
#define Or_TOK					1018
#define GreaterThan_TOK			1020
#define NotEqual_TOK			1021
#define GreaterEqual_TOK		1022
#define LessEqual_TOK			1023
#define Blank_TOK				1025
#define Tab_TOK					1026
#define NewLine_TOK				1027
#define Quote_TOK				1028
#define Division_TOK			1029
#define Backspace_TOK			1031
#define CommentShort_TOK		1032
#define CommentLong_TOK			1033

//Even More Token Types
#define Auto_TOK				1034
#define Break_TOK				1035
#define Case_TOK				1036
#define Char_TOK				1037
#define Continue_TOK			1038
#define Default_TOK				1039
#define Do_TOK					1040
#define Double_TOK				1041
#define Else_TOK				1042
#define Enum_TOK				1043
#define Extern_TOK				1044
#define Float_TOK				1045
#define For_TOK					1046
#define Goto_TOK				1047
#define If_TOK					1048
#define Int_TOK					1049
#define Long_TOK				1050
#define Register_TOK			1051
#define Return_TOK				1052
#define Short_TOK				1053
#define Sizeof_TOK				1054
#define Static_TOK				1055
#define Struct_TOK				1056
#define Switch_TOK				1057
#define Typedef_TOK				1058
#define Union_TOK				1059
#define Unsigned_TOK			1060
#define While_TOK				1061
#define EOF_TOK					1062
#define String_TOK				1063
#define Declaration_TOK			1064
#define Backslash_TOK			1065
#define DOLLAR_TOK				1066
#define UNIARY_MINUS_TOK		1067
#define DOUBLE_TOK				1068

//function initializers
int lexer(void);
int expr_parser(void);
void program(void);
int start_scope(void);
void end_scope(int prev_stack_star);

void preamble(char *sourcefilename);
void postamble(void);

//Structure initializers
struct symbol_t *push_symbol(char *name);
struct symbol_t *symbol_lookup(char *name);

// Symbol table declarations
struct symbol_t {
	char name[IDENT_MAX_LENGTH + 1];
	char asm_name[IDENT_MAX_LENGTH + 1];
	int symbol_kind;
};