/******************************************************************************
* \file             uart_io.c
*
* \version          1.10
*
* \brief            This file retargets printf()/scanf() functions to UART by 
*                   overriding the fputc()/fgetc() or _write()/read() stub
*                   routines. 
*
********************************************************************************
* \copyright
* Copyright 2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
* CYPRESS PROVIDES THIS SOFTWARE "AS IS" AND MAKES NO WARRANTY
* OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS SOFTWARE,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
* PURPOSE.
*******************************************************************************/

#ifndef __UART_IO__
#define __UART_IO__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "mcu.h"
//#include "pdl_user.h"
#include <stdio.h>

#if (PDL_UTILITY_ENABLE_UART_PRINTF == PDL_ON)

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup UartPrintfGroup Uart Input/Output Function
 **
 ** Provided functions of UartIoGroup module:
 **
 ** - Uart_Io_Init()
 ** - fputc()
 ** - fgetc()
 **
 ** To enable the pirntf/scanf via UART0 funciton, Uart_Io_Init() must be called
 ** to initialize UART0.
 **
 ** fputc() will be called in printf() to output one byte via UART0. 
 ** fgetc() will be called in scanf() to receive one byte from UART0. 
 **
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
 
/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
void Uart_Io_Init(void);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

//@} // UartPrintfGroup

#ifdef __cplusplus
}
#endif

#endif /* #if (PDL_UTILITY_ENABLE_UART_PRINTF == PDL_ON) */

#endif /* __UART_PRINTF__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
