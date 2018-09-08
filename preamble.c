#include "atto-c.h"

/*
Author: Terrence MOore
Date : May 13, 2017

preamble.c

Purpose:
Begins and ends assembly file that is written with the purpose of turning the source code in source file into code that
is to be assembled and ran.

*/

//receives source name and applies the initial headings for the assembly file 
void preamble(char *sourcefilename)
{
	fprintf(asmfile, "	TITLE %s\n", sourcefilename);
	fprintf(asmfile, "	.686P\n");
	fprintf(asmfile, "	.model	flat\n\n");
	fprintf(asmfile, "INCLUDELIB MSVCRTD\n");
	fprintf(asmfile, "INCLUDELIB OLDNAMES\n");
}

//handles ending the assembly file once main parser and expressions parser  finish processsing source file into assembly.
void postamble()
{
	output_strings();
	fprintf(asmfile, "END\n");
	fclose(asmfile);
}
/****************************************************************************
* Procedure: output_strings()
* Purpose: write out all the strings to the CONST segment.
*/
void output_strings()
{
	int state; // for inquote/outquote finite state machine.
	char *c;  // for traversing the string one character at a time
	int n;  // string index

	fprintf(asmfile, "CONST  SEGMENT\n");
	for (n = 0; n < string_serial; n++)
	{
		fprintf(asmfile, "string@%d DB ", n);
		// Now a little finite state machine to handle unprintable characters.
#define INQUOTE 1
#define OUTQUOTE 2
		state = OUTQUOTE;
		for (c = strings[n]; *c != 0; c++)
		{
			if (isprint(*c) && (*c != '\''))
			{
				if (state == OUTQUOTE)
				{
					fprintf(asmfile, "'%c", *c);
					state = INQUOTE;
				}
				else  // already in quote
					fprintf(asmfile, "%c", *c);
			}
			else // nonprintable, so do hex
			{
				if (state == INQUOTE)
				{
					fprintf(asmfile, "',%02xH", *c);
					state = OUTQUOTE;
				}
				else // already out of quote
					fprintf(asmfile, ",%02xH", *c);
			}
		}
		fprintf(asmfile, ",00H\n"); // terminating null
	}
	fprintf(asmfile, "CONST   ENDS\n");

} // end output_strings()