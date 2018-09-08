
/* lexer.c

A simple lexical analyzer for atto-c.

Terrence MOore
Jan. 26, 2017

A simple lexical analyzer for atto-c used to recognize input strings in order to add
acceptable (readable tokens for the language atto-c).
tokens 

*/


#include "atto-c.h"


// Finite State Machine states
#define START  1
#define BEFORE 2
#define AFTER  3
#define FINAL  4

// More States
#define QUOTE					110
#define COMMENT_LONG			111
#define COMMENT_SHORT			112
#define NOT_EQUAL				113
#define EQUAL_TO				114
#define LESS_THAN				115
#define AND_STATE				116
#define OR_STATE				117
#define BACKSLASH_IN_QUOTE		118
#define FRONTSLASH				119
#define IDENT					120
#define GREATER_THAN			121
#define NUMBER					123
#define COMMENT					124
#define END_COMMENT_LONG		125
#define INT_STATE				126

//Keyword set lists
#define KEYWORDS_MAX  28
char *keywords[KEYWORDS_MAX] = { "auto", "break", "case", "char", "continue", "default", "do", "double", "enum", "extern", "float", "for", "goto","if", "else", "int", "long", "register", "return", "short", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "while"};
int keyword_tokens[KEYWORDS_MAX] = { Auto_TOK, Break_TOK, Case_TOK, Char_TOK, Continue_TOK, Default_TOK, Do_TOK, DOUBLE_TOK, Enum_TOK, Extern_TOK, Float_TOK, For_TOK, Goto_TOK, If_TOK, Else_TOK, Int_TOK, Long_TOK, Register_TOK, Return_TOK, Short_TOK, Sizeof_TOK, Static_TOK, Struct_TOK, Switch_TOK, Typedef_TOK, Union_TOK, Unsigned_TOK, While_TOK};

#pragma region ****************Global variables*********************
//global variables
int line_no = 1; //line number in current input file
char lexeme[MAX_LEXEME]; // The characters of the token
int next_char; // The next character of input.

#pragma endregion

//global variables
//int line_no = 1; //line number in current input file
//char lexeme[MAX_LEXEME]; // The characters of the token
//int next_char; // The next character of input.

/* 

Procedure with series of if statements and a finite state machine 
to search for the acceptable input tokens of the atto-c language.
Recieves stdin from sourcefile and returns the token type of the 
accepted characters in the input string.

*/
int lexer()
{
	int state;   // The current state of the FSM.
	int lex_spot; // Current spot in lexeme.
	int token_type;  // The type of token found.

					 // Infinite loop, doing one token at a time.
	
	while (1)
	{  // Initialize the Finite State Machine.
		state = START;
		lex_spot = 0;
		// Loop over characters of the token.
		while (state != FINAL)
		{
			if (next_char == 0)
				next_char = getc(sourcefile);  // get one character from standard source file
			if (next_char == EOF) // EOF is special character for End-Of-File
				return EOF_TOK;    // exit the program with eof token
			switch (state) //Beginning of finite state machine
			{
			case START:
				if (next_char == '\n')
				{
					line_no++;
					next_char = 0;
				}
				else if (isdigit(next_char))
				{
					state = BEFORE;
					lexeme[lex_spot++] = next_char;  // Add the character to the lexeme
					next_char = 0;  // eat the character
				}
				else if (next_char == ' ')
				{
					state = FINAL;
					lexeme[lex_spot] = 0;
					next_char = 0;
				}
				else if (next_char == ',') //comma token
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT Comma %s\n", lexeme);  // This is a final state
					token_type = Comma_TOK;
					next_char = 0;
					return token_type;
				}
				else if (next_char == '-') //minus token
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT Minus %s\n", lexeme);  // This is a final state
					token_type = Minus_TOK;
					next_char = 0;
					return token_type;
				}
				else if (next_char == '!')
				{
					state = NOT_EQUAL;
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}
				else if (next_char == '{')
				{
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					token_type = LeftCurly_TOK;
					if (print_flag)
						printf("ACCEPT Left Curly Brace %s\n", lexeme);
					next_char = 0;
					state = FINAL;
					return token_type;
				}
				else if (next_char == '}')
				{
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT Right Curly Bracket %s\n", lexeme);  // This is a final state
					token_type = RightCurly_TOK;
					next_char = 0;
					state = FINAL;
					return token_type;
				}
				else if (next_char == '*') //asterisk token
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT Asterisk %s\n", lexeme);  // This is a final state
					token_type = Asterisk_TOK;
					next_char = 0;
					return token_type;
				}
				else if (next_char == '+') //plus token
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT Plus %s\n", lexeme);  // This is a final state
					token_type = Plus_TOK;
					next_char = 0;
					return token_type;
				}
				else if (next_char == '<') //less than token
				{
					state = LESS_THAN;
					lexeme[lex_spot++] = next_char;
					next_char = 0;

				}
				else if (next_char == '>') //greater than token
				{
					state = GREATER_THAN;
					lexeme[lex_spot++] = next_char;
					next_char = 0;

				}
				else if (next_char == '=')
				{
					state = EQUAL_TO;
					lexeme[lex_spot++] = next_char;
					next_char = 0;

				}
				else if (next_char == '&')
				{
					state = AND_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}
				else if (next_char == '|')
				{
					state = OR_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = 0;

				}
				else if (next_char == '(') //LeftParenthesis token
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT LEFT PARENTHESES %s\n", lexeme);  // This is a final state
					token_type = LeftParenth_TOK;
					next_char = 0;
					return token_type;
				}
				else if (next_char == ')') //RightParenthesis token
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT RIGHT_PARENTHESIS %s\n", lexeme);  // This is a final state
					token_type = RightParenth_TOK;
					next_char = 0;
					return token_type;
				}
				else if (next_char == ';') //semicolon token
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT SemiColon %s\n", lexeme);  //This is a final state
					token_type = SemiColon_TOK;
					next_char = 0;
					return token_type;
				}
				else if (next_char == '_')
				{
					state = IDENT;
					lexeme[lex_spot++] = next_char;	//adds the character to the lexeme
					next_char = 0;	//eats character
				}
				else if (next_char == '"')
				{
					state = QUOTE;
					next_char = 0;
				}
				else if (next_char == '/')
				{
					state = COMMENT;
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}
				else if (isalpha(next_char))
				{
					state = IDENT;
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}

				else
				{
					line_no++;
					printf("REJECT %c\n", next_char);  // This is not a legal final state
					state = FINAL;  // but we want to end the token anyway
					next_char = 0;   // eat the offending character
				}
				break;  // Need "break" at the end of a case, else you will continue to the next case.

			//Handles next character in integer token
			case BEFORE:
				if (isdigit(next_char))
				{
					state = BEFORE;
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}
				else if (next_char == '.')
				{
					state = AFTER;
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}
				else
				{
					lexeme[lex_spot] = 0; // null for end of string
					token_type = INTEGER_TOK;
					if (print_flag)
						printf("ACCEPT INTEGER %s\n", lexeme);  // This is a final state
					state = FINAL;       // leave next_char alone, for next token
					return token_type;
				}
				break;

			case NOT_EQUAL:
				if (next_char == '=')
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT NotEqual %s\n", lexeme);  // This is a final state
					token_type = NotEqual_TOK;
					next_char = 0;
					return token_type;
				}
				else
				{
					lexeme[lex_spot] = 0;	//null for end of string
					token_type = Negation_TOK;
					if (print_flag)
						printf("ACCEPT NEGATION %s\n", lexeme);  //This is a final state
					state = FINAL;       //leave next_char alone, for next token
					return token_type;
				}
				break;
			case LESS_THAN:
				if (next_char == '=')
				{
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT LessEqual %s\n", lexeme);  // This is a final state
					token_type = LessEqual_TOK;
					next_char = 0;
					return token_type;
				}
				else
				{
					lexeme[lex_spot] = 0; // null for end of string
					token_type = LessThan_TOK;
					if (print_flag)
						printf("ACCEPT LessThan %s\n", lexeme);  // This is a final state
					state = FINAL;       // leave next_char alone, for next token
					return token_type;
				}
				break;
			case GREATER_THAN:
				if (next_char == '=')
				{
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT GreaterEqual %s\n", lexeme);  // This is a final state
					token_type = GreaterEqual_TOK;
					next_char = 0;
					return token_type;
					state = FINAL;
				}
				else
				{
					lexeme[lex_spot] = 0; // null for end of string
					token_type = GreaterThan_TOK;
					if (print_flag)
						printf("ACCEPT GreaterThan %s\n", lexeme);  // This is a final state
					return token_type;
					state = FINAL;       // leave next_char alone, for next token
				}
				break;
			case EQUAL_TO:
				if (next_char == '=')
				{
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT EQUAL %s\n", lexeme);  // This is a final state
					token_type = Equal_TOK;
					next_char = 0;
					state = FINAL;
					return token_type;
				}
				else
				{
					lexeme[lex_spot] = 0; // null for end of string
					token_type = Assign_TOK;
					if (print_flag)
						printf("ACCEPT Assign %s\n", lexeme);  // This is a final state
					state = FINAL;       // leave next_char alone, for next token
					return token_type;
				}
				break;
			case AND_STATE:
				if (next_char == '&')
				{
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT AND %s\n", lexeme);  // This is a final state
					token_type = And_TOK;
					next_char = 0;
					state = FINAL;
					return token_type;
				}
				else
				{
					line_no++;
					printf("REJECT %c\n", next_char);  // This is not a legal final state
					state = FINAL;  // but we want to end the token anyway
					next_char = 0;   // eat the offending character
				}
				break;
			case OR_STATE:
				if (next_char == '|')
				{
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT OR %s\n", lexeme);  // This is a final state
					token_type = Or_TOK;
					next_char = 0;
					state = FINAL;
					return token_type;
				}
				else
				{
					line_no++;
					printf("REJECT %c\n", next_char);  // This is not a legal final state
					state = FINAL;  // but we want to end the token anyway
					next_char = 0;   // eat the offending character
				}
				break;
			case QUOTE:
				if (lex_spot == MAX_LEXEME - 3)
				{
					printf("ERROR: quoted string too long. Truncating. Line %d\n", line_no);
					lexeme[lex_spot++] = next_char;
				}
				else if (next_char == '\\')
				{
					state = BACKSLASH_IN_QUOTE;
					next_char = 0;
				}
				else if (next_char == '"')
				{
					lexeme[lex_spot] = 0; // null for end of string
					token_type = Quote_TOK;
					if (print_flag)
						printf("ACCEPT Quote %s\n", lexeme);  // This is a final state
					state = FINAL;
					next_char = 0;
					return token_type;
				}
				else if (lex_spot < MAX_LEXEME - 3)
				{
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}
				break;
			case COMMENT:
				if (next_char == '*')
				{
					lexeme[lex_spot++] = next_char;
					next_char = 0;  // eat the character
					state = COMMENT_LONG;

				}
				else if (next_char == '/')
				{
					lexeme[lex_spot++] = next_char;  // Add the character to the lexeme
					next_char = 0;  // eat the character
					state = COMMENT_SHORT;
				}
				else
				{
					lexeme[lex_spot] = 0;
					token_type = Division_TOK;
					if (print_flag)
						printf("ACCEPT Division %s\n", lexeme);  //This is a final state
					state = FINAL; //leave next_char alone for next token
					return token_type;
				}
				break;
			case COMMENT_SHORT:
				if (next_char == '\n')
				{
					line_no++;
					lexeme[lex_spot] = 0;
					if (print_flag)
						printf("ACCEPT Short Comment \n");
					next_char = 0;
					token_type = CommentShort_TOK;
					state = FINAL;
					break;
				}
				else
				{
					next_char = 0;
					state = COMMENT_SHORT;
				}
				break;
			case COMMENT_LONG:
				if (next_char == '\n')
				{
					line_no++;
					next_char = 0;
				}
				else if (next_char == '/')
				{
					lexeme[lex_spot++] = next_char;
					next_char = 0;
					lexeme[lex_spot] = 0; // null for end of string
					token_type = CommentLong_TOK;
					if (print_flag)
						printf("ACCEPT LONG Comment \n");  // This is a final state
					state = FINAL;
				}
				else
				{
					next_char = 0; //eats character
					state = COMMENT_LONG;
				}
				break;
			case BACKSLASH_IN_QUOTE:
				if (next_char == 'n')
				{
					lexeme[lex_spot++] = '\n';
					next_char = 0;
					state = QUOTE;       // leave next_char alone, for next token
				}
				else if (next_char == '"')
				{
					lexeme[lex_spot++] = next_char;
					next_char = 0;
					state = QUOTE;       // leave next_char alone, for next token
				}
				else if (next_char == 'b')
				{
					lexeme[lex_spot++] = '\b';
					next_char = 0;
					state = QUOTE;       // leave next_char alone, for next token
				}
				else if (next_char == 't')
				{
					lexeme[lex_spot++] = '\t';
					next_char = 0;
					state = QUOTE;       // leave next_char alone, for next token
				}
				else if (next_char == '\\')
				{
					lexeme[lex_spot++] = next_char;
					next_char = 0;
					state = QUOTE;       // leave next_char alone, for next token
				}
				break;
			case AFTER:
				if (isdigit(next_char))
				{
					state = AFTER;
					lexeme[lex_spot++] = next_char;
					next_char = 0;
				}
				else
				{
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0; // null for end of string
					token_type = DECIMAL_FRACTION_TOK;
					if (print_flag)
						printf("ACCEPT DECIMAL_FRACTION %s\n", lexeme);  // This is a final state
					state = FINAL;
					return token_type;
				}
				if (lex_spot == NUMBER_MAX_LENGTH)
				{
					printf("ERROR: number too long. Truncating. Line %d\n", line_no);
					lexeme[lex_spot++] = 0;
				}
				else if (lex_spot < NUMBER_MAX_LENGTH)
				{
					lexeme[lex_spot++] = next_char;
				}
				next_char = 0;
				break;
			case IDENT:
				if (lex_spot == IDENT_MAX_LENGTH)
				{
					printf("ERROR: identifier too long. Truncating. Line %d\n", line_no);
					lexeme[lex_spot++] = 0;
				}
				else if (lex_spot < IDENT_MAX_LENGTH)
				{
					if (isalpha(next_char) || isdigit(next_char) || next_char == '_')
					{
						lexeme[lex_spot++] = next_char;
						next_char = 0;
						break;
					}
					else {
						lexeme[lex_spot] = 0;
					}
				}
				token_type = IDENT_TOK;
				int k;
				for (k = 0; k < KEYWORDS_MAX; k+=4)
				{
					if (strcmp(lexeme, keywords[k]) == 0)
					{
						token_type = keyword_tokens[k];
						state = FINAL;
						break;
					}
					k++;
					if (strcmp(lexeme, keywords[k]) == 0)
					{
						token_type = keyword_tokens[k];
						state = FINAL;
						break;
					}
					k++;
					if (strcmp(lexeme, keywords[k]) == 0)
					{
						token_type = keyword_tokens[k];
						state = FINAL;
						break;
					}
					k++;
					if (strcmp(lexeme, keywords[k]) == 0)
					{
						token_type = keyword_tokens[k];
						state = FINAL;
						break;
					}
				}
				if (print_flag)
					printf("ACCEPT IDENTIFIER %s\n", lexeme);
				state = FINAL;
				return token_type;
				break;
			} // end of switch
		} // end of while state
	}  // end of infinite loop
} // end of main