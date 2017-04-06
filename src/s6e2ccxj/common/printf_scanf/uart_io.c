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

#include "uart_io.h"
#include "mfs/mfs.h"
#include "gpio/gpio.h"   

#if (PDL_UTILITY_ENABLE_UART_PRINTF == PDL_ON) || (PDL_UTILITY_ENABLE_UART_SCANF == PDL_ON)

#if (PDL_PERIPHERAL_ENABLE_MFS_UART_MODE == PDL_OFF)
    #error "Enable MFS UART Mode in pdl_user.h to use printf()/scanf() functions." 
#endif

/*******************************************************************************
* UART channel to use for I/O       
*******************************************************************************/
volatile stc_mfsn_uart_t* UartCh = &UART0;

/**************************************************************************//***
 \brief  UART initialization
*******************************************************************************/
void Uart_Io_Init(void)
{
    stc_mfs_uart_config_t stcUartConfig;
    PDL_ZERO_STRUCT(stcUartConfig);

    /* Initialize UART */
    stcUartConfig.enMode         = UartNormal;
    stcUartConfig.u32BaudRate    = 115200;
    stcUartConfig.enDataLength   = UartEightBits;
    stcUartConfig.enParity       = UartParityNone;
    stcUartConfig.enStopBit      = UartOneStopBit;
    stcUartConfig.enBitDirection = UartDataLsbFirst;
    stcUartConfig.bInvertData    = FALSE;
    stcUartConfig.bHwFlow        = FALSE;
    stcUartConfig.pstcFifoConfig = NULL;
    
    Mfs_Uart_Init(UartCh, &stcUartConfig);

    /* Set UART I/O pins and enable TX and/or RX functions of UART0 */
#if (PDL_UTILITY_ENABLE_UART_PRINTF == PDL_ON)
    SetPinFunc_SOT0_0();    
    Mfs_Uart_EnableFunc(UartCh, UartTx);
#endif
#if (PDL_UTILITY_ENABLE_UART_SCANF == PDL_ON)
    SetPinFunc_SIN0_0();    
    Mfs_Uart_EnableFunc(UartCh, UartRx);
#endif    
}

#if (PDL_UTILITY_ENABLE_UART_PRINTF == PDL_ON)  

/**************************************************************************//***
 ** \brief  Override fputc() / _write() for printf() output to UART
 ******************************************************************************/
#if defined (__CC_ARM) || defined (__ICCARM__)
int fputc(int ch, FILE *f)
{
    if(((uint8_t)ch) == '\n')
    {
        while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty));
        Mfs_Uart_SendData(UartCh, '\r');
    }
    while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty))
    {
        /* Wait for room in the Tx FIFO */
    }
    Mfs_Uart_SendData(UartCh, ch);
    return ch;
}
#else /* __GNUC__ */
int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for(i = 0; i < len; i++)
    {
        if(*ptr == '\n')
        {
            while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty))
            {
                /* Wait for room in the Tx FIFO */
            };
            Mfs_Uart_SendData(UartCh, '\r');
        }
        while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty))
        {
            /* Wait for room in the Tx FIFO */
        }
        Mfs_Uart_SendData(UartCh, (uint16_t)*ptr);
        ++ptr;
    }
    return len;
}
#endif /* (__CC_ARM) || defined (__ICCARM__) */
#endif /* (PDL_UTILITY_ENABLE_UART_PRINTF == PDL_ON) */

#if (PDL_UTILITY_ENABLE_UART_SCANF == PDL_ON)   

/***************************************************************************//**
* \brief  Override fgetc() / _read() for scanf() input from UART
*******************************************************************************/
#if defined (__CC_ARM) || defined (__ICCARM__)
int fgetc(FILE *f)
{ 
    while (TRUE != Mfs_Uart_GetStatus(UartCh, UartRxFull))
    {
        /* Wait until there is a character in the Rx FIFO */ 
    }
    return (int)Mfs_Uart_ReceiveData(UartCh);
}
#else
/* __GNUC__ */
int _read(int file, char *ptr, int len)
{
    int i;
    file = file;
    for(i = 0; i < len; i++)
    {
        while (TRUE != Mfs_Uart_GetStatus(UartCh, UartRxFull))
        {
            /* Wait until there is a character in the Rx FIFO */
        }
        *ptr++ = (char)Mfs_Uart_ReceiveData(UartCh);
    }
    return len;
}
#endif /* (__CC_ARM) || defined (__ICCARM__) */
#endif /* (PDL_UTILITY_ENABLE_UART_SCANF == PDL_ON) */

#endif /* (PDL_UTILITY_ENABLE_UART_PRINTF == PDL_ON) || (PDL_UTILITY_ENABLE_UART_SCANF == PDL_ON) */

/* [] END OF FILE */

