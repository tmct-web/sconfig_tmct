/****************************************************************************/
/*																			*/
/*	Module:			mb_io.c	(MicroBlaster)									*/	
/*																			*/
/*					Copyright (C) Altera Corporation 2001					*/
/*																			*/
/*	Descriptions:	Defines all IO control functions. operating system		*/
/*					is defined here. Functions are operating system 		*/
/*					dependant.												*/
/*																			*/
/*	Revisions:		1.0	12/10/01 Sang Beng Ng								*/
/*					Supports Altera ByteBlaster hardware download cable		*/
/*					on Windows NT.											*/
/*																			*/
/****************************************************************************/

#include <stdio.h>
#include "user.h"

#include "bb.h"


/*////////////////////*/
/* Global Definitions */
/*////////////////////*/
#define	PGDC_IOCTL_GET_DEVICE_INFO_PP	0x00166A00L
#define PGDC_IOCTL_READ_PORT_PP			0x00166A04L
#define PGDC_IOCTL_WRITE_PORT_PP		0x0016AA08L
#define PGDC_IOCTL_PROCESS_LIST_PP		0x0016AA1CL
#define PGDC_WRITE_PORT					0x0a82
#define PGDC_HDLC_NTDRIVER_VERSION		2
#define PORT_IO_BUFFER_SIZE				256


/*//////////////////*/
/* Global Variables */
/*//////////////////*/
HANDLE	nt_device_handle		= INVALID_HANDLE_VALUE;
int		port_io_buffer_count	= 0;




struct PORT_IO_LIST_STRUCT
{
	USHORT command;
	USHORT data;
} port_io_buffer[PORT_IO_BUFFER_SIZE];

int bb_type = 0;

/* port_data holds the current values of signals for every port. By default, they hold the values in */
/*   reset mode (PM_RESET_<ByteBlaster used>). */
/*   port_data[Z], where Z - port number, holds the value of the port. */
int	cur_data = 0x42;/* Initial value for Port 0, 1 and 2 */


/********************************************************************************/
/*	Name:			InitNtDriver  												*/
/*																				*/
/*	Parameters:		None.          												*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Initiallize Windows NT Driver for ByteBlasterMV.			*/
/*																				*/
/********************************************************************************/
int bb_open( void )
{
	int init_ok = 0;	/* Initialization OK */
	int status = 0;

	ULONG buffer[1];
	ULONG returned_length = 0;
	char nt_lpt_str[] = { '\\', '\\', '.', '\\', 'A', 'L', 'T', 'L', 'P', 'T', '1', '\0' }; 

	nt_device_handle = CreateFile( 
			nt_lpt_str,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

	if ( nt_device_handle == INVALID_HANDLE_VALUE )
	{
		fprintf( stderr, "I/O Error: Cannot open device \"%s\"\n", nt_lpt_str );
		status = CB_BB_OPEN_ERROR_OPEN_PORT;
	}
	else
	{
		if ( DeviceIoControl(
				nt_device_handle,
				PGDC_IOCTL_GET_DEVICE_INFO_PP,
				(ULONG *) NULL,
				0,
				&buffer,
				sizeof(ULONG),
				&returned_length,
				NULL ))
		{
			if ( returned_length == sizeof( ULONG ) )
			{
				if (buffer[0] == PGDC_HDLC_NTDRIVER_VERSION)
				{
					init_ok = 1;
					fprintf( stdout, "Info: Port \"%s\" opened.\n", nt_lpt_str );
				}
				else
				{
					fprintf(stderr,
						"I/O Error:  device driver %s is not compatible!\n(Driver version is %lu, expected version %lu.\n",
						nt_lpt_str,
						(unsigned long) buffer[0],
						(unsigned long) PGDC_HDLC_NTDRIVER_VERSION );
					status = CB_BB_OPEN_VERSION_INCOMPATIBLE;
				}
			}	
			else
				fprintf(stderr, "I/O Error:  device driver %s is not compatible!\n", nt_lpt_str);
				status = CB_BB_OPEN_DRIVER_INCOMPATIBLE;
		}

		if ( !init_ok )
		{
			fprintf( stderr, "I/O Error: DeviceIoControl not successful!" );
			CloseHandle( nt_device_handle );
			nt_device_handle = INVALID_HANDLE_VALUE;
			status = CB_BB_OPEN_DEVICEIOCONTROL_FAIL;
		}
	}

	if ( !init_ok )
	{
		fprintf( stderr, "Error: Driver initialization fail!\n" );
		CloseHandle( nt_device_handle );
		nt_device_handle = INVALID_HANDLE_VALUE;
		return status;
	}
	else
	{
		status = bb_verify( &bb_type );
		if ( status != CB_OK )
		{
			CloseHandle( nt_device_handle );
			nt_device_handle = INVALID_HANDLE_VALUE;
			return status;
		}
		else
		{
			if ( bb_type == 1 )
				status = bb_reset( BBMV_CONFIG_MODE );
			else if ( bb_type == 2)
				status = bb_reset( BBII_CONFIG_MODE );
			
			return status;
		}
	}
}


/********************************************************************************/
/*	Name:			CloseNtDriver 												*/
/*																				*/
/*	Parameters:		None.          												*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Close Windows NT Driver.									*/
/*																				*/
/********************************************************************************/
int bb_close( void )
{
	int status = 0;
	
	if ( bb_type == BBNONE )
	{
		fprintf(stderr, "ByteBlaster not opened!");
		return CB_BB_CLOSE_BYTEBLASTER_NOT_OPEN;
	}
	else
	{
		if ( bb_type == BBMV )
			status = bb_reset( BBMV_USER_MODE );
		else if ( bb_type == BBII)
			status = bb_reset( BBII_USER_MODE );
		
		if ( status == CB_OK )
			bb_type = BBNONE;
		return status;
	}
	
	CloseHandle( nt_device_handle );
	nt_device_handle = INVALID_HANDLE_VALUE;
}


/********************************************************************************/
/*	Name:			flush_ports 												*/
/*																				*/
/*	Parameters:		None.          												*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Flush processes in [port_io_buffer]	and reset buffer		*/
/*					size to 0.													*/
/*																				*/
/********************************************************************************/
int bb_flush( void )
{
	ULONG n_writes = 0L;
	BOOL status;

	status = DeviceIoControl(
		nt_device_handle,			/* handle to device */
		PGDC_IOCTL_PROCESS_LIST_PP,	/* IO control code */
		(LPVOID)port_io_buffer,		/* IN buffer (list buffer) */
		port_io_buffer_count * sizeof(struct PORT_IO_LIST_STRUCT),/* length of IN buffer in bytes */
		(LPVOID)port_io_buffer,	/* OUT buffer (list buffer) */
		port_io_buffer_count * sizeof(struct PORT_IO_LIST_STRUCT),/* length of OUT buffer in bytes */
		&n_writes,					/* number of writes performed */
		0);							/* wait for operation to complete */

	if ((!status) || ((port_io_buffer_count * sizeof(struct PORT_IO_LIST_STRUCT)) != n_writes))
	{
		fprintf(stderr, "I/O Error:  Cannot flush ByteBlaster hardware!\n");
		return CB_BB_FLUSH_ERROR;
	}
	else
	{
		port_io_buffer_count = 0;
		return CB_OK;
	}
}


/******************************************************************/
/* Name:         VerifyBBII (ByteBlaster II)					  */
/*                                                                */
/* Parameters:   None.                                            */
/*                                                                */
/* Return Value: '0' if verification is successful;'1' if not.    */
/*               		                                          */
/* Descriptions: Verify if ByteBlaster II is properly attached to */
/*               the parallel port.                               */
/*                                                                */
/******************************************************************/
int bb_verify( int *types )
{
	int status = 0;
	
	int type = 0;
	int test_count = 0;
	int read_data = 0;
	int error = 0;
	int i = 0;
	
	for ( type = 0; type < 2; type++ )
	{
		int vector = (type) ? 0x10 : 0xA0;
		int expect = (type) ? 0x40 : 0x60;
		int vtemp;
		
		for ( test_count = 0; test_count < 2; test_count++ )
		{
			/* Write '0' to Pin 6 (Data4) for the first test and '1' for the second test */
			vtemp = (test_count) ? (vector & 0xff) : 0x00;/* 0001 0000:0000 0000... drive to Port0 */

			status = bb_lptwrite( LPT_DATA, vtemp, 1 );
			if ( status != CB_OK )
				return status;

			//delay
			for (i=0;i<1500;i++);

			/* Expect '0' at Pin 10 (Ack) and Pin 15 (Error) for the first test */
			/* and '1' at Pin 10 (Ack) and '0' Pin 15 (Error) for the second test */
			status = bb_lptread( LPT_STATUS, &read_data );
			if ( status != CB_OK )
				return status;
			
			read_data = read_data & (expect & 0xff);

			/* If no ByteBlaster II detected, error = 1 */
			if (test_count==0)
			{
				if(read_data==0x00)
					error=0;
				else error=1;
			}

			if (test_count==1)
			{
				if(read_data == (expect & 0xff))
					error=error|0;
				else error=1;
			}
		}

		if ( !error )
			break;
	}

	if (!type)
	{
		fprintf( stdout, "Info: Verifying hardware: ByteBlasterMV found.\n" );
		*types = BBMV;
		return CB_OK;
	}
	else
	{
		if (!error)
		{
			fprintf( stdout, "Info: Verifying hardware: ByteBlaster II found.\n" );
			*types = BBII;
			return CB_OK;
		}
		else
		{
			fprintf( stderr, "Error: Verifying hardware: ByteBlaster not found or not installed properly!\n" );
			return CB_BB_VERIFY_BYTEBLASTER_NOT_FOUND;
		}
	}
}


/********************************************************************************/
/*	Name:			ReadByteBlaster												*/
/*																				*/
/*	Parameters:		int port       												*/
/*					- port number 0, 1, or 2. Index to parallel port base		*/
/*					  address.													*/
/*																				*/
/*	Return Value:	Integer, value of the port.									*/
/*																				*/
/*	Descriptions:	Read the value of the port registers.						*/
/*																				*/
/********************************************************************************/
int bb_lptread( int port, int *data )
{
	int temp = 0;
	int status = 0;
	int returned_length = 0;

	status = DeviceIoControl(
			nt_device_handle,			/* Handle to device */
			PGDC_IOCTL_READ_PORT_PP,	/* IO Control code for Read */
			(ULONG *)&port,				/* Buffer to driver. */
			sizeof(int),				/* Length of buffer in bytes. */
			(ULONG *)&temp,				/* Buffer from driver. */
			sizeof(int),				/* Length of buffer in bytes. */
			(ULONG *)&returned_length,	/* Bytes placed in data_buffer. */
			NULL);						/* Wait for operation to complete */

	if ((!status) || (returned_length != sizeof(int)))
	{
		fprintf(stderr, "I/O error:  Cannot read from ByteBlaster hardware!\n");
		return CB_BB_LPTREAD_ERROR;
	}
	else
	{
		*data = temp & 0xff;
		return CB_OK;
	}
}


/********************************************************************************/
/*	Name:			WriteByteBlaster											*/
/*																				*/
/*	Parameters:		int port, int data, int test								*/
/*					- port number 0, 1, or 2. Index to parallel port base		*/
/*					  address.													*/
/*					- value to written to port registers.						*/
/*					- purpose of write.											*/ 
/*																				*/
/*	Return Value:	None                       									*/
/*																				*/
/*	Descriptions:	Write [data] to [port] registers. When dump to Port0, if	*/
/*					[test] = '0', processes in [port_io_buffer] are dumped		*/
/*					when [PORT_IO_BUFFER_SIZE] is reached. If [test] = '1',		*/
/*					[data] is dumped immediately to Port0.						*/
/*																				*/
/********************************************************************************/
int bb_lptwrite( int port, int data, int nbuffering )
{
	int status = 0;
	int returned_length = 0;
	int buffer[2];

	/* Collect up to [PORT_IO_BUFFER_SIZE] data for Port0, then flush them */
	/* if nbuffering = 1 or Port = 1 or Port = 2, writing to the ports are done immediately */
	if (port == 0 && nbuffering == 0)
	{
		port_io_buffer[port_io_buffer_count].data = (USHORT) data;
		port_io_buffer[port_io_buffer_count].command = PGDC_WRITE_PORT;
		++port_io_buffer_count;

		if (port_io_buffer_count >= PORT_IO_BUFFER_SIZE) bb_flush();
	}
	else
	{
		buffer[0] = port;
		buffer[1] = data;

		status = DeviceIoControl(
				nt_device_handle,			/* Handle to device */
				PGDC_IOCTL_WRITE_PORT_PP,	/* IO Control code for write */
				(ULONG *)&buffer,			/* Buffer to driver. */
				2 * sizeof(int),			/* Length of buffer in bytes. */
				(ULONG *)NULL,				/* Buffer from driver.  Not used. */
				0,							/* Length of buffer in bytes. */
				(ULONG *)&returned_length,	/* Bytes returned.  Should be zero. */
				NULL);						/* Wait for operation to complete */
		
		if ( !status )
		{
			fprintf(stderr, "I/O error:  Cannot write to ByteBlaster hardware!\n");
			return CB_BB_LPTWRITE_ERROR;
		}
	}
	return CB_OK;
}


/********************************************************************************/
/*	Name:			CheckSignal													*/
/*																				*/
/*	Parameters:		int signal						 							*/
/*					- name of the signal (SIG_*).								*/
/*																				*/
/*	Return Value:	Integer, the value of the signal. '0' is returned if the	*/
/*					value of the signal is LOW, if not, the signal is HIGH.		*/
/*																				*/
/*	Descriptions:	Return the value of the signal.								*/
/*																				*/
/********************************************************************************/
int bb_read( int signal, int *data )
{
	int temp = 0;
	int	status = 0;

	status = bb_lptread( LPT_STATUS, &temp );
	if ( status == CB_OK )
		*data = (temp ^ 0x80) & signal;
	return status;
}

/********************************************************************************/
/*	Name:			Dump2Port													*/
/*																				*/
/*	Parameters:		int signal, int data, int clk	 							*/
/*					- name of the signal (SIG_*).								*/
/*					- value to be dumped to the signal.							*/
/*					- assert a LOW to HIGH transition to SIG_DCLK togther with	*/
/*					  [signal].													*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Dump [data] to [signal]. If [clk] is '1', a clock pulse is	*/
/*					generated after the [data] is dumped to [signal].			*/
/*																				*/
/********************************************************************************/
int bb_write( int signal, int data )
{
	int status = 0;

	/* AND signal bit with '0', then OR with [data] */
	int mask = ~signal;

	cur_data = ( cur_data & mask ) | ( data * signal );
	status = bb_lptwrite( LPT_DATA, cur_data, 0 );
	return status;
}

/********************************************************************************/
/*	Name:			SetPortMode													*/
/*																				*/
/*	Parameters:		int mode  						 							*/
/*					- The mode of the port (PM_*)								*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Set the parallel port registers to particular values.		*/
/*																				*/
/********************************************************************************/
int bb_reset( int mode )
{
	int status = 0;

	/* write to Port 0 and Port 2 with predefined values */
	int control = mode ? 0x0C : 0x0E; 
	cur_data = 0x42;	
	

	status = bb_lptwrite( LPT_DATA, cur_data, 1 );
	if ( status == CB_OK )
		status = bb_lptwrite( LPT_CONTROL, control, 1 );
	return status;
}