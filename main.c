/*
Author: Terrence MOore
Date : May 12, 2017

main.c

Purpose: 
Serves as the driving function of the atto-c compiler.
Calls all the necessary functions to operate compiler.

*/
#include "atto-c.h"
//Used inorder to use source file as stdin and manipulate assembly files
FILE *sourcefile;
FILE *asmfile;

// variable handling print statements for debugging
int print_flag;

/*

Function that calls other functions of compiler and handles how the compiler reads
a source file from the command line. Incoming arguments int argc, char **argv.

*/
main(int argc, char **argv)
{
	while ((argv[1] != NULL) && (argv[1][0] == '-'))
	{
		switch (argv[1][1])
		{
		case 'd': print_flag = 1; break;
		default: printf("Unknown option: %c\n", argv[1][1]);
			exit(9);
		}
		argc--;  // reduce the argument count
		argv++;  // move the argument pointer one word over
	}
	if (argv[1] == NULL)
	{
		printf("Missing source file on command line.\n");
		exit(99);
	}
	sourcefile = fopen(argv[1], "r"); //"r" is for read mode. Used to read text ing sourcefile.

	if (sourcefile == NULL)
	{
		perror(argv[1]);
		exit(1);
	}

	char asmfilename[100];
	strcpy(asmfilename, argv[1], 95);
	int length;
	length = strlen(asmfilename);
	strcpy(asmfilename + length - 2, ".asm");
	asmfile = fopen(asmfilename, "w");
	if (asmfile == NULL)
	{
		perror(asmfilename);
		exit(9);
	}
	preamble(argv[1]);
	program();
	postamble();
}