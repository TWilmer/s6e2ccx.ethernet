/*******************************************************************************
* Copyright (C) 2013 Spansion LLC. All Rights Reserved.
*
* This software is owned and published by:
* Spansion LLC, 915 DeGuigne Dr. Sunnyvale, CA  94088-3453 ("Spansion").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with Spansion
* components. This software is licensed by Spansion to be adapted only
* for use in systems utilizing Spansion components. Spansion shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein.  Spansion is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* SPANSION MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* SPANSION SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF SPANSION HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file emac.c
 **
 ** A detailed description is available at
 ** @link EmacGroup EMAC Module description @endlink
 **
 ** History:
 **   - 2012-11-20  1.0  Fujitsu    First version.
 **   - 2012-11-23  1.1  CNo, MWi   Improved L3 compatibility
 **   - 2012-12-10  1.1a QXu,CNo    Work-around for LwIP and uIP ICMP echo bug
 **   - 2012-12-18  1.2  YMo, CNo   Support for multicast, bugfixes
 **   - 2013-08-09  1.3  CNo        Bugfixes, License ownership transfered to Spansion Inc.
 **                                 Added promiscuous mode for EMAC driver
 **                                 Improved support for Flow Control
 **   - 2014-07-15  1.4  CNo        Fixed a bug with 10Mbit mode
 **                                 Better IRQ support for Ethernet MCUs with and without CAN option
 **                                 Deactivate MMC counting to prevent lock-up
 **                                 MAC address now set via stc_emac_config_t
 **                                 Distinction MB9B610 and MB9BD10 now done automatically
 **                                 Isolated link-setup routines from auto-negotiate function to reuse it in for manual mode selection
 **   - 2014-10-17  1.5  CNo        New function Emac_TxFrameDirect() for optimized throughput e.g. in Ethernet switch software
 **                                 Adapted for PDL compatibility and FM4 support
 **                                 IRQ handling improved
 **   - 2015-01-09  1.6  CNo        Improved autonegotiation feature for universal use
 **   - 2015-07-27  1.7  CNo        Updated driver to be compliant with template structure verson
 **                                 Integrated ethphy.c into emac.c and ethphy.h into emac.h in order to simplify code structure
 **   - 2015-08-31  1.8  CNo        Bugfixes for operation with IRQ enabled operation
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "emac.h"

#if EMAC_ECOUT == PDL_ON
#include "usbethernetclock.h"
#endif

#if (defined(PDL_PERIPHERAL_EMAC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup EmacGroup
 ******************************************************************************/
//@{
#if (defined(EMAC_USE_PDL) && (EMAC_USE_PDL == 0))
void PDL_WAIT_LOOP_HOOK(void){}
#endif

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

///  Macro to return the number of enabled EMAC instances
#define EMAC_INSTANCE_COUNT (uint32_t)(sizeof(m_astcEmacInstanceDataLut) / sizeof(m_astcEmacInstanceDataLut[0]))

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/

/// Look-up table for all enabled EMAC instances and their internal data
stc_emac_instance_data_t m_astcEmacInstanceDataLut[] =
{
    #if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)
    { &EMAC0, // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
    #if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
    { &EMAC1, // pstcInstance
      NULL    // stcInternData (not initialized yet)
    },
    #endif
};

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/
static en_result_t Emac_InitRxDescChain(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);
static en_result_t Emac_InitTxDescChain(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

#if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)

// EMAC-DMAC descriptors must be defined in any case
#if defined (  __CC_ARM  ) // Keil compiler
  __align(4)
static volatile stc_emac_dma_tx_desc_t astcEmac0TxDescChain[EMAC0_TX_RING_SIZE];
  __align(4)
static volatile stc_emac_dma_rx_desc_t astcEmac0RxDescChain[EMAC0_RX_RING_SIZE];

#elif defined (  __ICCARM__  ) //IAR compiler
#pragma data_alignment = 4
static volatile stc_emac_dma_tx_desc_t astcEmac0TxDescChain[EMAC0_TX_RING_SIZE];
#pragma data_alignment = 4
static volatile stc_emac_dma_rx_desc_t astcEmac0RxDescChain[EMAC0_RX_RING_SIZE];

#elif defined (  __GNUC__  ) // GCC compiler
static volatile stc_emac_dma_tx_desc_t astcEmac0TxDescChain[EMAC0_TX_RING_SIZE] __attribute__ ((aligned (4)));
static volatile stc_emac_dma_rx_desc_t astcEmac0RxDescChain[EMAC0_RX_RING_SIZE] __attribute__ ((aligned (4)));
#endif

#if (EMAC0_BUFFERS_IN_DRIVERSPACE == PDL_ON)
#if defined (  __CC_ARM  ) // Keil compiler
  __align(4)
static volatile uint8_t au8Emac0TxBufs[EMAC0_TX_RING_SIZE][EMAC0_TX_BUF_SIZE];
  __align(4)
static volatile uint8_t au8Emac0RxBufs[EMAC0_RX_RING_SIZE][EMAC0_RX_BUF_SIZE];

#elif defined (  __ICCARM__  ) //IAR compiler
#pragma data_alignment = 4
static volatile uint8_t au8Emac0TxBufs[EMAC0_TX_RING_SIZE][EMAC0_TX_BUF_SIZE];
#pragma data_alignment = 4
static volatile uint8_t au8Emac0RxBufs[EMAC0_RX_RING_SIZE][EMAC0_RX_BUF_SIZE];

#elif defined (  __GNUC__  ) // GCC compiler
static volatile uint8_t au8Emac0TxBufs[EMAC0_TX_RING_SIZE][EMAC0_TX_BUF_SIZE] __attribute__ ((aligned (4)));
static volatile uint8_t au8Emac0RxBufs[EMAC0_RX_RING_SIZE][EMAC0_RX_BUF_SIZE] __attribute__ ((aligned (4)));
#endif
#endif // EMAC0_BUFFERS_IN_DRIVERSPACE

#endif // PDL_PERIPHERAL_ENABLE_EMAC0


#if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
// EMAC-DMAC descriptors must be defined in any case

#if defined (  __CC_ARM  ) // Keil compiler
  __align(4)
static volatile stc_emac_dma_tx_desc_t astcEmac1TxDescChain[EMAC1_TX_RING_SIZE];
  __align(4)
static volatile stc_emac_dma_rx_desc_t astcEmac1RxDescChain[EMAC1_RX_RING_SIZE];

#elif defined (  __ICCARM__  ) //IAR compiler
#pragma data_alignment = 4
static volatile stc_emac_dma_tx_desc_t astcEmac1TxDescChain[EMAC1_TX_RING_SIZE];
#pragma data_alignment = 4
static volatile stc_emac_dma_rx_desc_t astcEmac1RxDescChain[EMAC1_RX_RING_SIZE];

#elif defined (  __GNUC__  ) // GCC compiler
static volatile stc_emac_dma_tx_desc_t astcEmac1TxDescChain[EMAC1_TX_RING_SIZE] __attribute__ ((aligned (4)));
static volatile stc_emac_dma_rx_desc_t astcEmac1RxDescChain[EMAC1_RX_RING_SIZE] __attribute__ ((aligned (4)));
#endif

#if (EMAC1_BUFFERS_IN_DRIVERSPACE == PDL_ON)
#if defined (  __CC_ARM  ) // Keil compiler
  __align(4)
static volatile uint8_t au8Emac1TxBufs[EMAC1_TX_RING_SIZE][EMAC1_TX_BUF_SIZE];
  __align(4)
static volatile uint8_t au8Emac1RxBufs[EMAC1_RX_RING_SIZE][EMAC1_RX_BUF_SIZE];

#elif defined (  __ICCARM__  ) //IAR compiler
#pragma data_alignment = 4
static volatile uint8_t au8Emac1TxBufs[EMAC1_TX_RING_SIZE][EMAC1_TX_BUF_SIZE];
#pragma data_alignment = 4
static volatile uint8_t au8Emac1RxBufs[EMAC1_RX_RING_SIZE][EMAC1_RX_BUF_SIZE];

#elif defined (  __GNUC__  ) // GCC compiler
static volatile uint8_t au8Emac1TxBufs[EMAC1_TX_RING_SIZE][EMAC1_TX_BUF_SIZE] __attribute__ ((aligned (4)));
static volatile uint8_t au8Emac1RxBufs[EMAC1_RX_RING_SIZE][EMAC1_RX_BUF_SIZE] __attribute__ ((aligned (4)));
#endif
#endif // EMAC1_BUFFERS_IN_DRIVERSPACE

#endif // PDL_PERIPHERAL_ENABLE_EMAC1

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/
/**
 *****************************************************************************
 ** \brief Return the internal data for a certain EMAC instance.
 **
 ** \param pstcEmac Pointer to EMAC instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled
 ** (or not known)
 **
 *****************************************************************************/
static stc_emac_intern_data_t* EmacGetInternDataPtr(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  if (pstcEmac == m_astcEmacInstanceDataLut[0].pstcInstance)
  {
    return &m_astcEmacInstanceDataLut[0].stcInternData;
  }

  if (pstcEmac == m_astcEmacInstanceDataLut[1].pstcInstance)
  {
    return &m_astcEmacInstanceDataLut[1].stcInternData;
  }

  return NULL;
}

#if (EMAC_INTERRUPT_MODE == PDL_ON)
/**
 ******************************************************************************
 ** \brief Device dependent initialization of interrupts according CMSIS with
 **        level defined in pdl.h
 **
 ** \param pstcEmac Pointer to EMAC instance
 **
 ** \retval Ok    Successful initialization
 **
 ******************************************************************************/
static en_result_t Emac_InitIrq( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac )
{
#if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)
  if (pstcEmac == &EMAC0)
  {
// in case FM3 MB9BD10S or MB9BD10T are used
// (shared interupt vector for MAC0/CAN1 and MAC1/CAN1)
#if defined(_MB9BD10S_H_) || defined(_MB9BD10T_H_)
    NVIC_ClearPendingIRQ(ETHER0_CAN0_IRQn);
    NVIC_SetPriority(ETHER0_CAN0_IRQn, PDL_IRQ_LEVEL_EMAC0);
#elif defined(__S6E2CC_H__) // in case FM4 S6E2CC is used
    NVIC_ClearPendingIRQ(ETHER0_IRQn);
    NVIC_SetPriority(ETHER0_IRQn, PDL_IRQ_LEVEL_EMAC0);
#else
    NVIC_ClearPendingIRQ(ETHER_MAC0_IRQn);
    NVIC_SetPriority(ETHER_MAC0_IRQn, PDL_IRQ_LEVEL_EMAC0);
#endif
    pstcEmac->IER_f.NIE = 1;
    pstcEmac->IER_f.RIE = 1;
    pstcEmac->IER_f.TIE = 1;
// in case FM3 MB9BD10S or MB9BD10T are used
// (shared interupt vector for MAC0/CAN1 and MAC1/CAN1)
#if defined(_MB9BD10S_H_) || defined(_MB9BD10T_H_)
    NVIC_EnableIRQ(ETHER0_CAN0_IRQn);
#elif defined(__S6E2CC_H__)
    NVIC_EnableIRQ(ETHER0_IRQn);
#else
    NVIC_EnableIRQ(ETHER_MAC0_IRQn);
#endif
    /* mask all GMAC MMC Interrupt.*/
//    pstcEmac->mmc_intr_mask_rx = 0xFFFFFFFF;
//    pstcEmac->mmc_intr_mask_tx = 0xFFFFFFFF;
//    pstcEmac->mmc_ipc_intr_mask_rx = 0xFFFFFFFF;

  }
#endif
#if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
  if (pstcEmac == &EMAC1)
  {
// in case FM3 MB9BD10S or MB9BD10T are used
// (shared interupt vector for MAC0/CAN1 and MAC1/CAN1)
#if defined(_MB9BD10S_H_) || defined(_MB9BD10T_H_)
    NVIC_ClearPendingIRQ(ETHER1_CAN1_IRQn);
    NVIC_SetPriority(ETHER1_CAN1_IRQn, PDL_IRQ_LEVEL_EMAC0);//
#else
    NVIC_ClearPendingIRQ(ETHER_MAC1_IRQn);
    NVIC_SetPriority(ETHER_MAC1_IRQn, PDL_IRQ_LEVEL_EMAC1);
#endif
    pstcEmac->stcIER.NIE = 1;
    pstcEmac->stcIER.RIE = 1;
    pstcEmac->stcIER.TIE = 1;
// in case FM3 MB9BD10S or MB9BD10T are used
// (shared interupt vector for MAC0/CAN1 and MAC1/CAN1)
#if defined(_MB9BD10S_H_) || defined(_MB9BD10T_H_)
    NVIC_EnableIRQ(ETHER1_CAN1_IRQn);
#else
    NVIC_EnableIRQ(ETHER_MAC1_IRQn);
#endif
    /* mask all GMAC MMC Interrupt.*/
//    pstcEmac->mmc_intr_mask_rx = 0xFFFFFFFF;
//    pstcEmac->mmc_intr_mask_tx = 0xFFFFFFFF;
//    pstcEmac->mmc_ipc_intr_mask_rx = 0xFFFFFFFF;

  }
#endif
  return Ok;
}
#endif //(EMAC_INTERRUPT_MODE == PDL_ON)

/**
 ******************************************************************************
 ** \brief Device dependent de-initialization of interrupts according CMSIS
 **
 ** \param pstcEmac Pointer to EMAC instance
 **
 ** \retval Ok    Successful de-initialization
 **
 ******************************************************************************/
static en_result_t Emac_DeInitIrq( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac )
{
#if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)
  if (pstcEmac == &EMAC0)
  {
// in case MB9BD10S or MB9BD10T are used (shared interupt vector for
// MAC0/CAN1 and MAC1/CAN1)
#if defined(IRQ_ETHER0_CAN0_AVAILABLE)
    NVIC_DisableIRQ(ETHER0_CAN0_IRQn);
    NVIC_ClearPendingIRQ(ETHER0_CAN0_IRQn);
    NVIC_SetPriority(ETHER0_CAN0_IRQn, PDL_DEFAULT_INTERRUPT_LEVEL);

#elif defined(IRQ_ETHER0_AVAILABLE)
    NVIC_DisableIRQ(ETHER0_IRQn);
    NVIC_ClearPendingIRQ(ETHER0_IRQn);
    NVIC_SetPriority(ETHER0_IRQn, PDL_DEFAULT_INTERRUPT_LEVEL);
#else
    #error ETHER0 interrupt not defined in MCU header file
#endif

    pstcEmac->MCR_f.TE = 0;
    pstcEmac->MCR_f.RE = 0;
    pstcEmac->IER_f.NIE = 0;
    pstcEmac->IER_f.RIE = 0;
    pstcEmac->IER_f.TIE = 0;
  }
#endif
#if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
  if (pstcEmac == &EMAC1)
  {
// in case MB9BD10S or MB9BD10T are used (shared interupt vector for
// MAC0/CAN1 and MAC1/CAN1)
#if defined(IRQ_ETHER1_CAN1_AVAILABLE)
    NVIC_DisableIRQ(ETHER1_CAN1_IRQn);
    NVIC_ClearPendingIRQ(ETHER1_CAN1_IRQn);
    NVIC_SetPriority(ETHER1_CAN1_IRQn, PDL_DEFAULT_INTERRUPT_LEVEL);
#elif defined(IRQ_ETHER1_AVAILABLE)
    NVIC_DisableIRQ(ETHER1_IRQn);
    NVIC_ClearPendingIRQ(ETHER1_IRQn);
    NVIC_SetPriority(ETHER1_IRQn, PDL_DEFAULT_INTERRUPT_LEVEL);
#else
    #error ETHER1 interrupt not defined in MCU header file
#endif

    pstcEmac->MCR_f.TE = 0;
    pstcEmac->MCR_f.RE = 0;
    pstcEmac->IER_f.NIE = 0;
    pstcEmac->IER_f.RIE = 0;
    pstcEmac->IER_f.TIE = 0;
  }
#endif
  return Ok;
}

#if EMAC_ECOUT
 /**
 ******************************************************************************
 ** \brief Configure port pin E_COUT to generate Ethernet clock for external PHY
 **
 ** This function configures the FMx internal USB/Ethernet clock module and
 ** the E_COUT pin function.
 **
 ** \return Ok    Successful configuration
 **
 ******************************************************************************/
en_result_t Emac_SetEcout(void)
{
  UsbEthernetClock_Init();

  /* configure E_COUT PIN */
  FMx_GPIO->EPFR14 |= (1 << 26); /* [bit26] E_CKE: E_COUT Output Select Bit */
  FMx_GPIO->PFRC |= (1 << 0x0B); /* PCB : E_COUT */

  return Ok;
}
#endif

#if (EMAC_INTERRUPT_MODE == PDL_ON)
/**
 *****************************************************************************
 ** \brief ISR callback for EMAC 0 & 1
 **
 ** These callbacks are called by the global EMAC ISR whenever an EMAC triggers
 ** an interrupt after initialization.
 **
 ** The active interrupt request flags are cleared by the ISR
 **
 ** \param [in] pstcEmac             Ethernet Mac instance
 ** \param [in] pstcEmacInternData   Internal data associated with the EMAC
 **                                  instance
 *****************************************************************************/
void EmacIrqHandler( volatile FM_ETHERNET_MAC_TypeDef*   pstcEmac,
                    stc_emac_intern_data_t* pstcEmacInternData )
{
  volatile stc_ethernet_mac_sr_field_t stcMySR = pstcEmac->SR_f;
  // Clear IRQ flags as early as possible and evaluate RI and TI later.
  // That way execution time of callbacks don't lead to missing frames
  pstcEmac->SR_f.NIS = 1;

  if(stcMySR.RI != 0)
  {
    if(pstcEmacInternData->pfnRxCallback != NULL)
    {
      pstcEmacInternData->pfnRxCallback();
    }
  }

  if(stcMySR.TI != 0)
  {
    if(pstcEmacInternData->pfnTxCallback != NULL)
    {
      pstcEmacInternData->pfnTxCallback();
    }
  }

  if(pstcEmac->SR_f.AIS == 1)
  {
    pstcEmac->SR_f.AIS = 1;
    if(pstcEmacInternData->pfnErrorCallback != NULL)
    {
      pstcEmacInternData->pfnErrorCallback();
    }
  }
} // EmacIrqHandler0
#endif


/**
 *****************************************************************************
 ** \brief Initialize EMAC Instance
 **
 ** This function initializes an EMAC instance and sets up the internal
 ** data structures
 **
 ** \param [in]  pstcEmac         Ethernet Mac instance
 ** \param [in]  pstcConfig       Ethernet Mac instance configuration
 **
 ** \retval Ok                    Internal data has been prepared
 ** \retval ErrorInvalidParameter pstcEmac == NULL,
 **                               pstcEmacInternData == NULL
 *****************************************************************************/
en_result_t Emac_Init( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                       stc_emac_config_t*    pstcConfig )
{
  // Pointer to internal data
  stc_emac_intern_data_t* pstcEmacInternData;
  volatile uint32_t u32AhbStatus;
  volatile uint32_t u32MdcSpeed;


  // Check for NULL pointer
  if ( pstcEmac == NULL )
  {
    return ErrorInvalidParameter ;
  }

  // Get pointer to internal data structure ...
  pstcEmacInternData = EmacGetInternDataPtr(pstcEmac);
  // ... and check for NULL
  if (pstcEmacInternData == NULL)
  {
    return ErrorInvalidParameter;
  }

    pstcEmacInternData->pfnRxCallback = pstcConfig->pfnRxCallback;

    pstcEmacInternData->pfnTxCallback = pstcConfig->pfnTxCallback;

    pstcEmacInternData->pfnErrorCallback = pstcConfig->pfnErrorCallback;

    if(pstcEmac == &EMAC0)
    {
#if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)
    pstcEmacInternData->pstcManagementBus = &EMAC0_MANAGEMENTBUS; //(MDC, MDIO)
    pstcEmacInternData->pstcTxDescChainHead = astcEmac0TxDescChain;
    pstcEmacInternData->pstcRxDescChainHead = astcEmac0RxDescChain;
    pstcEmacInternData->pstcTxDescCurrent   = astcEmac0TxDescChain;
    pstcEmacInternData->pstcRxDescCurrent   = astcEmac0RxDescChain;
    pstcEmacInternData->au8MacAddress[0] = pstcConfig->au8MacAddress[0];
    pstcEmacInternData->au8MacAddress[1] = pstcConfig->au8MacAddress[1];
    pstcEmacInternData->au8MacAddress[2] = pstcConfig->au8MacAddress[2];
    pstcEmacInternData->au8MacAddress[3] = pstcConfig->au8MacAddress[3];
    pstcEmacInternData->au8MacAddress[4] = pstcConfig->au8MacAddress[4];
    pstcEmacInternData->au8MacAddress[5] = pstcConfig->au8MacAddress[5];
    pstcEmacInternData->bLinkConfigured = 0;
    pstcEmacInternData->bPromiscuousMode = pstcConfig->bPromiscuousMode;
#else
    return ErrorInvalidParameter ;
#endif
    }
    else if(pstcEmac == &EMAC1)
    {
#if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
    pstcEmacInternData->pstcManagementBus = &EMAC1_MANAGEMENTBUS; //(MDC, MDIO)
    pstcEmacInternData->pstcTxDescChainHead = astcEmac1TxDescChain;
    pstcEmacInternData->pstcRxDescChainHead = astcEmac1RxDescChain;
    pstcEmacInternData->pstcTxDescCurrent   = astcEmac1TxDescChain;
    pstcEmacInternData->pstcRxDescCurrent   = astcEmac1RxDescChain;
    pstcEmacInternData->au8MacAddress[0] = pstcConfig->au8MacAddress[0];
    pstcEmacInternData->au8MacAddress[1] = pstcConfig->au8MacAddress[1];
    pstcEmacInternData->au8MacAddress[2] = pstcConfig->au8MacAddress[2];
    pstcEmacInternData->au8MacAddress[3] = pstcConfig->au8MacAddress[3];
    pstcEmacInternData->au8MacAddress[4] = pstcConfig->au8MacAddress[4];
    pstcEmacInternData->au8MacAddress[5] = pstcConfig->au8MacAddress[5];
    pstcEmacInternData->bLinkConfigured = 0;
    pstcEmacInternData->bPromiscuousMode = pstcConfig->bPromiscuousMode;
#else
    return ErrorInvalidParameter ;
#endif
    }
    else
    {
      return ErrorInvalidParameter ;
    }


  // Initialize EMAC  - this must be the first step when working with this Ethernet MAC!//////////////////////////////
  // MB9BF210T_610T-MN706-00015-1v0-E.pdf, p.20
  if (pstcEmac == &EMAC0)
  {
    EMAC_CONTROL->ETH_CLKG_f.MACEN0 = 1; //start clock supply to Ethernet-MAC0

#if (EMAC_PHYINTERFACE_RMII == PDL_ON)
    EMAC_CONTROL->ETH_MODE_f.IFMODE = 1; // select RMII
#else
    EMAC_CONTROL->ETH_MODE_f.IFMODE = 0; // select MII
#endif

    EMAC_CONTROL->ETH_MODE_f.RST0 = 1; // reset MAC0

    // At this point, clock signal (REF_CLK) must be input from external PHY.
    // If the clock signal has not been input, wait until it is input.


    EMAC_CONTROL->ETH_MODE_f.RST0 = 0;
  }
#if (EMAC_PHYINTERFACE_RMII == PDL_ON)
  else if (pstcEmac == &EMAC1)
  {
    //Hardware design demands clock supply for MAC0 also if only MAC1 is used
    if (0 == EMAC_CONTROL->ETH_CLKG_f.MACEN0)
    { //MAC0 has no clock supply yet. If MAC 0 is already initialized, do nothing here
      EMAC_CONTROL->ETH_CLKG_f.MACEN0 = 1; // start clock supply to Ethernet-MAC0
      EMAC_CONTROL->ETH_MODE_f.RST0   = 1; // reset MAC0 and keep it that way.
    }
    EMAC_CONTROL->ETH_CLKG_f.MACEN1 = 1; // start clock supply to Ethernet-MAC1
    EMAC_CONTROL->ETH_MODE_f.IFMODE = 1; // select RMII
    EMAC_CONTROL->ETH_MODE_f.RST1   = 1; // reset MAC1

    // At this point, clock signal (REF_CLK) must be input from external PHY.
    // If the clock signal has not been input, wait until it is input.

    EMAC_CONTROL->ETH_MODE_f.RST1 = 0;

  }
#endif
  else
  {
    //Error! MACnumber does neither designate MAC0 nor MAC1 or MAC1 was tried to address in MII mode
    return ErrorInvalidParameter;
  }




  // Initialize DMA controller, descriptors and buffers /////////////////////////////////////
  // Software reset the DMAC
  pstcEmac->BMR_f.SWR = 1;
  while (1 == pstcEmac->BMR_f.SWR);   // wait for reset being finished
  while (1 == pstcEmac->AHBSR_f.AHBS);   // wait for all ongoing AHB transactions to be completed

  // Port Select - Always set this register
  pstcEmac->MCR = 0;
  pstcEmac->MCR_f.PS = 1;

  // Configure bits for Mixed Burst mode
  pstcEmac->BMR_f.MB = 1;
  pstcEmac->BMR_f.AAL = 0;
  pstcEmac->BMR_f.FB = 0;

  //pstcEmac->BMR_f.DA = 1; // DMA Arbitration Scheme: 0: round robin, 1: fixed priority
  //pstcEmac->BMR_f.TXPR = 1; // TX has priority over RX

  // Prepare descriptors for DMAC
  Emac_InitTxDescChain(pstcEmac);
  Emac_InitRxDescChain(pstcEmac);


  // Make descriptor lists known to DMAC
  pstcEmac->RDLAR = (uint32_t) pstcEmacInternData->pstcRxDescChainHead; // RDLAR: Receive Descriptor List Address Register, DMA Register 3
  pstcEmac->TDLAR = (uint32_t) pstcEmacInternData->pstcTxDescChainHead; // TDLAR: Transmit Descriptor List Address Register, DMA Register 4

  // Finish DMA initialization by programming OMR (Operation Mode Register)
  pstcEmac->OMR = ((1<<20)/*FTF -- Flush Transmit FIFO*/
                  |(1<<21)/*TSF -- Transmit Store and Forward*/
                  |(1<<25)/*RSF -- Receive Store and Forward*/
                  );

  while (1 == pstcEmac->OMR_f.FTF); // wait until transmit FIFO is flushed.

  // Clear all DMA controller's interrupt requests
  pstcEmac->SR = 0x01E7FF;

  // Enable interrupts
#if (EMAC_INTERRUPT_MODE == PDL_ON)
  Emac_InitIrq(pstcEmac);
#endif

  // Make sure that AHB is free
	while (1 == pstcEmac->AHBSR_f.AHBS);   // wait for all ongoing AHB transactions to be completed

  // Start the Receive and Transmit DMA
	pstcEmac->OMR |= ((1<<13)/*ST*/
                   |(1<< 1)/*SR*/
                   );

 // DMA is initialized ////////////////////////////////////////////////////////////


 // Initialize GMAC ///////////////////////////////////////////////////////////////

  // Reset external PHY
  Ethphy_Reset(pstcEmac);

  // Set CR (Clock Range) bits in GAR (GMII Address register)
  // MDC Generation (SMI clock) - Ethernet-Manual, chapter 3.7: Station Management Agent

  u32MdcSpeed = pstcEmac->GAR;
  u32MdcSpeed &= ((uint32_t) ~(0xF<<2));

  if((__HCLK >= 60000000)&&(__HCLK < 100000000))
  {
    u32MdcSpeed |= (0<<2);
  }
  else if((__HCLK >= 100000000)&&(__HCLK < 150000000))
  {
    u32MdcSpeed |= (1<<2);
  }
  else if((__HCLK >= 20000000)&&(__HCLK < 35000000))
  {
    u32MdcSpeed |= (2<<2);
  }
  else if((__HCLK >= 35000000)&&(__HCLK < 60000000))
  {
    u32MdcSpeed |= (3<<2);
  }
  else if((__HCLK >= 150000000)&&(__HCLK < 250000000))
  {
    u32MdcSpeed |= (4<<2);
  }
  else if((__HCLK >= 250000000)&&(__HCLK <= 300000000))
  {
    u32MdcSpeed |= (5<<2);
  }
  else
	{
    while(1);//should not happen, as checked above (search for "sanity check"). If program hangs here, __HCLK setting is wrong
	}

  // Apply new clock settings
  pstcEmac->GAR = u32MdcSpeed;

  // Set MMC counter freeze bit. Prevents MMC module from locking-up the core when not used!
  pstcEmac->MMC_CNTL |= (1<<3);

  // Soft reset the PHY
  Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, (1<<ETHPHY_BCR_SOFT_RESET));

  // Write configuration into Basic Control Register
  Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, ((1<<ETHPHY_BCR_AUTO_NEGOTIATION_ENABLE)|(0<<ETHPHY_BCR_COLLISION_TEST)));
  // now, EMAC_Autonegotiate() can be called
  return Ok;

} // Emac_Init

/**
 *****************************************************************************
 ** \brief De-Initialize EMAC Instance
 **
 ** This function de-initializes an EMAC instance:
 ** Deinitialize Ethernet IRQs,
 ** put EMAC into reset condition,
 ** erase internal data
 **
 ** Please note that MAC1 can only work if MAC0 is sourced with a clock signal
 ** Therefore, Emac_DeInit(EMAC0) will put EMAC0 into reset state in any case,
 ** but turn off clock supply only if EMAC1 has been deactive. This can
 ** be done either by calling Emac_DeInit(EMAC1) before or by never calling
 ** Emac_Init(EMAC1) before.
 **
 ** Therefore, if you want to deinitialize both Ethernet channels,
 ** please call Emac_DeInit(EMAC1) first and Emac_DeInit(EMAC0) secondly.
 **
 **
 **
 ** \param [in]  pstcEmac         Ethernet Mac instance
 **
 ** \retval Ok                    Ethernet Mac de-initialized
 ** \retval ErrorInvalidParameter pstcEmac == NULL,
 **                               pstcEmacInternData == NULL
 *****************************************************************************/
en_result_t Emac_DeInit( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac )
{
  // Pointer to internal data
  stc_emac_intern_data_t* pstcEmacInternData;

  // Check for NULL pointer
  if ( pstcEmac == NULL )
  {
    return ErrorInvalidParameter;
  }

  // Get pointer to internal data structure ...
  pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );
  // ... and check for NULL
  if ( pstcEmacInternData == NULL )
  {
    return ErrorInvalidParameter;
  }

  // Deinitialize IRQs
  Emac_DeInitIrq(pstcEmac);


  if (pstcEmac == &EMAC0)
  {
    EMAC_CONTROL->ETH_MODE_f.RST0 = 1; // reset MAC0

    if (EMAC_CONTROL->ETH_CLKG_f.MACEN1 == 0)
    {
      EMAC_CONTROL->ETH_CLKG_f.MACEN0 = 0; //stop clock supply to Ethernet-MAC0 onlz if MAC1 is not active
    }
  }
#if (EMAC_PHYINTERFACE_RMII == PDL_ON)
  else if (pstcEmac == &EMAC1)
  {
    EMAC_CONTROL->ETH_MODE_f.RST1   = 1; // reset MAC1

    EMAC_CONTROL->ETH_CLKG_f.MACEN1 = 0; // stop clock supply to Ethernet-MAC1
  }
#endif
  else
  {
    //Error! MACnumber does neither designate MAC0 nor MAC1 or MAC1 was tried to address in MII mode
    return ErrorInvalidParameter;
  }


  // Erase internal data
    pstcEmacInternData->pstcManagementBus   = 0; //(MDC, MDIO)
    pstcEmacInternData->pstcTxDescChainHead = 0;
    pstcEmacInternData->pstcRxDescChainHead = 0;
    pstcEmacInternData->pstcTxDescCurrent   = 0;
    pstcEmacInternData->pstcRxDescCurrent   = 0;
    pstcEmacInternData->au8MacAddress[0]    = 0;
    pstcEmacInternData->au8MacAddress[1]    = 0;
    pstcEmacInternData->au8MacAddress[2]    = 0;
    pstcEmacInternData->au8MacAddress[3]    = 0;
    pstcEmacInternData->au8MacAddress[4]    = 0;
    pstcEmacInternData->au8MacAddress[5]    = 0;
    pstcEmacInternData->bLinkConfigured     = 0;
    pstcEmacInternData->bPromiscuousMode    = 0;
    pstcEmacInternData->pfnRxCallback       = 0;
    pstcEmacInternData->pfnTxCallback       = 0;
    pstcEmacInternData->pfnErrorCallback    = 0;

  return Ok;
} // Emac_DeInit



/**
 *****************************************************************************
 ** \brief Determine interface settings for speed and duplex mode
 **
 **
 **
 ** \param [in] pstcEmac     Ethernet Mac instance
 ** \retval enLinkMode       Any value of #en_emac_link_mode_t but not
 **                          EMAC_LinkModeAutonegotiation.
 *****************************************************************************/
en_emac_link_mode_t Emac_GetLinkMode(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  stc_ethernet_mac_mcr_field_t stcMcrConf;
  en_emac_link_mode_t enLinkMode;


  stcMcrConf = pstcEmac->MCR_f;

  if ((stcMcrConf.DM == 0) && (stcMcrConf.FES == 0))
	{
    enLinkMode = EMAC_LinkModeHalfDuplex10M;
	}
  else if ((stcMcrConf.DM == 0) && (stcMcrConf.FES == 1))
	{
    enLinkMode = EMAC_LinkModeHalfDuplex100M;
	}
  else if ((stcMcrConf.DM == 1) && (stcMcrConf.FES == 0))
	{
    enLinkMode = EMAC_LinkModeFullDuplex10M;
	}
  else // if ((stcMcrConf.DM == 1) && (stcMcrConf.FES == 1))
	{
    enLinkMode = EMAC_LinkModeFullDuplex100M;
	}

  return enLinkMode;
}

/**
 *****************************************************************************
 ** \brief  Select interface settings for speed and duplex mode manually
 **
 ** \param [in] pstcEmac    Ethernet Mac instance
 ** \param [in] enLinkMode  Any value of en_EthLinkMode_t
 **                         but not EMAC_LinkModeAutonegotiation.
 **
 ** \retval enLinkMode      See #en_emac_link_mode_t for details
 *****************************************************************************/
en_emac_link_mode_t Emac_SetLinkMode( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                      en_emac_link_mode_t   enLinkMode
                                    )
{
  stc_ethernet_mac_mcr_field_t stcMcrConf;
  PDL_ZERO_STRUCT(stcMcrConf);

  // Disable autonegotiation feature in PHY
  Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, ((0<<ETHPHY_BCR_AUTO_NEGOTIATION_ENABLE)|(0<<ETHPHY_BCR_COLLISION_TEST)));

  stcMcrConf.PS  = 1; // Port Select - Always set this register
  stcMcrConf.ACS = 1; // Automatic Pad/CRC Stripping

  switch (enLinkMode)
  {
  case (EMAC_LinkModeHalfDuplex10M):
    stcMcrConf.DM  = 0;
    stcMcrConf.FES = 0;
    Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, ((0<<ETHPHY_BCR_SPEED_SELECT)|(0<<ETHPHY_BCR_DUPLEX_MODE)));
    break;
  case (EMAC_LinkModeFullDuplex10M):
    stcMcrConf.DM  = 1;
    stcMcrConf.FES = 0;
    Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, ((0<<ETHPHY_BCR_SPEED_SELECT)|(1<<ETHPHY_BCR_DUPLEX_MODE)));
    break;
  case (EMAC_LinkModeHalfDuplex100M):
    stcMcrConf.DM  = 0;
    stcMcrConf.FES = 1;
    Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, ((1<<ETHPHY_BCR_SPEED_SELECT)|(0<<ETHPHY_BCR_DUPLEX_MODE)));
    break;
  case (EMAC_LinkModeFullDuplex100M):
    stcMcrConf.DM  = 1;
    stcMcrConf.FES = 1;
    Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, ((1<<ETHPHY_BCR_SPEED_SELECT)|(1<<ETHPHY_BCR_DUPLEX_MODE)));
    break;
  default:
    stcMcrConf.DM  = 1;
    stcMcrConf.FES = 1;
    Ethphy_Write(pstcEmac, ETHPHY_BASIC_CONTROL_REGISTER, ((1<<ETHPHY_BCR_AUTO_NEGOTIATION_ENABLE)|(1<<ETHPHY_BCR_RESTART_AUTO_NEGOTIATE)));
    break;
  }
  EmacGetInternDataPtr(pstcEmac)->bLinkConfigured = 1;
  pstcEmac->MCR_f = stcMcrConf;
  Emac_SetLinkUp(pstcEmac);
  return enLinkMode;
}


/**
******************************************************************************
** \brief  Read out link status flags from PHY to determine if link is up or down
**
** \param[in]  pstcEmac      Handle of EMAC being used
**
** \retval     enLinkStatus  See #en_emac_link_status_t for details
**
******************************************************************************/
en_emac_link_status_t Emac_GetLinkStatus(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  // Set up MCR (MAC Configuration Register) with status after auto-negotiation
  uint32_t u32Status;
  en_emac_link_status_t enLinkStatus;

  u32Status = Ethphy_Read(pstcEmac, ETHPHY_BASIC_STATUS_REGISTER);

  // Link is up and MAC has been configured accordingly
  if (u32Status & (1 << ETHPHY_BSR_LINK_STATUS))
  {
      enLinkStatus = EMAC_LinkStatusLinkUp;
  }
  else
  {
    enLinkStatus = EMAC_LinkStatusLinkDown;
  }

  return enLinkStatus;
}

/**
******************************************************************************
** \brief  Set registers for MAC address, filter blocks and rx/tx enable
**
** This function is used by Emac_Autonegotiate() and Emac_SetLinkMode().
** Usually it does not have to be called elsewhere.
**
** \param[in]  pstcEmac      Handle of EMAC being used
**
** \retval     enLinkStatus  See #en_emac_link_status_t for details
**
******************************************************************************/
en_result_t Emac_SetLinkUp(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  // Pointer to internal data
  stc_emac_intern_data_t* pstcEmacInternData;
  stc_ethernet_mac_mcr_field_t stcMcrConf;

  // provide default settings for rx filter and flow control
  stc_ethernet_mac_mffr_field_t stcMffrConf;
  stc_ethernet_mac_fcr_field_t stcFcrConf;

  // Check for NULL pointer
  if ( pstcEmac == NULL )
  {
    return ErrorInvalidParameter;
  }

  // Get pointer to internal data structure ...
  pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );
  // ... and check for NULL
  if ( pstcEmacInternData == NULL )
  {
    return ErrorInvalidParameter;
  }

  PDL_ZERO_STRUCT(stcMffrConf);
  PDL_ZERO_STRUCT(stcFcrConf);

  // Setup MAC address (GMAC Registers 16 and 17)
  pstcEmac->MAR0H = (uint32_t)((1UL<<31) // MO bit must always be set
                              |((pstcEmacInternData->au8MacAddress[5]) <<  8)
                              |((pstcEmacInternData->au8MacAddress[4]) <<  0)
                              );
  pstcEmac->MAR0L = (uint32_t)(((pstcEmacInternData->au8MacAddress[3]) << 24)
                              |((pstcEmacInternData->au8MacAddress[2]) << 16)
                              |((pstcEmacInternData->au8MacAddress[1]) <<  8)
                              |((pstcEmacInternData->au8MacAddress[0]) <<  0)
                              );

  // program the hash filter registers ((GMAC registers 2, 3: MHTRH, MHTRL)
#if (EMAC_MULTICAST_FILTER == PDL_ON)
  pstcEmac->MAR1H |= (uint32_t)0x20000000;
  pstcEmac->MAR1H |= (uint32_t)0x80000000;

  pstcEmac->MAR1H = (uint32_t)(((MULTICAST_ADDRESS5) <<  8)
                              |((MULTICAST_ADDRESS4) <<  0)
                              );
  pstcEmac->MAR1L = (uint32_t)(((MULTICAST_ADDRESS3) << 24)
                              |((MULTICAST_ADDRESS2) << 16)
                              |((MULTICAST_ADDRESS1) <<  8)
                              |((MULTICAST_ADDRESS0) <<  0)
                              );
#endif
  // Program the filter for incoming frames (GMAC register 1: MFFR)
  //stcMffrConf.RA   = 1; // Receive All
  //stcMffrConf.HPF  = 1; // Hash or Perfect Filter
  //stcMffrConf.SAF  = 1; // Source Address Filter
  //stcMffrConf.SAIF = 1; // Source Address Inverse Filter
  //stcMffrConf.PCF0 = 1; // Pass Control Frames
  //stcMffrConf.PCF1 = 1; // Pass Control Frames
  //stcMffrConf.DB   = 1; // Disable Broadcast Frames
  //stcMffrConf.PM   = 1; // Pass All Multicast
  //stcMffrConf.DAIF = 1; // DA Inverse Filtering
  //stcMffrConf.HMC  = 1; // Hash Multicast
  //stcMffrConf.HUC  = 1; // Hash Unicast
  stcMffrConf.PR   = (0 != pstcEmacInternData->bPromiscuousMode); // Enable promiscuous mode
  pstcEmac->MFFR_f = stcMffrConf;

  // Program proper flow control (GMAC register 6: FCR)
  stcFcrConf.UP      = 1; // Unicast Pause Frame Detect Enable
  stcFcrConf.RFE     = 1; // Receive Flow Control Enable
  stcFcrConf.TFE     = 1; // Transmit Flow Control Enable
  stcFcrConf.FCB_BPA = 1; // Flow Control Busy / Backpressure Activate
  pstcEmac->FCR_f = stcFcrConf;

  // Activate Interrupts by setting the interrupt mask register
  ///\todo
  // Enable transmission and reception by setting the respective bits
  stcMcrConf =  pstcEmac->MCR_f;
  stcMcrConf.TE = 1; // Transmit enable
  stcMcrConf.RE = 1; // Receive enable
  pstcEmac->MCR_f = stcMcrConf;

  return Ok;
}

/**
******************************************************************************
** \brief  Read out PHY's autonegotiation flags and configure MAC unit accordingly
**
** This function configures link parameters (speed, duplex) automatically.
** If return value is EMAC_LinkStatusLinkUp, transmission and reception is enabled,
** so Emac_TxFrame() and Emac_RxFrame() can be used.
**
** You can change filter settings by calling EMAC_ConfigRxFilter() afterwards.
**
** Before this function, EMAC_MacInit() must be called.
**
** It is recommended to call this function regularly to react on disconnected and reconnected cables.
**
** \param [in] pstcEmac       Handle of EMAC being used
** \retval     enLinkStatus   See #en_emac_link_status_t for details
**
******************************************************************************/
en_emac_link_status_t Emac_Autonegotiate(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
    // Pointer to internal data
    stc_emac_intern_data_t* pstcEmacInternData ;

    // Set up MCR (MAC Configuration Register) with status after auto-negotiation
    volatile uint16_t u16BSR;
    //  volatile uint16_t u16SR;        /// \TODO: use vendor specific PHY registers to determine autonegotiation result
    volatile uint32_t u32Work;
    en_emac_link_status_t enAutoNegResult;
    stc_ethernet_mac_mcr_field_t stcMcrConf;
    PDL_ZERO_STRUCT(stcMcrConf);

    // Check for NULL pointer
    if (pstcEmac == NULL)
    {
        return EMAC_LinkStatusInvalidParameter;
    }

    // Get pointer to internal data structure ...
    pstcEmacInternData = EmacGetInternDataPtr(pstcEmac);
    // ... and check for NULL
    if (pstcEmacInternData == NULL)
    {
        return EMAC_LinkStatusInvalidParameter;
    }

    //  u16SR  = Ethphy_Read(pstcEmac, ETHPHY_STATUS_REGISTER);
    u16BSR = Ethphy_Read(pstcEmac, ETHPHY_BASIC_STATUS_REGISTER);

    // Link is up and MAC has been configured accordingly
    if ((u16BSR & (1 << ETHPHY_BSR_LINK_STATUS)) && ((pstcEmacInternData->bLinkConfigured) == 1))
    {
        enAutoNegResult = EMAC_LinkStatusLinkUp;
    }

    // MAC is configured but Link went down => clean up
    else if (!(u16BSR & (1<<ETHPHY_BSR_LINK_STATUS)) && ((pstcEmacInternData->bLinkConfigured) == 1))
    {
        enAutoNegResult = EMAC_LinkStatusLinkDown;
        pstcEmacInternData->bLinkConfigured = 0;
        stcMcrConf = pstcEmac->MCR_f;
        stcMcrConf.TE = 0; // Transmit enable bit
        stcMcrConf.RE = 0; // Receive enable bit
        pstcEmac->MCR_f = stcMcrConf;
    }

    // Link is up but MAC is not configured yet
    else if ((u16BSR & (1 << ETHPHY_BSR_LINK_STATUS)) && ((pstcEmacInternData->bLinkConfigured) == 0))
    {
        if (u16BSR & (1 << ETHPHY_BSR_AUTO_NEGOTIATE_ABILITY))
        {
            // Link is up and autonegotiation is possible and complete
            if (u16BSR & (1 << ETHPHY_BSR_AUTO_NEGOTIATE_COMPLETE))
            {
                en_emac_link_mode_t enLinkMode = EMAC_LinkModeAutonegotiation;
                enAutoNegResult = EMAC_LinkStatusAutonegotiationSuccessful;
                pstcEmacInternData->bLinkConfigured = 1;

                stcMcrConf.PS  = 1; // Port Select - Always set this register
                stcMcrConf.ACS = 1; // Automatic Pad/CRC Stripping

                // PHY specific function, must be adapted in emac_user.c according to your vendor's data sheet
                enLinkMode = EMAC_AUTONEG_FUNCTION(pstcEmac);

                if (EMAC_LinkModeHalfDuplex10M == enLinkMode)
                {
                    stcMcrConf.DM  = 0; // Duplex Mode
                    stcMcrConf.FES = 0; // Speed
                }

                if (EMAC_LinkModeFullDuplex10M == enLinkMode)
                {
                    stcMcrConf.DM  = 1; // Duplex Mode
                    stcMcrConf.FES = 0; // Speed
                }

                if (EMAC_LinkModeHalfDuplex100M == enLinkMode)
                {
                    stcMcrConf.DM  = 0; // Duplex Mode
                    stcMcrConf.FES = 1; // Speed
                }

                if (EMAC_LinkModeFullDuplex100M == enLinkMode)
                {
                    stcMcrConf.DM  = 1; // Duplex Mode
                    stcMcrConf.FES = 1; // Speed
                }

                // Write data optained from auto-negotiation into respective registers
                pstcEmac->MCR_f = stcMcrConf;
            }
            // Link is up and autonegotiation is possible but not complete
            else
            {
                enAutoNegResult = EMAC_LinkStatusAutonegotiationInProgress;
            }
        }
        else
        {
            enAutoNegResult = EMAC_LinkStatusAutonegotiationNotSupported;
            pstcEmacInternData->bLinkConfigured = 1;

            // set prudent settings
            stcMcrConf.DM  = 0; // half duplex mode
            stcMcrConf.FES = 0; // speed: 10 Mbit
            pstcEmac->MCR_f = stcMcrConf;
        }


        if ((pstcEmacInternData->bLinkConfigured) == 1)
        {
            Emac_SetLinkUp(pstcEmac);
        }
    }
    else if ( !(u16BSR & (1 << ETHPHY_BSR_LINK_STATUS) && ((pstcEmacInternData->bLinkConfigured)) == 0))
    {
        enAutoNegResult = EMAC_LinkStatusLinkDown;
    }

    else
    {
        enAutoNegResult = EMAC_LinkStatusUnknownError; // unexpected case, please check status, which contains the content of PHY's Basic Status Register
    }

    return enAutoNegResult;
}

/**
 *****************************************************************************
 ** \brief Transmit data per EMAC Instance
 **
 ** This function will transmit data to the PHY through DMA
 **
 ** \param [in]  pstcEmac             Ethernet Mac instance
 ** \param [in]  pu8Buf               Buffer address of data
 ** \param [in]  u16Len               Lengh of data to be sent
 **
 ** \retval Ok                        Data has been sent to DMA FIFO
 ** \retval ErrorInvalidParameter     pstcEmac == NULL,
 **                                   pstcEmacInternData == NULL,
 ** \retval ErrorOperationInProgress  Descriptor is owned by DMA
 *****************************************************************************/
 en_result_t Emac_TxFrame( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                           uint8_t*              pu8Buf,
                           uint16_t              u16Len
                         )
{
    stc_emac_intern_data_t* pstcEmacInternData;
    uint16_t u16Count;
    uint8_t* pu8CurrentTxBuf;

    // Check if EMAC is MAC0 or MAC1
    if ( pstcEmac == NULL )
    {
        return ErrorInvalidParameter;
    }
     // Get pointer to internal data structure ...
    pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );
    if ( pstcEmacInternData == NULL )
    {
        return ErrorInvalidParameter;
    }

    if (1 == (pstcEmacInternData->pstcTxDescCurrent->TDES0_f.OWN))//descriptor is owned by DMA
    {
        return ErrorOperationInProgress;
    }
    else
    {
      // Clear all DMA controller's interrupt requests
      //pstcEmac->SR = 0x01E7FF;

      // Copy the frame into memory pointed by the current ETHERNET DMA Tx descriptor
      pu8CurrentTxBuf = (uint8_t*)(pstcEmacInternData->pstcTxDescCurrent->TDES2_f.B1AP);
      for (u16Count = 0; u16Count < u16Len; ++u16Count)
      {
        pu8CurrentTxBuf[u16Count] = pu8Buf[u16Count];// Fill buffer with payload
      }

      // Set buffer size
      pstcEmacInternData->pstcTxDescCurrent->TDES1_f.TBS1 = u16Len & ((uint32_t)0x00001FFF) ;

#if ((EMAC_ENABLE_ICMP_CHECKSUM_BUG_WORKAROUND == PDL_ON) && (EMAC_COE_MODE == 0x03))
// ICMP checksum handling, clean ICMP checksum
// As this is a LwIP/uIP bug, it should better be fixed there.
// However, this work-around enables you to test these free open-source stacks.
    {
      uint16_t *u16IpType = (uint16_t *)(pstcEmacInternData->pstcTxDescCurrent->TDES2+12);
      // is IP ?
      if( *u16IpType == 0x0008 )
      {
        uint8_t *u8IcmpType = (uint8_t *)(pstcEmacInternData->pstcTxDescCurrent->TDES2 + 23);
        // is ICMP ?
        if( *u8IcmpType == 1 )
        {
          uint16_t *u16IcmpChecksum = (uint16_t *)(pstcEmacInternData->pstcTxDescCurrent->TDES2 + 36 );
          *u16IcmpChecksum = 0;
        }
      }
	}
#endif

      // Is this first and/or last frame?
      // here, both is the case because we accommodate a whole frame in one single buffer
#ifdef EMAC_BUFFERS_NOT_FRAGMENTED
      ///\todo set up for variable length buffers and scatter gather mechanism
      ///\todo if not, compare u16Len with TDES1.TBS1
      pstcEmacInternData->pstcTxDescCurrent->TDES0_f.FS = 1;
      pstcEmacInternData->pstcTxDescCurrent->TDES0_f.LS = 1;

      // Enable TX Completion Interrupt
      pstcEmacInternData->pstcTxDescCurrent->TDES0_f.IC = 1;
      // We are finished, buffer is prepared for transmission, give ownership to DMAC
      pstcEmacInternData->pstcTxDescCurrent->TDES0_f.OWN = 1;
#endif
      // Tx DMAC is in SUSPEND state. Issue a poll demand to put it into RUN state
      pstcEmac->TPDR = 0;

      // Set tx pointer to next descriptor
      pstcEmacInternData->pstcTxDescCurrent = pstcEmacInternData->pstcTxDescCurrent->TDES3;

      return Ok;
    }
}


/**
 *****************************************************************************
 ** \brief Transmit data per EMAC Instance
 **
 ** This function will transmit data to the PHY through DMA
 **
 ** This function is used only for special purposes like the software Ethernet switch.
 ** Usually using Emac_TxFrame() is safer to use as it copies the data to be transmitted
 ** into the driver's internal TX buffer, so the data buffer can be reused right after the function returns.
 **
 ** In contrast, Emac_TxFrameDirect() uses the EMAC DMA directly on the user provided buffer.
 ** Therefore the user has to ensure the buffer's content remains valid until the end of transmission.
 **
 ** That means the TX descriptors' buffer pointer remain modified after the function was used
 ** and Emac_TxFrame() will fail if not reinitialized. So, use only Emac_TxFrameDirect()
 ** or only Emac_TxFrame() for a given EMAC in a project but refrain from mixing them!
 **
 **
 ** \param [in]  pstcEmac             Ethernet Mac instance
 ** \param [in]  pu8Buf               Buffer address of data
 ** \param [in]  u16Len               Lengh of data to be sent
 **
 ** \retval Ok                        Data has been sent to DMA FIFO
 ** \retval ErrorInvalidParameter     pstcEmac == NULL,
 **                                   pstcEmacInternData == NULL,
 ** \retval ErrorOperationInProgress  Descriptor is (still) owned by DMA
 *****************************************************************************/
 en_result_t Emac_TxFrameDirect( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                 uint8_t*              pu8Buf,
                                 uint16_t              u16Len
                               )
{
  stc_emac_intern_data_t* pstcEmacInternData;

  // Check if EMAC is MAC0 or MAC1
  if ( pstcEmac == NULL )
  {
    return ErrorInvalidParameter;
  }
  // Get pointer to internal data structure ...
  pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );
  if ( pstcEmacInternData == NULL )
  {
    return ErrorInvalidParameter;
  }

  if (1 == (pstcEmacInternData->pstcTxDescCurrent->TDES0_f.OWN)) // Descriptor is owned by DMA
  {
    return ErrorOperationInProgress;
  }
  else
  {
    // Clear all DMA controller's interrupt requests
    //pstcEmac->SR = 0x01E7FF;

    // Set the current ETHERNET DMA Tx descriptor's buffer to user provided address
    pstcEmacInternData->pstcTxDescCurrent->TDES2_f.B1AP = pu8Buf;


    // Set buffer size
    pstcEmacInternData->pstcTxDescCurrent->TDES1_f.TBS1 = u16Len & ((uint32_t)0x00001FFF) ;

#if ((EMAC_ENABLE_ICMP_CHECKSUM_BUG_WORKAROUND == PDL_ON) && (EMAC_COE_MODE == 0x03))
    // ICMP checksum handling, clean ICMP checksum
    // As this is a LwIP/uIP bug, it should better be fixed there.
    // However, this work-around enables you to test these free open-source stacks.
    {
      uint16_t *u16IpType = (uint16_t *)(pstcEmacInternData->pstcTxDescCurrent->TDES2 + 12);
      // Is it IP?
      if( *u16IpType == 0x0008 )
      {
        uint8_t *u8IcmpType = (uint8_t *)(pstcEmacInternData->pstcTxDescCurrent->TDES2 + 23);
        // Is it ICMP?
        if( *u8IcmpType == 1 )
        {
          uint16_t *u16IcmpChecksum = (uint16_t *)(pstcEmacInternData->pstcTxDescCurrent->TDES2 + 36 );
          *u16IcmpChecksum = 0;
        }
      }
    }
#endif

    // Is this first and/or last frame?
    // here, both is the case because we accommodate a whole frame in one single buffer
#ifdef EMAC_BUFFERS_NOT_FRAGMENTED
    ///\todo set up for variable length buffers and scatter gather mechanism
    ///\todo if not, compare u16Len with TDES1.TBS1
    pstcEmacInternData->pstcTxDescCurrent->TDES0_f.FS = 1;
    pstcEmacInternData->pstcTxDescCurrent->TDES0_f.LS = 1;

    // Enable TX Completion Interrupt
    pstcEmacInternData->pstcTxDescCurrent->TDES0_f.IC = 1;
    // We are finished, buffer is prepared for transmission, give ownership to DMAC
    pstcEmacInternData->pstcTxDescCurrent->TDES0_f.OWN = 1;
#endif
    // Tx DMAC is in SUSPEND state. Issue a poll demand to put it into RUN state
    pstcEmac->TPDR = 0;

    // Set tx pointer to next descriptor
    pstcEmacInternData->pstcTxDescCurrent = pstcEmacInternData->pstcTxDescCurrent->TDES3;

    return Ok;
  }
}


/**
******************************************************************************
** \brief Check how many bytes are in rx buffer without taking them out
**
** This function can be used to determine how much memory has to be
** allocated if dynamic memory management is used.
**
** \param[in] pstcEmac    Handle of EMAC being used
** \return    length      Number of bytes received, 0 if no frame in queue
******************************************************************************/
uint16_t Emac_GetFrameLength(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  uint16_t u16Length;
  stc_emac_intern_data_t* pstcEmacInternData;

  // Check for NULL pointer
  if ( pstcEmac == NULL )
  {
    return 0 ;
  }
  // Get pointer to internal data structure ...
  pstcEmacInternData = EmacGetInternDataPtr( pstcEmac ) ;

  if ( pstcEmacInternData == NULL )
  {
    return 0 ;
  }
  //descriptor is owned by DMA => No frame arrived
  else if (1 == pstcEmacInternData->pstcRxDescCurrent->RDES0_f.OWN)
  {
    return 0 ;
  }
  else
  {
    u16Length = (pstcEmacInternData->pstcRxDescCurrent->RDES0_f.FL);
  }
  return u16Length;
}

/**
 *****************************************************************************
 ** \brief Receive data from MAC FIFO for  EMAC Instance
 **
 ** This function will receive and copy data to the buffer addressed by pu8Buf
 **
 ** \param [in]  pstcEmac     Ethernet Mac instance
 ** \param [in]  pu8Buf       Buffer address to store the data
 ** \retval      length       Length of data has been received
 ** \retval      0            Error of parameter
 *****************************************************************************/
 uint16_t Emac_RxFrame( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                        uint8_t * pu8Buf )
{
    stc_emac_intern_data_t* pstcEmacInternData;
    uint16_t u16Length;
    uint16_t u16Count;
    volatile uint8_t* pu8CurrentRxBuf;

    // Check for NULL pointer
    if ( pstcEmac == NULL )
    {
        return 0;
    }
     // Get pointer to internal data structure ...
    pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );
    if ( pstcEmacInternData == NULL )
    {
        return 0;
    }

    if (1 == pstcEmacInternData->pstcRxDescCurrent->RDES0_f.OWN)
    { // Descriptor still owned by DMAC => Nothing received
        return 0;
    }
    else
    {
        // Copy the frame into memory pointed by the current ETHERNET DMA Rx descriptor
        pu8CurrentRxBuf = (pstcEmacInternData->pstcRxDescCurrent->RDES2_f.B1AP);

//#ifdef EMAC_BUFFERS_NOT_FRAGMENTED
        // check the validity of the package
        //if (0 == (pstcEmacInternData->pstcRxDescCurrent->RDES0_f.ES))
        {
            u16Length = pstcEmacInternData->pstcRxDescCurrent->RDES0_f.FL;

            for (u16Count = 0; u16Count < u16Length; u16Count++)
            {
                pu8Buf[u16Count] = *(pu8CurrentRxBuf + u16Count);
            }
        }
        // Set Own bit of the Rx descriptor Status: gives the descriptor to ETHERNET DMA
        pstcEmacInternData->pstcRxDescCurrent->RDES0_f.OWN = 1;

        // Set rx pointer to next descriptor
        pstcEmacInternData->pstcRxDescCurrent = pstcEmacInternData->pstcRxDescCurrent->RDES3;
//#endif

        // issue poll command to wake DMAC up from suspend state
        pstcEmac->RPDR = 0;
    }
    return u16Length;
}

/**
 *****************************************************************************
 ** \brief Get buffer of received frame using DMA Receive interrupt,
 **        it allows scanning of Rx descriptors to get the frame
 **
 ** \param [in]  pstcEmac                 Ethernet Mac instance
 ** \retval      (void*)pu8CurrentRxBuf   Buffer address of the data stored
 ** \retval      NULL                     If one or more of the follwoing
 **                                       conditions are met:
 **                                       - pstcEmac == NULL
 **                                       - pstcEmacInternData == NULL
 **                                       - Nothing received
 *****************************************************************************/
void * Emac_RxFrame_GetBufPtr( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac )
{
  stc_emac_intern_data_t* pstcEmacInternData;
  static volatile uint8_t * pu8CurrentRxBuf;
  uint16_t u16Count = 0;

  // Check for NULL pointer
  if ( pstcEmac == NULL )
  {
    return NULL;
  }
  // Get pointer to internal data structure ...
  pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );
  if ( pstcEmacInternData == NULL )
  {
    return NULL;
  }

  if (1 == pstcEmacInternData->pstcRxDescCurrent->RDES0_f.OWN)// Descriptor still owned by DMAC => Nothing received
  {
    return NULL;
  }
  else
  {
    // Scan the descriptors owned by CPU
    while((!pstcEmacInternData->pstcRxDescCurrent->RDES0_f.OWN )&&(u16Count<EMAC0_RX_RING_SIZE))
    {
      u16Count++;

      if((pstcEmacInternData->pstcRxDescCurrent->RDES0_f.FS == 1) && \
        (pstcEmacInternData->pstcRxDescCurrent->RDES0_f.LS == 1))
      {
        pu8CurrentRxBuf = (pstcEmacInternData->pstcRxDescCurrent->RDES2);
        return (void*)pu8CurrentRxBuf;
      }
    }

  }
  return NULL;

}

/**
 *****************************************************************************
 ** \brief Update the status of the receive descriptor for further use
 **
 ** \param [in]  pstcEmac   Ethernet Mac instance
 *****************************************************************************/
void Emac_RxFrame_ReleaseBuf (volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
    stc_emac_intern_data_t* pstcEmacInternData;
    pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );// Get pointer to internal data structure ...

    // Set Own bit of the Rx descriptor Status: gives the descriptor to ETHERNET DMA
    pstcEmacInternData->pstcRxDescCurrent->RDES0_f.OWN = 1;
    // Set rx pointer to next descriptor
    pstcEmacInternData->pstcRxDescCurrent = pstcEmacInternData->pstcRxDescCurrent->RDES3;

    /* When Rx Buffer unavailable flag is set, clear it and resume reception */
    if(pstcEmac->SR_f.RU == 1)
    {
        pstcEmac->SR_f.RU = 0;
        pstcEmac->RPDR = 0;// issue poll command to wake DMAC up from suspend state
    }
}

#if ((EMAC0_BUFFERS_IN_DRIVERSPACE != PDL_ON) || (EMAC1_BUFFERS_IN_DRIVERSPACE != PDL_ON))
/**
 *****************************************************************************
 ** \brief If Tx buffers are defined in user space, this function is used to
 **
 ** \param [in]   pstcEmac            Ethernet Mac instance
 ** \param [in]   u16DescNumber       Descriptor number
 ** \param [in]   pu8UserBuf          Pointer to user buffer
 ** \param [in]   u16BufLength        Buffer length
 ** \retval   Ok                      Buffers set
 ** \retval   ErrorInvalidParameter   If one or more of the following conditions
 **                                   are met:
 **                                   - pstcEmac invalid
 **                                   - EMAC0_BUFFERS_IN_DRIVERSPACE == PDL_ON
 **                                   - EMAC1_BUFFERS_IN_DRIVERSPACE == PDL_ON
 **                                   - u16DescNumber >= u16RingSize
 **                                   - pu8UserBuf == NULL
 **                                   - u16BufLength = 0
 *****************************************************************************/
en_result_t Emac_SetDescToTxBuf( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                 uint16_t              u16DescNumber,
                                 volatile uint8_t *    pu8UserBuf,
                                 uint16_t              u16BufLength
                               )
{
  volatile stc_emac_dma_tx_desc_t *pastcCurrentTx;
  uint16_t u16RingSize;

  if (pstcEmac == &EMAC0)
  {
#if (EMAC0_BUFFERS_IN_DRIVERSPACE != PDL_ON)
    u16RingSize = EMAC0_TX_RING_SIZE;
    pastcCurrentTx = astcEmac0TxDescChain;
#else
    return ErrorInvalidParameter ;
#endif
  }
  else if (pstcEmac == &EMAC1)
  {
#if (EMAC1_BUFFERS_IN_DRIVERSPACE != PDL_ON)
    u16RingSize = EMAC1_TX_RING_SIZE;
    pastcCurrentTx = astcEmac1TxDescChain;
#else
  return ErrorInvalidParameter ;
#endif
  }
  else
  {
    return ErrorInvalidParameter;
  }

  if (u16DescNumber >= u16RingSize)
  {
    return ErrorInvalidParameter;
  }

  if (pu8UserBuf == NULL)
  {
    return ErrorInvalidParameter;
  }

  if ((u16BufLength == 0 ))
  {
    return ErrorInvalidParameter;
  }


  (pastcCurrentTx[u16DescNumber]).TDES1_f.TBS1 = u16BufLength;
  (pastcCurrentTx[u16DescNumber]).TDES2 = pu8UserBuf;

  return Ok;
}

/**
 *****************************************************************************
 ** \brief If Rx buffers are defined in user space, this function is used to
 **
 ** \param [in]   pstcEmac            Ethernet Mac instance
 ** \param [in]   u16DescNumber       Descriptor number
 ** \param [in]   pu8UserBuf          Pointer to user buffer
 ** \param [in]   u16BufLength        Buffer length
 ** \retval   Ok                      Buffers set
 ** \retval   ErrorInvalidParameter   If one or more of the following conditions
 **                                   are met:
 **                                   - pstcEmac invalid
 **                                   - EMAC0_BUFFERS_IN_DRIVERSPACE == PDL_ON
 **                                   - EMAC1_BUFFERS_IN_DRIVERSPACE == PDL_ON
 **                                   - u16DescNumber >= u16RingSize
 **                                   - pu8UserBuf == NULL
 **                                   - u16BufLength = 0
 *****************************************************************************/
en_result_t Emac_SetDescToRxBuf( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                 uint16_t              u16DescNumber,
                                 volatile uint8_t *    pu8UserBuf,
                                 uint16_t              u16BufLength
                               )
{
  volatile stc_emac_dma_rx_desc_t *pastcCurrentRx;
  uint16_t u16RingSize;

  if (pstcEmac == &EMAC0)
  {
#if (EMAC0_BUFFERS_IN_DRIVERSPACE != PDL_ON)
    u16RingSize = EMAC0_RX_RING_SIZE;
    pastcCurrentRx = astcEmac0RxDescChain;
#else
  return ErrorInvalidParameter ;
#endif
  }

  else if (pstcEmac == &EMAC1)
  {
#if (EMAC1_BUFFERS_IN_DRIVERSPACE != PDL_ON)
    u16RingSize = EMAC1_RX_RING_SIZE;
    pastcCurrentRx = astcEmac1RxDescChain;
#else
  return ErrorInvalidParameter ;
#endif
  }
  else
  {
    return ErrorInvalidParameter;
  }

  if (u16DescNumber >= u16RingSize)
  {
    return ErrorInvalidParameter;
  }

  if (pu8UserBuf == NULL)
  {
    return ErrorInvalidParameter;
  }

  if ((u16BufLength == 0 ))
  {
    return ErrorInvalidParameter;
  }

  (pastcCurrentRx[u16DescNumber]).RDES1_f.RBS1 = u16BufLength;
  (pastcCurrentRx[u16DescNumber]).RDES2 = pu8UserBuf;

  return Ok;
}


/**
 *****************************************************************************
 ** \brief Low-level function used for Ethernet switch functionality
 **
 ** Used to replace assign an empty rx buffer to an rx descriptor so that
 ** the received frame can be handled independently from EMAC state.
 **
 ** \param [in]   pstcEmac            Ethernet Mac instance of concern
 ** \param [in]   pu8UserBuf          Pointer to user buffer which is to be assigned to current RX descriptor
 ** \param [in]   u16BufLength        Length of user buffer
 ** \retval   Ok                      Buffers set
 ** \retval   ErrorInvalidParameter   If one or more of the following conditions
 **                                   are met:
 **                                   - pstcEmac invalid
 **                                   - EMAC0_BUFFERS_IN_DRIVERSPACE == PDL_ON
 **                                   - EMAC1_BUFFERS_IN_DRIVERSPACE == PDL_ON
 **                                   - u16DescNumber >= u16RingSize
 **                                   - pu8UserBuf == NULL
 **                                   - u16BufLength = 0
 *****************************************************************************/
en_result_t Emac_SetCurrentDescToRxBuf( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                        volatile uint8_t *    pu8UserBuf,
                                        uint16_t              u16BufLength
                                      )
{
  stc_emac_intern_data_t * pstcEmacInternData;

  if (pstcEmac == &EMAC0)
  {
#if (EMAC0_BUFFERS_IN_DRIVERSPACE != PDL_ON)
#else
    return ErrorInvalidParameter ;
#endif
  }

  else if (pstcEmac == &EMAC1)
  {
#if (EMAC1_BUFFERS_IN_DRIVERSPACE != PDL_ON)
#else
    return ErrorInvalidParameter ;
#endif
  }
  else
  {
    return ErrorInvalidParameter;
  }


  if (pu8UserBuf == NULL)
  {
    return ErrorInvalidParameter;
  }

  if ((u16BufLength == 0 ))
  {
    return ErrorInvalidParameter;
  }

  // Get pointer to internal data structure ...
  pstcEmacInternData = EmacGetInternDataPtr( pstcEmac );
  // ... and check for NULL
  if ( pstcEmacInternData == NULL )
  {
    return ErrorInvalidParameter;
  }

  // Set buffer pointer of current RX descriptor to user-provided buffer
  pstcEmacInternData->pstcRxDescCurrent->RDES2_f.B1AP = pu8UserBuf;
  pstcEmacInternData->pstcRxDescCurrent->RDES1_f.RBS1 = u16BufLength;

  /* Set Own bit of the Rx descriptor Status: gives the descriptor to ETHERNET DMA */
  pstcEmacInternData->pstcRxDescCurrent->RDES0_f.OWN = 1;

  // Set rx pointer to next descriptor
  pstcEmacInternData->pstcRxDescCurrent = pstcEmacInternData->pstcRxDescCurrent->RDES3;

  // issue poll command to wake DMAC up from suspend state
  pstcEmac->RPDR = 0;

  return Ok;
}
#endif //((EMAC0_BUFFERS_IN_DRIVERSPACE != PDL_ON) && (EMAC1_BUFFERS_IN_DRIVERSPACE != PDL_ON))


/**
 *****************************************************************************
 ** \brief Interneal function to set up a descriptor chain for transmitting frames
 **
 ** \param [in]    pstcEmac               Ethernet Mac instance
 ** \retval        Ok                     Sucessfully done
 ** \retval        ErrorInvalidParameter  pstcEmac invalid
 *****************************************************************************/
static en_result_t Emac_InitTxDescChain(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  //Parameters are already checked by caller function

  volatile stc_emac_dma_tx_desc_t *pstcTxChain = EmacGetInternDataPtr(pstcEmac)->pstcTxDescChainHead;
  volatile stc_emac_dma_tx_desc_t *pstcCurrentTx = pstcTxChain;
  uint16_t u16TxChainElement = 0;

  uint16_t u16RingSize;
  uint16_t u16BufSize;
  volatile uint8_t *pu8TxBuf = NULL;

  if (pstcEmac == &EMAC0)
  {
#if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)
    u16RingSize = EMAC0_TX_RING_SIZE;
    u16BufSize  = EMAC0_TX_BUF_SIZE;
#if (EMAC0_BUFFERS_IN_DRIVERSPACE == PDL_ON)
    pu8TxBuf = &au8Emac0TxBufs[0][0];
#endif
#else
    return ErrorInvalidParameter ;
#endif
  }
  else if (pstcEmac == &EMAC1)
  {
#if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
    u16RingSize = EMAC1_TX_RING_SIZE;
    u16BufSize  = EMAC1_TX_BUF_SIZE;
#if (EMAC1_BUFFERS_IN_DRIVERSPACE == PDL_ON)
    pu8TxBuf = &au8Emac1TxBufs[0][0];
#endif
#else
    return ErrorInvalidParameter ;
#endif
  }
  else
  {
    return ErrorInvalidParameter;
  }

  for (u16TxChainElement = 0; u16TxChainElement < u16RingSize; ++u16TxChainElement)
  {
    pstcCurrentTx = &pstcTxChain[u16TxChainElement];
    pstcCurrentTx->TDES0_f.OWN = 0; // Buffer must be filled with payload, then given to DMA engine
    pstcCurrentTx->TDES0_f.TCH = 1; // Chained descriptors -> Second address points to next descriptor
    pstcCurrentTx->TDES0_f.CIC = EMAC_COE_MODE; // Enable Checksum Offload Engine
    if (pu8TxBuf != NULL)
    {
      pstcCurrentTx->TDES1_f.TBS1 = u16BufSize;
      pstcCurrentTx->TDES2 = &(pu8TxBuf[u16TxChainElement * u16BufSize]);
    }
    // If (pu8TxBuf == NULL), user must have set TDES2 and TDES1_f.TBS1 first, so it must not be overwritten here!
    pstcCurrentTx->TDES3 = &pstcTxChain[u16TxChainElement+1];
  }

  // Last descriptor in list
    pstcCurrentTx->TDES0_f.TER = 1;
    pstcCurrentTx->TDES3 = pstcTxChain; //Ring structure: last element points to first

  return Ok;
}

/**
 *****************************************************************************
 ** \brief Interneal function to set up a descriptor chain for receiving frames
 **
 ** \param [in]    pstcEmac                 Ethernet Mac instance
 ** \retval        Ok                       Successfully done
 ** \retval        ErrorInvalidParameter    pstcEmac invalid
 *****************************************************************************/
static en_result_t Emac_InitRxDescChain(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  //Parameters are already checked by caller function

  volatile stc_emac_dma_rx_desc_t *pstcRxChain = EmacGetInternDataPtr(pstcEmac)->pstcRxDescChainHead;
  volatile stc_emac_dma_rx_desc_t *pstcCurrentRx = pstcRxChain;
  uint16_t u16RxChainElement = 0;

  uint16_t u16RingSize;
  uint16_t u16BufSize;
  volatile uint8_t *pu8RxBuf = NULL;

  if (pstcEmac == &EMAC0)
  {
#if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)
    u16RingSize = EMAC0_RX_RING_SIZE;
    u16BufSize  = EMAC0_RX_BUF_SIZE;
#if (EMAC0_BUFFERS_IN_DRIVERSPACE == PDL_ON)
    pu8RxBuf = &au8Emac0RxBufs[0][0];
#endif
#else
    return ErrorInvalidParameter ;
#endif
  }
  else if (pstcEmac == &EMAC1)
  {
#if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
    u16RingSize = EMAC1_RX_RING_SIZE;
    u16BufSize  = EMAC1_RX_BUF_SIZE;
#if (EMAC1_BUFFERS_IN_DRIVERSPACE == PDL_ON)
    pu8RxBuf = &au8Emac1RxBufs[0][0];
#endif
#else
    return ErrorInvalidParameter ;
#endif
  }
  else
  {
    return ErrorInvalidParameter;
  }

  for (u16RxChainElement = 0; u16RxChainElement < u16RingSize; ++u16RxChainElement)
  {
    pstcCurrentRx = &pstcRxChain[u16RxChainElement];
    pstcCurrentRx->RDES0_f.OWN = 1; // Receiving DMA controller must own descriptor before data can be received.
    pstcCurrentRx->RDES1_f.RCH = 1; // Chained descriptors -> Second address points to next descriptor
    if (pu8RxBuf != NULL)
    {
      pstcCurrentRx->RDES1_f.RBS1 = u16BufSize;
      pstcCurrentRx->RDES2 = &(pu8RxBuf[u16RxChainElement * u16BufSize]);
    }
    // If pu8RxBuf == NULL, Buffers are in user space, so nothing is touched here.
    // User must have written valid buffer address in RDES2 and its length in RDES1_f.RBS1 first.
    pstcCurrentRx->RDES3 = &pstcRxChain[u16RxChainElement+1];
  }

  // Last descriptor in list:
  pstcCurrentRx->RDES1_f.RER = 1;
  pstcCurrentRx->RDES3 = pstcRxChain; //Ring structure: last element points to first

  return Ok;
}

//@} // EmacGroup


/**
 ******************************************************************************
 ** \addtogroup EthphyGroup
 ******************************************************************************/
//@{

/**
 *****************************************************************************
 ** \brief Read from PHY register
 **
 ** This function reads out a PHY register
 **
 ** \param [in]  pstcEmac       Ethernet MAC instance
 ** \param [in]  u8PhyReg       Ethernet PHY register address
 **
 ** \return uint16_t            Register content
 *****************************************************************************/
uint16_t Ethphy_Read( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                      uint8_t               u8PhyReg
                    )
{
  volatile uint32_t u32Timeout = 0;
  uint16_t u16PhyContent = 0;
  uint8_t u8PhyAddress;
  uint32_t u32Work = 0;
  if (pstcEmac == &EMAC0)
  {
    u8PhyAddress = EMAC0_PHY_ADDRESS;
  }
  else if (pstcEmac == &EMAC1)
  {
    u8PhyAddress = EMAC1_PHY_ADDRESS;
  }
  else // Error: Neither MAC 0 nor MAC 1 selected!
  {
    return ETHPHY_ERROR_INVALID_ADDRESS;
  }

  // GAR (GMII Address register, GMAC Register 4)

  u32Work = pstcEmac->GAR;
  // Safe the clock range bits (CR[3:0])
  u32Work &= (0xF << 2);

  // Select the physical device with the PA bits (Physical Address)
  u32Work |= (u8PhyAddress << 11);
  // Select register on PHY by setting GR bits (GMII Register)
  u32Work |= (u8PhyReg  <<  6);
  // Set read mode (GW/GMII Write = 0)
  u32Work &= ~(1u << 1);

  // Set busy flag
  u32Work |= 1;

  // apply settings
  pstcEmac->GAR = u32Work;

  //is transmission still running?
  u32Timeout = 0;
  do{
    u32Timeout++;
    u32Work = pstcEmac->GAR;
    if (u32Timeout >= ETHPHY_TIMEOUT)
		{
      break;
		}
  } while (u32Work & 1);

  if (ETHPHY_TIMEOUT == u32Timeout)
	{
    return ETHPHY_ERROR_TIMEOUT;
	}
  else
	{
    u16PhyContent = (pstcEmac->GDR);
	}
  return u16PhyContent;
}

/**
 *****************************************************************************
 ** \brief Write to PHY register
 **
 ** This function writes data to a PHY register
 **
 ** \param [in]  pstcEmac       Ethernet MAC instance
 ** \param [in]  u8PhyReg       Ethernet PHY register address
 ** \param [in]  u16Val         Ethernet PHY register data to write
 **
 ** \retval Ok                      Value has been written into the PHY
 ** \retval ErrorInvalidParameter   pstcEmac invalid
 *****************************************************************************/
en_result_t Ethphy_Write( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                          uint8_t               u8PhyReg,
                          uint16_t              u16Val
                        )
{
  volatile uint32_t u32Timeout = 0;
  uint8_t u8PhyAddress;
  uint32_t u32Work = 0;

  if (pstcEmac == &EMAC0)
  {
    u8PhyAddress = EMAC0_PHY_ADDRESS;
  }
  else if (pstcEmac == &EMAC1)
  {
    u8PhyAddress = EMAC1_PHY_ADDRESS;
  }
  else // Error: Neither MAC 0 nor MAC 1 selected!
  {
    return ErrorInvalidParameter;
  }

  // get GAR (GMII Address register, GMAC Register 4)
  u32Work = pstcEmac->GAR;
  // Keep the clock range bits (CR[3:0])
  u32Work &= (0xF << 2);

  // Select the physical device with the PA bits (Physical Address)
  u32Work |= (u8PhyAddress << 11);
  // Select register on PHY by setting GR bits (GMII Register)
  u32Work |= (u8PhyReg <<  6);
  // Set write mode (GW/GMII Write = 1)
  u32Work |= (1 << 1);
  // Set busy flag
  u32Work |= 1;

  // Write data to send in data register
  pstcEmac->GDR = (u16Val & 0xFFFF);

  // send message
  pstcEmac->GAR = u32Work;

  /* is GMII still busy? */
  do{
    u32Work = pstcEmac->GAR;
  }while (u32Work & 1);

  return Ok;
}

/**
 *****************************************************************************
 ** \brief Reset PHY
 **
 ** This function resets the PHY via the GPIO pin defined by
 ** EMAC0_PHY_RESET_PIN or EMAC1_PHY_RESET_PIN.
 **
 ** \param [in]  pstcEmac       Ethernet MAC instance
 **
 ** \retval Ok                      PHY reset sequence has been conducted
 ** \retval ErrorInvalidParameter   pstcEmac invalid
 *****************************************************************************/
en_result_t Ethphy_Reset(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
  volatile uint32_t u32Timer100usec;
  uint32_t * pu32PhyResetPin;

  if (pstcEmac == &EMAC0)
  {
    pu32PhyResetPin = EMAC0_PHY_RESET_PIN;
  }
  else if (pstcEmac == &EMAC1)
  {
#if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
    pu32PhyResetPin = EMAC1_PHY_RESET_PIN;
#endif
  }
  else
  {
    return ErrorInvalidParameter;
  }

  *pu32PhyResetPin = 0;

  //PHY's nRST must be low for at least 100 usec.
  u32Timer100usec = ((__HCLK / 1000000) * 100) ; // e.g. 144 MHz * 100 usec = 14400
  for ( ; u32Timer100usec > 0; --u32Timer100usec);

  *pu32PhyResetPin = 1;

  return Ok;
}






//@} // EthphyGroup


#endif // #if (defined(PDL_PERIPHERAL_EMAC_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

