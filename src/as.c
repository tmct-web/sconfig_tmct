#include <stdio.h>
#include "user.h"
#include "as.h"
#include "fs.h"
#include "bb_gpio.h"


int EPCS_device = 0;
int RPD_file_size = 0;


/********************************************************************************/
/*	Name:			as_program  												*/
/*																				*/
/*	Parameters:		FILE* finputid												*/
/*					- programming file pointer.									*/
/*																				*/
/*	Return Value:	Error Code													*/
/*																				*/
/*	Descriptions:	Get programming file size, parse through every single byte	*/
/*					and dump to parallel port.									*/
/*																				*/
/*					FPGA access to the EPCS is disable when the programming 	*/
/*					starts.														*/
/*																				*/
/*																				*/
/********************************************************************************/
int as_program( char *file_path, int epcsDensity )
{
	int status = 0;
	int file_id = 0;
	long int file_size = 0;


	/* Open RPD file for programming */
	status = as_open( file_path, &file_id, &file_size );
	if ( status != CB_OK )
		return status;


	/* Disable FPGA access to EPCS */
	status = as_program_start();
	if ( status != CB_OK )
		return status;


	/* Read EPCS silicon ID */
	status = as_silicon_id(file_size, epcsDensity);
	if ( status != CB_OK )
		return status;



	/* EPCS Bulk Erase */
	status = as_bulk_erase( );
	if ( status != CB_OK )
		return status;


	/* Start EPCS Programming */
	status = as_prog( file_id, file_size );
	if ( status != CB_OK )
		return status;


	/* Start EPCS Verifying */
	//status = as_verify( file_id, file_size );
	//if ( status != CB_OK )
	//	return status;



	/* Enable FPGA access to EPCS */
	status = as_program_done();
	if ( status != CB_OK )
		return status;


	status = as_close( file_id );
	if ( status != CB_OK )
		return status;



	return CB_OK;

}

/********************************************************************************/
/*	Name:			as_ver  													*/
/*																				*/
/*	Parameters:		FILE* finputid												*/
/*					- programming file pointer.									*/
/*																				*/
/*	Return Value:	Error Code													*/
/*																				*/
/*	Descriptions:	Verify EPCS data											*/
/*																				*/
/*																				*/
/*					FPGA access to the EPCS is disable when the programming 	*/
/*					starts.														*/
/*																				*/
/*																				*/
/********************************************************************************/
int as_ver( char *file_path, int epcsDensity)
{
	int status = 0;
	int file_id = 0;
	long int file_size = 0;



	/* Open RPD file for verify */
	status = as_open( file_path, &file_id, &file_size );
	if ( status != CB_OK )
		return status;


	/* Disable FPGA access to EPCS */
	status = as_program_start();
	if ( status != CB_OK )
		return status;


	/* Read EPCS silicon ID */
	status = as_silicon_id(file_size, epcsDensity);
	if ( status != CB_OK )
		return status;


	/* Start EPCS Verifying */
	status = as_verify( file_id, file_size );
	if ( status != CB_OK )
		return status;



	/* Enable FPGA access to EPCS */
	status = as_program_done();
	if ( status != CB_OK )
		return status;


	status = as_close( file_id );
	if ( status != CB_OK )
		return status;



	return CB_OK;

}

/********************************************************************************/
/*	Name:			as_read     												*/
/*																				*/
/*	Parameters:		FILE* finputid												*/
/*					- programming file pointer.									*/
/*																				*/
/*	Return Value:	Error Code													*/
/*																				*/
/*	Descriptions:	Get EPCS data and save in a file							*/
/*																				*/
/*																				*/
/*					FPGA access to the EPCS is disable when the reading     	*/
/*					starts.														*/
/*																				*/
/*																				*/
/********************************************************************************/
int as_read( char *file_path, int epcsDensity )
{
	int status = 0;
	int file_id = 0;
	long int file_size = 0;


	status = bb_open();
	if ( status != CB_OK )
		return status;


	/* Open RPD file for to store EPCS data */
	status = fs_open( file_path, "w+b", &file_id );
	if ( status != CB_OK )
		return status;


	/* Disable FPGA access to EPCS */
	status = as_program_start();
	if ( status != CB_OK )
		return status;


	/* Read EPCS silicon ID */
	status = as_silicon_id(DEV_READBACK, epcsDensity);
	if ( status != CB_OK )
		return status;



	/* Start EPCS Readback */
	status = as_readback( file_id);
	if ( status != CB_OK )
		return status;



	/* Enable FPGA access to EPCS */
	status = as_program_done();
	if ( status != CB_OK )
		return status;


	status = as_close( file_id );
	if ( status != CB_OK )
		return status;

	return CB_OK;

}


int as_erase( int epcsDensity )
{
	int status = 0;


	status = bb_open();
	if ( status != CB_OK )
		return status;


	/* Disable FPGA access to EPCS */
	status = as_program_start();
	if ( status != CB_OK )
		return status;


	/* Read EPCS silicon ID */
	status = as_silicon_id(DEV_READBACK, epcsDensity);
	if ( status != CB_OK )
		return status;

	/* EPCS Bulk Erase */
	status = as_bulk_erase( );
	if ( status != CB_OK )
		return status;


	/* Enable FPGA access to EPCS */
	status = as_program_done();
	if ( status != CB_OK )
		return status;

	bb_close();

	return CB_OK;

}


int as_program_start(void)
{
	int status = 0;

	// Drive NCONFIG to reset FPGA before programming EPCS
	status = bb_write( NCONFIG, 0 );
	if ( status != CB_OK )
		return status;

	// Drive NCE to disable FPGA from accessing EPCS
	status = bb_write( NCE, 1 );
	if ( status != CB_OK )
		return status;

	// Drive NCS to high when not acessing EPCS
	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;

	return CB_OK;
}



int as_program_done(void)
{
	int		status;

	// Drive NCE to enable FPGA
	status = bb_write( NCE, 0 );
	if ( status != CB_OK )
		return status;

	// Drive NCONFIG from low to high to reset FPGA
	status = bb_write( NCONFIG, 1 );
	if ( status != CB_OK )
		return status;

	// Drive NCS to high when not acessing EPCS
	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;


	return CB_OK;
}


int as_open( char *file_path, int *file_id, long int *file_size )
{
	int status = 0;

	status = fs_open( file_path, "rb", file_id );
	if ( status != CB_OK )
		return status;

	status = bb_open();
	if ( status != CB_OK )
		return status;

	status = fs_size( *file_id, file_size );
	if ( status != CB_OK )
		return status;

	return CB_OK;
}


int as_close( int file_id )
{
	int	status = 0;

	bb_close();
	//status = bb_close();
	//if ( status != CB_OK )
	//	return status;

	status = fs_close( file_id );
	if ( status != CB_OK )
		return status;

	return CB_OK;
}





/********************************************************************************/
/*	Name:			as_prog														*/
/*																				*/
/*	Parameters:		int file_size					 							*/
/*					- file size to check for the correct programming file.		*/
/*					int file_id					 								*/
/*					- to refer to the RPD file.									*/
/*																				*/
/*	Return Value:	status.														*/
/*																				*/
/*	Descriptions:	program the data in the EPCS								*/
/*																				*/
/********************************************************************************/
int as_prog( int file_id, int file_size )
{
	int			page = 0;
	int         one_byte = 0;
	int         EPCS_Address =0;
	int         StatusReg =0;
	int			i,j;
	int			status = 0;
	int         bal_byte = 0;
	int         byte_per_page = 256;

	fprintf( stdout, "\nInfo: Start programming process.\n" );
	page = file_size/256;

	bal_byte = file_size%256;

	if(bal_byte) //if there is balance after divide, program the balance in the next page
	{
		page++;
	}


	//=========== Page Program command Start=========//
	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;

	status = as_program_byte_msb( AS_WRITE_ENABLE );
	if ( status != CB_OK )
		return status;

	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;


	// page program
	fprintf( stdout, "\nInfo: Programming. Please wait few minutes...\n");

	for(i=0; i<page; i++ )
	{

		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb( AS_WRITE_ENABLE );
		if ( status != CB_OK )
			return status;

		status = bb_write( NCS, 1 );
		if ( status != CB_OK )
			return status;

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;


		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb( AS_PAGE_PROGRAM );
		if ( status != CB_OK )
			return status;

		EPCS_Address = i*256;

		status = as_program_byte_msb( ((EPCS_Address & 0xFF0000)>>16));
		status = as_program_byte_msb( ((EPCS_Address & 0x00FF00)>>8) );
		status = as_program_byte_msb( EPCS_Address & 0xFF);

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

		if((i == (page - 1)) && (bal_byte != 0))	//if the last page has has been truncated less than 256
			byte_per_page = bal_byte;

		for(j=0; j<byte_per_page; j++)
		{

			// read one byte
			status = fs_read( file_id, &one_byte );
			if ( status != CB_OK )
				return status;


			// Progaram a byte
			status = as_program_byte_lsb( one_byte );
			if ( status != CB_OK )
				return status;
		}

		status = bb_write( NCS, 1 );
		if ( status != CB_OK )
			return status;

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

		//Program in proress
		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb( AS_READ_STATUS );
		if ( status != CB_OK )
			return status;

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;


		status = as_read_byte_msb(&StatusReg);
		if ( status != CB_OK )
			return status;


		while((StatusReg & 0x01))
		{
			status = as_read_byte_msb(&StatusReg);
			if ( status != CB_OK )
				return status;
		}


		status = bb_write( NCS, 1 );
		if ( status != CB_OK )
			return status;

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;
		//Program End

	}

	fprintf( stdout, "Info: Programming successful.\n" );

	//=========== Page Program command End==========//


	return CB_OK;
}





/********************************************************************************/
/*	Name:			as_bulk_erase												*/
/*																				*/
/*	Parameters:		int file_size					 							*/
/*					- file size to check for the correct programming file.		*/
/*					int file_id					 								*/
/*					- to refer to the RPD file.									*/
/*																				*/
/*	Return Value:	status.														*/
/*																				*/
/*	Descriptions:	program the data in the EPCS								*/
/*																				*/
/********************************************************************************/
int as_bulk_erase( void )
{
	int status =0;
	int StatusReg =0;

	//=========== Bulk erase command Start ===========//
	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;

	status = as_program_byte_msb( AS_WRITE_ENABLE );
	if ( status != CB_OK )
		return status;

	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;


	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;

	status = as_program_byte_msb( AS_ERASE_BULK );
	if ( status != CB_OK )
		return status;

	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;


	status = bb_write( NCS, 0 );
	if ( status != CB_OK )
		return status;

	status = as_program_byte_msb( AS_READ_STATUS );
	if ( status != CB_OK )
		return status;

	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;

	//Erase in proress
	fprintf( stdout, "Info: Erasing. Please wait few minutes...\n" );

	status = as_read_byte_msb(&StatusReg);
	if ( status != CB_OK )
		return status;


	while((StatusReg & 0x01))	//Keep on polling if the WIP is high
	{
		status = as_read_byte_msb(&StatusReg);
		if ( status != CB_OK )
			return status;
	}

	fprintf( stdout, "Info: Erase Done." );
	//Erase End

	status = bb_write( NCS, 1 );
	if ( status != CB_OK )
		return status;

	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;

	//=========== Bulk erase command End ============//
	return CB_OK;

}


/********************************************************************************/
/*	Name:			as_readback													*/
/*																				*/
/*	Parameters:		none							 							*/
/*																				*/
/*																				*/
/*																				*/
/*																				*/
/*	Return Value:	status.														*/
/*																				*/
/*	Descriptions:	read the content of the EPCS devices and store in RPD file	*/
/*																				*/
/********************************************************************************/
int as_readback( int file_id )
{
	//=========== Readback Program command Start=========//
	int		status;
	int     i;
	int     read_byte;


		fprintf( stdout, "Info: Reading. Please wait few minutes...\n" );

		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb( AS_READ_BYTES );
		if ( status != CB_OK )
			return status;


		status = as_program_byte_msb(0x00);
		status = as_program_byte_msb(0x00);
		status = as_program_byte_msb(0x00);


		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

		for(i=0; i<RPD_file_size; i++)
		{
			status = as_read_byte_lsb(&read_byte);
			if ( status != CB_OK )
				return status;

			fs_write(file_id, read_byte);

		}

		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

		fprintf( stdout, "Info: Read successful.\n" );
	//=========== Readback Program command End==========//

	return CB_OK;
}



/********************************************************************************/
/*	Name:			as_verify													*/
/*																				*/
/*	Parameters:		int file_size					 							*/
/*					- file size to check for the correct programming file.		*/
/*					int file_id					 								*/
/*					- to refer to the RPD file.									*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	verify the all the programmed data matach the data in the	*/
/*					data in RPD file.											*/
/*																				*/
/********************************************************************************/
int as_verify( int file_id, int file_size )
{
	//=========== Readback Program command Start=========//
	int		status;
	int     i;
	int     read_byte =0;
	int     one_byte = 0;



		fprintf( stdout, "Info: Verifying. Please wait few minutes...\n" );

		fs_rewind(file_id);	//reposition the file pointer of the RPD file


		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb( AS_READ_BYTES );
		if ( status != CB_OK )
			return status;

		status = as_program_byte_msb(0x00);
		status = as_program_byte_msb(0x00);
		status = as_program_byte_msb(0x00);

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

		for(i=0; i<file_size; i++)
		{
			// read one byte from the EPCS
			status = as_read_byte_lsb(&read_byte);
			if ( status != CB_OK )
				return status;


			// read one byte from RPD file
			status = fs_read( file_id, &one_byte );
			if ( status != CB_OK )
				return status;

			if(one_byte != read_byte)
			{
				status = CB_AS_VERIFY_FAIL;
				return status;
			}

		}

		fprintf( stdout, "Info: Verify completed.\n" );

		status = bb_write( NCS, 1 );
		if ( status != CB_OK )
			return status;

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

	//=========== Readback Program command End==========//

	return CB_OK;
}



/********************************************************************************/
/*	Name:			as_silicon_id												*/
/*																				*/
/*	Parameters:		int file_size					 							*/
/*					- file size to check for the correct programming file.		*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	check silicon id to determine:								*/
/*					EPCS devices.												*/
/*					RPD file size.												*/
/*																				*/
/********************************************************************************/
int as_silicon_id(int file_size, int epcsDensity)
{
	//=========== Read silicon id command Start=========//
	int	status;
	int silicon_ID = 0;

		status = bb_write( NCS, 0 );
		if ( status != CB_OK )
			return status;

		if (epcsDensity != 128)		//for EPCS1, EPCS4, EPCS16, EPCS64
		{
			status = as_program_byte_msb( AS_READ_SILICON_ID );
			if ( status != CB_OK )
				return status;

			status = as_program_byte_msb(0x00);		//3 Dummy bytes
			status = as_program_byte_msb(0x00);
			status = as_program_byte_msb(0x00);
		}
		else						// for EPCS128
		{
			status = as_program_byte_msb( AS_CHECK_SILICON_ID );
			if ( status != CB_OK )
				return status;

			status = as_program_byte_msb(0x00);		//2 Dummy bytes
			status = as_program_byte_msb(0x00);
		}

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

		// read silicon byte from the EPCS
		status = as_read_byte_msb(&silicon_ID);
		if ( status != CB_OK )
			return status;

		// determine the required RPD file size and EPCS devices
		if(silicon_ID == EPCS1_ID)
		{
			EPCS_device = EPCS1;
		}
		else if(silicon_ID == EPCS4_ID)
		{
			EPCS_device = EPCS4;
		}
		else if(silicon_ID == EPCS16_ID)
		{
			EPCS_device = EPCS16;
		}
		else if(silicon_ID == EPCS64_ID)
		{
			EPCS_device = EPCS64;
		}
		else if(silicon_ID == EPCS128_ID)
		{
			EPCS_device = EPCS128;
		}
		else
		{
			fprintf( stdout, "\nError: Unsupported Device");
			status = CB_AS_UNSUPPORTED_DEVICE;
			return status;
		}

		fprintf( stdout, "\nInfo: Silicon ID - 0x%x \n", silicon_ID);
		fprintf( stdout, "Info: Serial Configuration Device - EPCS%d\n", EPCS_device);

		RPD_file_size = EPCS_device * 131072;	//To calculate the maximum file size for the EPCS

		if(file_size > RPD_file_size && file_size != DEV_READBACK)
		{
			fprintf( stdout, "\nError: Wrong programming file");
			return CB_AS_WRONG_RPD_FILE;
		}

		status = bb_write( NCS, 1 );
		if ( status != CB_OK )
			return status;

		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

	//=========== Readback Program command End==========//

	return CB_OK;
}



/********************************************************************************/
/*	Name:			as_program_byte_lsb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to dump.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Dump to parallel port bit by bit, from least significant	*/
/*					bit to most significant bit. A positive edge clock pulse	*/
/*					is also asserted.											*/
/*																				*/
/********************************************************************************/
int as_program_byte_lsb( int one_byte )
{
	int	bit = 0;
	int i = 0;
	int status = 0;

	// write from LSB to MSB
	for ( i = 0; i < 8; i++ )
	{
		bit = one_byte >> i;
		bit = bit & 0x1;

		// Dump to DATA0 and insert a positive edge pulse at the same time
		status = bb_write( DCLK, 0 );
		if ( status != CB_OK )
			return status;
		//status = bb_write( DATA0, bit );
		status = bb_write( ASDI, bit );
		if ( status != CB_OK )
			return status;
		status = bb_write( DCLK, 1 );
		if ( status != CB_OK )
			return status;
	}


	return CB_OK;
}



/********************************************************************************/
/*	Name:			as_program_byte_msb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to dump.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Convert MSB to LSB and Dump to parallel port bit by bit,	*/
/*					from most significant bit to least significant bit.			*/
/*					A positive edge clock pulse	is also asserted.				*/
/*																				*/
/********************************************************************************/
int as_program_byte_msb( int one_byte )
{
	int status = 0;
	int data_byte = 0;

	//Convert MSB to LSB before programming
	as_lsb_to_msb(&one_byte, &data_byte);

	//After conversion, MSB will goes out first
	status = as_program_byte_lsb(data_byte);

	return CB_OK;
}


/********************************************************************************/
/*	Name:			as_read_byte_lsb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to read.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	read to parallel port bit by bit, from least significant    */
/*					bit to most significant bit. A positive edge clock pulse	*/
/*					is also asserted. (read during positive edge)				*/
/*																				*/
/********************************************************************************/
int as_read_byte_lsb( int *one_byte )
{
	int	bit = 0;
	int mask = 0x01;
	int i;
	int status = 0;

	*one_byte = 0;


	// Flush out the remaining data in Port0 before reading
	//status = bb_flush();
	//if ( status != CB_OK )
	//	return status;

	// read from from LSB to MSB
	for ( i = 0; i < 8; i++ )
	{
		// Dump to DATA0 and insert a positive edge pulse at the same time
		status = bb_write( DCLK, 0 );
		if ( status != CB_OK )
			return status;

		status = bb_write( DCLK, 1 );
		if ( status != CB_OK )
			return status;

		// Flush the positive clk before reading
		//status = bb_flush();
		//if ( status != CB_OK )
		//	return status;

		status = bb_read( DATAOUT, &bit );
		if ( status != CB_OK )
			return status;

		if (bit!=0) //if bit is true
			*one_byte |= (mask << i);
	}

	return CB_OK;

}



/********************************************************************************/
/*	Name:			as_read_byte_msb											*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to read.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	read from parallel port bit by bit, from most significant	*/
/*					bit to least significant bit. A positive edge clock pulse	*/
/*					is also asserted. (read during positive edge)				*/
/*																				*/
/********************************************************************************/
int as_read_byte_msb( int *one_byte )
{
	int status = 0;
	int data_byte = 0;

	status = as_read_byte_lsb(&data_byte);
	if ( status != CB_OK )
			return status;


	//After conversion, MSB will come in first
	as_lsb_to_msb(&data_byte, one_byte);

	return CB_OK;

}


/********************************************************************************/
/*	Name:			as_lsb_to_msb												*/
/*																				*/
/*	Parameters:		int *in_byte					 							*/
/*					- The byte to convert.										*/
/*					int *out_byte												*/
/*					- The converted byte										*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Convert LSB to MSB											*/
/*																				*/
/*																				*/
/*																				*/
/********************************************************************************/
void as_lsb_to_msb( int *in_byte, int *out_byte)
{
	int		mask;
	int		i;
	int     temp;

	*out_byte = 0x00;


	for ( i = 0; i < 8; i++ )
	{
		temp = *in_byte >> i;
		mask = 0x80 >> i;

		if(temp & 0x01)	//if lsb is set inbyte, set msb for outbyte
		{
			*out_byte |= mask;
		}

	}
}
