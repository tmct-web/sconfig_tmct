#include <stdio.h>
#include "user.h"

#include "fs.h"


/********************************************************************************/
/*	Name:			fs_open  													*/
/*																				*/
/*	Descriptions:	Open programming file										*/
/********************************************************************************/
int fs_open( char argv[], char* mode, int *file_id )
{
	FILE* fid;
	fid = fopen( argv, mode );
	if ( fid == NULL )
	{
		fprintf( stderr, "Error: Could not open file: \"%s\"!\n", argv );
		return CB_FS_OPEN_FILE_ERROR;
	}
	else
	{
		*file_id = (int) fid;
		fprintf( stdout, "Info: Programming file: \"%s\" opened.\n", argv );
		return CB_OK;
	}
}



/********************************************************************************/
/*	Name:			fs_close  													*/
/*																				*/
/*	Descriptions:	Close file													*/
/********************************************************************************/
int	fs_close( int file_id )
{
	int status = 0;
	status = fclose( (FILE*) file_id);
	if ( status )
	{
		fprintf( stderr, "Error: Could not close file!\n");
		return CB_FS_CLOSE_FILE_ERROR;
	}
	else
	{
		return CB_OK;
	}
}



/********************************************************************************/
/*	Name:			fs_size		  												*/
/*																				*/
/*	Descriptions:	check file size												*/
/********************************************************************************/
int fs_size( int file_id, long int *size )
{
	int status = 0;
	status = fseek( (FILE*) file_id, 0, S_END );
	if ( status )
	{
		fprintf( stderr, "Error: End of file could not be located!" );
		return CB_FS_SIZE_EOF_NOT_FOUND;
	}
	else
	{
		*size = ftell( (FILE*) file_id );
		fseek( (FILE*) file_id, 0, S_SET );
		fprintf( stdout, "Info: File size: %ld bytes.\n", *size );
		return CB_OK;
	}
}



/********************************************************************************/
/*	Name:			fs_read		  												*/
/*																				*/
/*	Descriptions:	read a byte from file										*/
/********************************************************************************/
int fs_read( int file_id, int *data )
{
	int status = 0;
	status = fgetc( (FILE*) file_id );
	if ( status == EOF )
	{
		fprintf( stderr, "Error: Could not read data from file!" );
		return CB_FS_READ_ERROR;
	}
	else
	{
		*data = status;
		return CB_OK;
	}
}



/********************************************************************************/
/*	Name:			fs_write	  												*/
/*																				*/
/*	Descriptions:	write a byte to file										*/
/********************************************************************************/
void fs_write(int file_id, int data)  
{
	fputc(data, (FILE*) file_id );
		
}



/********************************************************************************/
/*	Name:			fs_rewind	  												*/
/*																				*/
/*	Descriptions:	Repositions the file pointer to the beginning of a file		*/
/********************************************************************************/
void fs_rewind( int file_id)  
{
	rewind( (FILE*) file_id );	
}

