/*////////////////////*/
/* Global Definitions */
/*////////////////////*/

#define CHECK_EVERY_X_BYTE	10240
#define INIT_CYCLE			200



/*///////////////////////*/
/* AS Instruction Set    */
/*///////////////////////*/
#define AS_WRITE_ENABLE				0x06
#define AS_WRITE_DISABLE			0x04
#define AS_READ_STATUS	    		0x05
#define AS_WRITE_STATUS	    		0x01
#define AS_READ_BYTES   			0x03
#define AS_FAST_READ_BYTES  		0x0B
#define AS_PAGE_PROGRAM				0x02
#define AS_ERASE_SECTOR				0xD8
#define AS_ERASE_BULK				0xC7
#define AS_READ_SILICON_ID			0xAB
#define AS_CHECK_SILICON_ID			0x9F


/*///////////////////////*/
/* Silicon ID for EPCS   */
/*///////////////////////*/
#define EPCS1_ID	0x10
#define EPCS4_ID	0x12
#define EPCS16_ID	0x14
#define EPCS64_ID	0x16
#define EPCS128_ID	0x18


/*///////////////////////*/
/* EPCS device			 */
/*///////////////////////*/
#define EPCS1		1
#define EPCS4		4
#define EPCS16		16
#define EPCS64		64
#define EPCS128		128

#define DEV_READBACK   0xFF //Special bypass indicator during EPCS data readback


/*///////////////////////*/
/* Functions Prototyping */
/*///////////////////////*/

int as_program( char*, int);
int as_read( char*, int );
int as_erase( int );
int as_ver( char *, int );
int as_open( char*, int*, long int* );
int as_close( int );
int as_program_start( void );
int as_program_done(void);
int as_bulk_erase( void );
int as_prog( int, int );
int as_silicon_id(int, int);
int as_program_byte_lsb( int );
int as_read_byte_lsb( int* );
int as_program_byte_msb( int );
int as_read_byte_msb( int* );
int as_readback(int);
int as_verify( int, int);
void as_lsb_to_msb( int *, int *);
