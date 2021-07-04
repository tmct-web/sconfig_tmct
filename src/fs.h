/*////////////////////*/
/* Global Definitions */
/*////////////////////*/

#define S_CUR				1 /* SEEK_CUR */
#define S_END				2 /* SEEK_END */
#define S_SET				0 /* SEEK_SET */


/*///////////////////////*/
/* Functions Prototyping */
/*///////////////////////*/

int fs_open( char[], char*, int* );
int	fs_close( int );
int fs_size( int, long int* );
int fs_read( int, int* );

int fs_open_log(void);			//Srunner 4827
void fs_write(int, int);		//Srunner 4827
void fs_rewind( int file_id);	//Srunner 4827
