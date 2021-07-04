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
#include <fcntl.h>
#include <stdlib.h>
#include "user.h"
#include "bb_gpio.h"

#define GPIO_PORT_ASDI                "4"
#define GPIO_PORT_nCE                 "17"
#define GPIO_PORT_nCS                 "18"
#define GPIO_PORT_nCONFIG             "27"
#define GPIO_PORT_DCLK                "22"
#define GPIO_PORT_CONFDONE            "23"
#define GPIO_PORT_DATA0               "24"

#define DRIVER_GPIO_EXPORT              "/sys/class/gpio/export"
#define DRIVER_GPIO_UNEXPORT            "/sys/class/gpio/unexport"
#define DRIVER_GPIO_ASDI_DIRECTION      "/sys/class/gpio/gpio4/direction"
#define DRIVER_GPIO_nCE_DIRECTION       "/sys/class/gpio/gpio17/direction"
#define DRIVER_GPIO_nCS_DIRECTION       "/sys/class/gpio/gpio18/direction"
#define DRIVER_GPIO_nCONFIG_DIRECTION   "/sys/class/gpio/gpio27/direction"
#define DRIVER_GPIO_DCLK_DIRECTION      "/sys/class/gpio/gpio22/direction"
#define DRIVER_GPIO_CONFDONE_DIRECTION  "/sys/class/gpio/gpio23/direction"
#define DRIVER_GPIO_DATA0_DIRECTION     "/sys/class/gpio/gpio24/direction"
#define DRIVER_GPIO_ASDI_VALUE          "/sys/class/gpio/gpio4/value"
#define DRIVER_GPIO_nCE_VALUE           "/sys/class/gpio/gpio17/value"
#define DRIVER_GPIO_nCS_VALUE           "/sys/class/gpio/gpio18/value"
#define DRIVER_GPIO_nCONFIG_VALUE       "/sys/class/gpio/gpio27/value"
#define DRIVER_GPIO_DCLK_VALUE          "/sys/class/gpio/gpio22/value"
#define DRIVER_GPIO_CONFDONE_VALUE      "/sys/class/gpio/gpio23/value"
#define DRIVER_GPIO_DATA0_VALUE         "/sys/class/gpio/gpio24/value"

int fhandle_ASDI = 0;
int fhandle_nCE = 0;
int fhandle_nCS = 0;
int fhandle_nCONFIG = 0;
int fhandle_DCLK = 0;
int fhandle_CONFDONE = 0;
int fhandle_DATA0 = 0;

int bb_open( void )
{
  volatile int init_ok = 0;	/* Initialization OK */

  /*-------------------------------------------------------*/
  // GPIO Reserve
  /*-------------------------------------------------------*/
  init_ok = DriverWrite(DRIVER_GPIO_EXPORT, GPIO_PORT_ASDI, sizeof(GPIO_PORT_ASDI));
  init_ok += DriverWrite(DRIVER_GPIO_EXPORT, GPIO_PORT_nCE, sizeof(GPIO_PORT_nCE));
  init_ok += DriverWrite(DRIVER_GPIO_EXPORT, GPIO_PORT_nCS, sizeof(GPIO_PORT_nCS));
  init_ok += DriverWrite(DRIVER_GPIO_EXPORT, GPIO_PORT_nCONFIG, sizeof(GPIO_PORT_nCONFIG));
  init_ok += DriverWrite(DRIVER_GPIO_EXPORT, GPIO_PORT_DCLK, sizeof(GPIO_PORT_DCLK));
  init_ok += DriverWrite(DRIVER_GPIO_EXPORT, GPIO_PORT_CONFDONE, sizeof(GPIO_PORT_CONFDONE));
  init_ok += DriverWrite(DRIVER_GPIO_EXPORT, GPIO_PORT_DATA0, sizeof(GPIO_PORT_DATA0));
  if (init_ok != 0)
  {
    fprintf( stderr, "I/O Error: Cannot open device.\n" );
    bb_close();
    return (CB_BB_OPEN_ERROR_OPEN_PORT);
  }

  /*-------------------------------------------------------*/
  // GPIO Direction setting
  /*-------------------------------------------------------*/
  usleep(100000);
  init_ok = DriverWrite(DRIVER_GPIO_ASDI_DIRECTION, "out", 4);
  init_ok += DriverWrite(DRIVER_GPIO_nCE_DIRECTION, "out", 4);
  init_ok += DriverWrite(DRIVER_GPIO_nCS_DIRECTION, "out", 4);
  init_ok += DriverWrite(DRIVER_GPIO_nCONFIG_DIRECTION, "out", 4);
  init_ok += DriverWrite(DRIVER_GPIO_DCLK_DIRECTION, "out", 4);
  init_ok += DriverWrite(DRIVER_GPIO_CONFDONE_DIRECTION, "in", 3);
  init_ok += DriverWrite(DRIVER_GPIO_DATA0_DIRECTION, "in", 3);
  if (init_ok != 0)
  {
    fprintf( stderr, "Error: Driver initialization fail.\n" );
    bb_close();
    return (CB_BB_OPEN_DEVICEIOCONTROL_FAIL);
  }

  /*-------------------------------------------------------*/
  // Open GPIO value handle
  /*-------------------------------------------------------*/
  fhandle_ASDI = open(DRIVER_GPIO_ASDI_VALUE, O_WRONLY);
  fhandle_nCE = open(DRIVER_GPIO_nCE_VALUE, O_WRONLY);
  fhandle_nCS = open(DRIVER_GPIO_nCS_VALUE, O_WRONLY);
  fhandle_nCONFIG = open(DRIVER_GPIO_nCONFIG_VALUE, O_WRONLY);
  fhandle_DCLK = open(DRIVER_GPIO_DCLK_VALUE, O_WRONLY);
  fhandle_CONFDONE = open(DRIVER_GPIO_CONFDONE_VALUE, O_RDONLY);
  fhandle_DATA0 = open(DRIVER_GPIO_DATA0_VALUE, O_RDONLY);
  if ((fhandle_ASDI < 0) || (fhandle_nCE < 0) || (fhandle_nCS < 0) || (fhandle_nCONFIG < 0) ||
      (fhandle_DCLK < 0) || (fhandle_CONFDONE < 0) || (fhandle_DATA0 < 0))
  {
    fprintf( stderr, "Error: Driver handling fail.\n" );
    bb_close();
    return (CB_BB_OPEN_DEVICEIOCONTROL_FAIL);
  }
  fprintf( stdout, "Info: Driver opened.\n" );
  return (CB_OK);
}

void bb_close( void )
{
  volatile int result = 0;
  result = DriverWrite(DRIVER_GPIO_UNEXPORT, GPIO_PORT_ASDI, sizeof(GPIO_PORT_ASDI));
	result = DriverWrite(DRIVER_GPIO_UNEXPORT, GPIO_PORT_nCE, sizeof(GPIO_PORT_nCE));
	result = DriverWrite(DRIVER_GPIO_UNEXPORT, GPIO_PORT_nCS, sizeof(GPIO_PORT_nCS));
	result = DriverWrite(DRIVER_GPIO_UNEXPORT, GPIO_PORT_nCONFIG, sizeof(GPIO_PORT_nCONFIG));
	result = DriverWrite(DRIVER_GPIO_UNEXPORT, GPIO_PORT_DCLK, sizeof(GPIO_PORT_DCLK));
  result = DriverWrite(DRIVER_GPIO_UNEXPORT, GPIO_PORT_CONFDONE, sizeof(GPIO_PORT_CONFDONE));
  result = DriverWrite(DRIVER_GPIO_UNEXPORT, GPIO_PORT_DATA0, sizeof(GPIO_PORT_DATA0));
	close(fhandle_ASDI);
	close(fhandle_nCE);
	close(fhandle_nCS);
	close(fhandle_nCONFIG);
	close(fhandle_DCLK);
  close(fhandle_CONFDONE);
  close(fhandle_DATA0);
  fprintf( stdout, "\nInfo: Driver closed.\n" );
}

int bb_read		( int sig, int *data )
{
  char rdata = 0;
  if (sig == CONF_DONE)
  {
    lseek(fhandle_CONFDONE, 0, SEEK_SET);
  	read(fhandle_CONFDONE, &rdata, 1);
    if (rdata == '0') *data = 0x80; else *data = 0x00;
  }
  else
  {
    lseek(fhandle_DATA0, 0, SEEK_SET);
  	read(fhandle_DATA0, &rdata, 1);
    if (rdata == '0') *data = 0x00; else *data = 0x10;
  }
  return (CB_OK);
}

int bb_write	( int sig, int data )
{
  if (sig == DCLK)
  {
    if (data == 0) write(fhandle_DCLK, "0", 2); else write(fhandle_DCLK, "1", 2);
  }
  else if (sig == NCONFIG)
  {
    if (data == 0) write(fhandle_nCONFIG, "0", 2); else write(fhandle_nCONFIG, "1", 2);
  }
  else if (sig == NCS)
  {
    if (data == 0) write(fhandle_nCS, "0", 2); else write(fhandle_nCS, "1", 2);
  }
  else if (sig == NCE)
  {
    if (data == 0) write(fhandle_nCE, "0", 2); else write(fhandle_nCE, "1", 2);
  }
  else
  {
    if (data == 0) write(fhandle_ASDI, "0", 2); else write(fhandle_ASDI, "1", 2);
  }
  return (CB_OK);
}

/*---------------------------------------------------------*/
//  Driver write function
/*---------------------------------------------------------*/
int DriverWrite(char *desc, char *param, int len)
{
  int fhandle = 0;
  fhandle = open(desc, O_WRONLY);
  if (fhandle < 0) return(1);
  else
  {
    write(fhandle, param, len);
    close(fhandle);
    return(0);
  }
}
