/****************************************************************************/
/*																			*/
/*	Module:			main.c	(SRunner)										*/
/*																			*/
/*					Copyright (C) Altera Corporation 2004					*/
/*																			*/
/*	Descriptions:	Main source file that manages SRunner-User interface	*/
/*					to execute program or read routine in SRunner			*/
/*																			*/
/*	Revisions:		1.0	09/30/04 Khai Liang Aw								*/
/*					1.1	06/05/05 Khai Liang Aw - EPCS64 Support				*/
/*					1.2	06/11/08 Noor Hazlina Ramly - EPCS128 Support		*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "user.h"
#include "as.h"

/* Version Number */
const char VERSION[4] = "1.0";
const char SRCVERSION[4] = "1.2";

/********************************************************************************/
/*	Name:			Main          												*/
/*																				*/
/*	Parameters:		int argc, char* argv[]										*/
/*					- number of argument.										*/
/*					- argument character pointer.								*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Open programming file and initialize driver if required		*/
/*					(WINDOWS NT).												*/
/*																				*/
/********************************************************************************/
int main( int argc, char** argv)
{
	int status = 0;
	int epcsDensity = 0;

	/* Introduction */
	fprintf(stdout,"\n=======================================================\n");
	fprintf(stdout," Mi-ke neko Tools :: SCONFIG Version %s\n", VERSION);
	fprintf(stdout," Copyright (c) 2015 Mi-ke neko Tools.\n");
	fprintf(stdout," Based on SRunner(Windows) Version %s\n", SRCVERSION);
	fprintf(stdout," Copyright (c) 1995-2015 Altera Corporation.\n");
	fprintf(stdout,"=======================================================\n");

	if(argc != 4)	//Syntax check
	{
		fprintf( stdout, " \nError: Invalid Number of Arguments\n");
		fprintf( stdout, " \n Command\t\t\t\t\t\tDescription\n");
		fprintf( stdout, " ============\t\t\t\t\t\t===============\n");
		fprintf( stdout, " sconfig -program -<EPCS density in Mb> <file.rpd>\t=> Program EPCS\n");
		fprintf( stdout, " sconfig -read -<EPCS density in Mb> <file.rpd>\t\t=> Read EPCS data to file\n");
		fprintf( stdout, " sconfig -verify -<EPCS density in Mb> <file.rpd>\t=> Verify EPCS data with file\n");
		fprintf( stdout, " sconfig -erase -<EPCS density in Mb> \"\"\t\t=> Erase EPCS data\n");
		fprintf( stdout, "\nExample of command -> srunner -program -64 Mydesign.rpd <-\n");

		status = CB_INVALID_NUMBER_OF_ARGUMENTS;
	}
	else if ( argv[1][1] != 'p' && argv[1][1] != 'r' && argv[1][1] != 'v' && argv[1][1] != 'e')
	{

		fprintf( stdout, " \nError: Invalid Command\n");
		fprintf( stdout, " \n Command\t\t\t\t\t\tDescription\n");
		fprintf( stdout, " ============\t\t\t\t\t\t===============\n");
		fprintf( stdout, " sconfig -program -<EPCS density in Mb> <file.rpd>\t=> Program EPCS\n");
		fprintf( stdout, " sconfig -read -<EPCS density in Mb> <file.rpd>\t\t=> Read EPCS data to file\n");
		fprintf( stdout, " sconfig -verify -<EPCS density in Mb> <file.rpd>\t=> Verify EPCS data with file\n");
		fprintf( stdout, " sconfig -erase -<EPCS density in Mb> \"\"\t\t=> Erase EPCS data\n");
		fprintf( stdout, "\nExample of command -> srunner -program -64 Mydesign.rpd <-\n");

		status = CB_INVALID_COMMAND;

	}
	else if	((strcmp(&argv[2][1],"1")!=0) && (strcmp(&argv[2][1],"4")!=0) && (strcmp(&argv[2][1],"16")!=0) && (strcmp(&argv[2][1],"64")!=0) && (strcmp(&argv[2][1],"128")!=0))
	{
			fprintf( stdout, "\nError: Invalid EPCS density\n");
			fprintf( stdout, "\nValid choices are 1, 4, 16, 64, or 128\n");
			fprintf( stdout, "\nExample of command -> srunner -program -64 Mydesign.rpd <-\n");

			status = CB_INVALID_EPCS_DENSITY;
	}
	else
	{
		epcsDensity = atoi(&argv[2][1]);
		if	( argv[1][1] == 'p')
		{
		fprintf( stdout, "\nOperation: Programming EPCS\n");
		status = as_program( &argv[3][0], epcsDensity );	//Execute programming function

		}
		else if ( argv[1][1] == 'r')
		{
		fprintf( stdout, "\nOperation: Reading EPCS Data\n");
		status = as_read( &argv[3][0], epcsDensity );		//Execute reading function
		}
		else if ( argv[1][1] == 'v')
		{
		fprintf( stdout, "\nOperation: Verifying EPCS Data\n");
		status = as_ver( &argv[3][0], epcsDensity );		//Execute verify function
		}
		else if ( argv[1][1] == 'e')
		{
		fprintf( stdout, "\nOperation: Erase EPCS Data\n");
		status = as_erase( epcsDensity );		//Execute erase fu	nction
		}
	}

	if(status != CB_OK)
	   fprintf( stdout, "\nError code: %d\n\n", status );
	else
	   fprintf( stdout, "\nOperation Completed!!!\n\n" );

	return status;

}
