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
/** \file emac.h
 **
 ** Headerfile for EMAC functions
 **
 **
 ** History:
 **   - 2012-11-20  1.0  Fujitsu    First version.
 **   - 2012-11-23  1.1  CNo, MWi   Improved documentation and L3 compatibility
 **   - 2012-12-10  1.1a QXu,CNo    Work-around for LwIP and uIP ICMP echo bug
 **   - 2012-12-18  1.2  YMo, CNo   Support for multicast, bugfixes
 **   - 2013-08-09  1.3  CNo        License ownership transfered to Spansion Inc.
 **                                 Added promiscuous mode for EMAC driver
 **                                 Improved support for Flow Control
 **   - 2014-07-15  1.4  CNo, TCh   Fixed a bug with 10Mbit mode
 **                                 MAC address now set via stc_emac_config_t
 **                                 First step to support vendor specific PHY registers
 **   - 2014-10-17  1.5  CNo        New function Emac_TxFrameDirect() for optimized throughput
 **                                 Adapted for PDL compatibility and FM4 support
 **                                 IRQ handling improved
 **   - 2015-01-09  1.6  CNo        Improved autonegotiation feature for universal use
 **   - 2015-07-27  1.7  CNo        Updated driver to be compliant with template structure verson
 **                                 Integrated ethphy.c into emac.c and ethphy.h into emac.h
 **
 ******************************************************************************/

#ifndef __EMAC_H__
#define __EMAC_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "emac_user.h"
#include "base_types.h"
#if (EMAC_USE_PDL == 1)
#include "pdl.h"
#endif

#include "mcu.h"
//#include "emac_reg.h"


/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup EmacGroup Ethernet MAC (EMAC)
 **
 ** Provided functions of EMAC (Ethernet MAC) module:
 **
 ** - Emac_Init()
 ** - Emac_DeInit()
 ** - Emac_TxFrame()
 ** - Emac_GetFrameLength()
 ** - Emac_RxFrame()
 ** - Emac_GetLinkStatus()
 ** - Emac_GetLinkMode()
 ** - Emac_SetLinkMode()
 ** - Emac_Autonegotiate()
 ** - Emac_SetDescToTxBuf()
 ** - Emac_SetDescToRxBuf()
 ** - Emac_SetEcout();
 ** - Emac_TxFrame_GetBufPtr()
 ** - Emac_RxFrame_GetBufPtr()
 ** - Emac_RxFrame_ReleaseBuf()
 ** - EmacIrqHandler()
 **
 **
 ** \brief How to use EMAC module
 **
 ** Emac_Init() must be used for activation of the internal data structures for
 ** using an Ethernet MAC instance. Here the interrupt callback functions for
 ** each of the 2 instance are set-up.
 **
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief EtherCallback function prototype.
 ******************************************************************************/
typedef void (*ether_cb_func_ptr_t)(void);

#if !defined(FM_ETHERNET_MAC1_BASE)
#define FM_ETHERNET_MAC1_BASE FM_ETHERNET_MAC0_BASE + 1
// MCU does not contain Ethernet MAC unit 1, so creating a dummy definition for FM_ETHERNET_MAC1_BASE. Do not use!
#endif
/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#define EMAC0        (*((volatile FM_ETHERNET_MAC_TypeDef *) FM_ETHERNET_MAC0_BASE))
#define EMAC1        (*((volatile FM_ETHERNET_MAC_TypeDef *) FM_ETHERNET_MAC1_BASE))
#define EMAC_CONTROL ((volatile FM_ETHERNET_CONTROL_TypeDef *)FM_ETHERNET_CONTROL_BASE)


/* C binding of definitions if building with C++ compiler                     */
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * definitions for stand-alone usage
 ******************************************************************************/

#ifndef PDL_ZERO_STRUCT
#include <string.h>
#define PDL_ZERO_STRUCT(x)  memset(&x, 0, sizeof(x))
#endif

#ifndef PDL_ON
#define PDL_ON  1    ///< Switches a feature on.
#endif

#ifndef PDL_OFF
#define PDL_OFF 0    ///< Switches a feature off.
#endif


extern void PDL_WAIT_LOOP_HOOK(void);

#ifndef PDL_DEFAULT_INTERRUPT_LEVEL
#define PDL_DEFAULT_INTERRUPT_LEVEL 0x0F
#endif



#if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON) || \
    (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
    #define PDL_PERIPHERAL_EMAC_ACTIVE
#endif


/******************************************************************************
 * Global type definitions
 ******************************************************************************/

// DMA Descriptors for Transmission ********************************************

/**
 *****************************************************************************
 ** \brief  Bit definitions for Transmit Enhanced Descriptor 0 (TDES0)
 **
 ** The TDES0 consists of the control bit and status bit of the transmit frame.
 *****************************************************************************/
typedef struct stc_emac_tdes0
{
  volatile uint32_t DB        : 1; ///< Deferred Bit
  volatile uint32_t UF        : 1; ///< Underflow Error
  volatile uint32_t ED        : 1; ///< Excessive Deferral
  volatile uint32_t CC        : 4; ///< Collision Count
  volatile uint32_t VF        : 1; ///< VLAN Frame
  volatile uint32_t EC        : 1; ///< Excessive Collision
  volatile uint32_t LCO       : 1; ///< Late Collision
  volatile uint32_t NC        : 1; ///< No Carrier
  volatile uint32_t LC        : 1; ///< Loss of Carrier
  volatile uint32_t IPE       : 1; ///< IP Payload Error
  volatile uint32_t FF        : 1; ///< Frame Flushed
  volatile uint32_t JT        : 1; ///< Jabber Timeout
  volatile uint32_t ES        : 1; ///< Error Summary
  volatile uint32_t IHE       : 1; ///< IP Header Error
  volatile uint32_t TTSS      : 1; ///< Transmit Time Stamp Status
  volatile uint32_t RESERVED2 : 2;
  volatile uint32_t TCH       : 1; ///< Second Address Chained
  volatile uint32_t TER       : 1; ///< Transmit End of Ring
  volatile uint32_t CIC       : 2; ///< Checksum Insertion Control
  volatile uint32_t RESERVED1 : 1;
  volatile uint32_t TTSE      : 1; ///< Transmit Time Setup Enable
  volatile uint32_t DP        : 1; ///< Disable Pad
  volatile uint32_t DC        : 1; ///< Disable CRC
  volatile uint32_t FS        : 1; ///< First Segment
  volatile uint32_t LS        : 1; ///< Last Segment
  volatile uint32_t IC        : 1; ///< Interrupt on Completion
  volatile uint32_t OWN       : 1; ///< OWN bit
} stc_emac_tdes0_t;



/**
 *****************************************************************************
 ** \brief  Bit definitions for Transmit Enhanced Descriptor 1 (TDES1)
 **
 ** The TDES1 specifies the size of the transmit buffers 1 and 2.
 *****************************************************************************/
typedef struct stc_emac_tdes1
{
  volatile uint32_t TBS1      : 13; ///< Transmit Buffer 1 Size
  volatile uint32_t RESERVED2 :  3;
  volatile uint32_t TBS2      : 13; ///< Transmit Buffer 2 Size
  volatile uint32_t RESERVED1 :  3;
} stc_emac_tdes1_t;


/**
 *****************************************************************************
 ** \brief  Bit definitions for Transmit Enhanced Descriptor 2 (TDES2)
 **
 ** The TDES2 specifies the physical address of the transmit buffer 1.
 **
 ** These bits specify the physical address of Buffer 1. There is no
 ** limitation on the buffer address alignment.
 *****************************************************************************/
typedef struct stc_emac_tdes2
{
  volatile uint8_t * B1AP; ///< Buffer 1 Address Pointer
} stc_emac_tdes2_t;


/**
 *****************************************************************************
 ** \brief  Bit definitions for Transmit Enhanced Descriptor 3 (TDES3)
 **
 ** The TDES3 specifies the physical addresses of the transmit buffer 2 and
 ** the next descriptor.
 **
 ** These bits specify the physical address of Buffer 2 when a descriptor
 ** ring structure is used. If the second Address Chained (TDES0[20]) bit is
 ** set, this address specifies the physical memory where the next Descriptor
 ** is present. The buffer address pointer must be aligned to the bus width
 ** only when TDES0[20] is set. (LSBs are ignored internally.).
 *****************************************************************************/
typedef struct stc_emac_tdes3
{
  volatile uint8_t * B2AP; ///< Buffer 2 Address Pointer
} stc_emac_tdes3_t;



/**
 *****************************************************************************
 ** \brief  Bit definitions for Transmit Enhanced Descriptor 6 (TDES6)
 **
 ** The TDES6 stores the least significant 32 bits of the time stamp captured
 ** during  the transmission by the DMA.
 **
 ** This field is updated by DMA with the least significant 32 bits of the
 ** time stamp captured for the corresponding transmit frame. This field has
 ** the time stamp only if the Last Segment bit (LS) in the descriptor is set
 ** and Time stamp status (TTSS) bit is set.
 *****************************************************************************/
typedef struct stc_emac_tdes6
{
  volatile uint32_t TTSL; ///< Transmit Frame Time Stamp Low
} stc_emac_tdes6_t;


/**
 *****************************************************************************
 ** \brief  Bit definitions for Transmit Enhanced Descriptor 7 (TDES7)
 **
 ** The TDES7 stores the most significant 32 bits of the time stamp captured
 ** during the transmission by the DMA.
 **
 ** This field is updated by DMA with the most significant 32 bits of the
 ** time stamp captured for the corresponding transmit frame. This field has
 ** the time stamp only if the Last Segment bit (LS) in the descriptor is set
 ** and Time stamp status (TTSS) bit is set.
 *****************************************************************************/
typedef struct stc_emac_tdes7
{
  volatile uint32_t TTSH; ///< Transmit Frame Time Stamp High
} stc_emac_tdes7_t;


/**
 *****************************************************************************
 ** \brief Structure of an DMA descriptor for transmission
 *****************************************************************************/
typedef struct stc_emac_dma_tx_desc
{
  union
  {
    volatile uint32_t TDES0;
    volatile stc_emac_tdes0_t TDES0_f; ///< OWN, Ctrl, TTSE, TTSS, Status
  };
  union
  {
    volatile uint32_t TDES1;
    volatile stc_emac_tdes1_t TDES1_f; ///< Byte Count Buffers
  };
  union
  {
    volatile uint8_t * TDES2;
    volatile stc_emac_tdes2_t TDES2_f; ///< Buffer Address 1
  };
  union
  {
    volatile struct stc_emac_dma_tx_desc * TDES3;
    volatile stc_emac_tdes3_t TDES3_f; ///< Buffer Address 2 / Next Descriptor Address
  };
//  const uint32_t TDES4 = 0;  ///< Reserved
//  const uint32_t TDES5 = 0;  ///< Reserved
//  EthMacTDES6_field_t TDES6; ///< Transmit Time Stamp Low
//  EthMacTDES7_field_t TDES7; ///< Transmit Time Stamp High
} stc_emac_dma_tx_desc_t;



// DMA Descriptors for data reception ******************************************

/**
 *****************************************************************************
 ** \brief  Bit definitions for Receive Enhanced Descriptor 0 (RDES0)
 **
 ** The RDES0 consists of the control bit and status bit of the receive frame.
 *****************************************************************************/
typedef struct stc_emac_rdes0
{
  volatile uint32_t ESA       :  1; ///< Extended Status Available
  volatile uint32_t CE        :  1; ///< CRC Error
  volatile uint32_t DBE       :  1; ///< Dribble Bit Error
  volatile uint32_t RE        :  1; ///< Receive Error
  volatile uint32_t RWT       :  1; ///< Receive Watchdog Timeout
  volatile uint32_t FT        :  1; ///< Frame Type
  volatile uint32_t LC        :  1; ///< Late Collision
  volatile uint32_t TS        :  1; ///< Time Stamp
  volatile uint32_t LS        :  1; ///< Last Descriptor
  volatile uint32_t FS        :  1; ///< First Descriptor
  volatile uint32_t VLAN      :  1; ///< VLAN tag
  volatile uint32_t OE        :  1; ///< Overflow Error
  volatile uint32_t LE        :  1; ///< Length Error
  volatile uint32_t SAF       :  1; ///< Source Address Filter Fail
  volatile uint32_t DE        :  1; ///< Descriptor Error
  volatile uint32_t ES        :  1; ///< Error Summary
  volatile uint32_t FL        : 14; ///< Frame Length
  volatile uint32_t AFM       :  1; ///< Destination Address Filter Fail
  volatile uint32_t OWN       :  1; ///< OWN bit
} stc_emac_rdes0_t;


/**
 *****************************************************************************
 ** \brief  Bit definitions for Receive Enhanced Descriptor 1 (RDES1)
 **
 ** The RDES2 specifies the size of the receive buffers 1 and 2, and the
 ** control information.
 *****************************************************************************/
typedef struct stc_emac_rdes1
{
  volatile uint32_t RBS1      : 13; ///< Receive Buffer 1 Size
  volatile uint32_t RESERVED2 :  1; ///< Reserved
  volatile uint32_t RCH       :  1; ///< Second Address Chained
  volatile uint32_t RER       :  1; ///< Receive End of Ring
  volatile uint32_t RBS2      : 13; ///< Receive Buffer 2 Size
  volatile uint32_t RESERVED1 :  2; ///< Reserved
  volatile uint32_t DIC       :  1; ///< Disable Interrupt on Completion
} stc_emac_rdes1_t;


/**
 *****************************************************************************
 ** \brief  Bit definitions for Receive Enhanced Descriptor 2 (TDES2)
 **
 ** The RDES2 specifies the physical address of the receive buffer 1.
 *****************************************************************************/
typedef struct stc_emac_rdes2
{
  volatile uint8_t* B1AP; ///< Buffer 1 Address Pointer
} stc_emac_rdes2_t;


/**
 *****************************************************************************
 ** \brief  Bit definitions for Receive Enhanced Descriptor 3 (RDES3)
 **
 ** The RDES3 specifies the physical addresses of the receive buffer 2 and
 ** the next descriptor.
 *****************************************************************************/
typedef struct stc_emac_rdes3
{
  volatile uint8_t* B2AP; ///< Buffer 2 Address Pointer
} stc_emac_rdes3_t;

/**
 *****************************************************************************
 ** \brief  Bit definitions for Receive Enhanced Descriptor 4 (RDES4)
 **
 ** The RDES4 stores the enhanced status information of the receive frame.
 *****************************************************************************/
typedef struct stc_emac_rdes4
{
  volatile uint32_t IPT      :  3; ///< IP Payload Type
  volatile uint32_t IPHE     :  1; ///< IP Header Error
  volatile uint32_t IPE      :  1; ///< IP Payload Error
  volatile uint32_t IPCB     :  1; ///< IP Checksum Bypassed
  volatile uint32_t IP4R     :  1; ///< IPv4 Packet Received
  volatile uint32_t IP6R     :  1; ///< IPv6 Packet Received
  volatile uint32_t MT       :  4; ///< Message Type
  volatile uint32_t PFT      :  1; ///< PTP Frame Type
  volatile uint32_t PV       :  1; ///< PTP Version
  volatile uint32_t TD       :  1; ///< Timestamp Dropped
  volatile uint32_t RESERVED : 17; ///< Reserved
} stc_emac_rdes4_t;

/**
 *****************************************************************************
 ** \brief Structure of an DMA descriptor for receiving
 *****************************************************************************/
typedef struct stc_emac_dma_rx_desc
{
  union
  {
    volatile uint32_t RDES0;
    volatile stc_emac_rdes0_t RDES0_f; ///< OWN, Ctrl, TTSE, TTSS, Status
  };
  union
  {
    volatile uint32_t RDES1;
    volatile stc_emac_rdes1_t RDES1_f; ///< Ctrl, Byte Count Buffers
  };
  union
  {
    volatile uint8_t * RDES2;
    volatile stc_emac_rdes2_t RDES2_f; ///< Buffer Address 1
  };
  union
  {
    volatile struct stc_emac_dma_rx_desc * RDES3;
    volatile stc_emac_rdes3_t RDES3_f; ///< Buffer Address 2 / Next Descriptor Address
  };
  union
  {
    volatile uint32_t RDES4;
    volatile stc_emac_rdes4_t RDES4_f; ///< Extended Status
  };
//  const uint32_t RDES5 = 0;  ///< Reserved
//  EthMacRDES6_field_t RDES6; ///< Receive Time Stamp Low
//  EthMacRDES7_field_t RDES7; ///< Receive Time Stamp High
} stc_emac_dma_rx_desc_t;


typedef enum en_emac_link_status
{
  EMAC_LinkStatusLinkDown                     = 0,
  EMAC_LinkStatusLinkUp                       = 1,
  EMAC_LinkStatusAutonegotiationInProgress    = 2,
  EMAC_LinkStatusAutonegotiationSuccessful    = 3,
  EMAC_LinkStatusAutonegotiationNotSupported  = 4,
  EMAC_LinkStatusInvalidParameter             = 5,
  EMAC_LinkStatusUnknownError                 = 6
} en_emac_link_status_t;


typedef enum en_emac_link_mode
{
  EMAC_LinkModeAutonegotiation = 0,
  EMAC_LinkModeHalfDuplex10M   = 1,
  EMAC_LinkModeFullDuplex10M   = 2,
  EMAC_LinkModeHalfDuplex100M  = 3,
  EMAC_LinkModeFullDuplex100M  = 4
} en_emac_link_mode_t;

typedef enum receive_status
{
   INIT=0,
   RECEIVED,
   HANDLED
}receive_status_t;

/**
 *****************************************************************************
 ** \brief Ethernet MAC configuration
 **
 ** The EMAC configuration of an instance
 *****************************************************************************/
typedef struct stc_emac_config
{
  boolean_t bPromiscuousMode; ///< If enabled, interface will receive all frames, including those not addressed to it

  func_ptr_t       pfnRxCallback;       ///< Reception Callback function pointer
  func_ptr_t       pfnTxCallback;       ///< Transmission Callback function pointer
  func_ptr_t       pfnErrorCallback;    ///< Error Callback function pointer

  uint8_t au8MacAddress[6]; ///< Ethernet Hardware Address, also known as MAC Address

} stc_emac_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled EMAC instance
typedef enum en_emac_instance_index
{
    #if (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_ON)
    EmacInstanceIndexEmac0,
    #endif

    #if (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_ON)
    EmacInstanceIndexEmac1,
    #endif

    // Case if no EMAC instance is selected
    #if (  (PDL_PERIPHERAL_ENABLE_EMAC0 == PDL_OFF) \
        && (PDL_PERIPHERAL_ENABLE_EMAC1 == PDL_OFF))
    EmacInstanceDummy
    #endif
} en_emac_instance_index_t;

/// Datatype for holding internal data needed for EMAC
typedef struct stc_emac_intern_data
{
    /// Rx Callback for interrupts
    func_ptr_t  pfnRxCallback;
    /// Tx Callback for interrupts
    func_ptr_t  pfnTxCallback;
    /// Error Callback for interrupts
    func_ptr_t  pfnErrorCallback;

    volatile stc_emac_dma_tx_desc_t * pstcTxDescChainHead; ///< First element in Tx ring buffer
    volatile stc_emac_dma_rx_desc_t * pstcRxDescChainHead; ///< First element in Rx ring buffer

    volatile stc_emac_dma_tx_desc_t * pstcTxDescCurrent; ///< Current element in Tx ring buffer
    volatile stc_emac_dma_rx_desc_t * pstcRxDescCurrent; ///< Current element in Rx ring buffer

    volatile FM_ETHERNET_MAC_TypeDef* pstcManagementBus; //(MDC, MDIO)

    uint8_t au8MacAddress[6]; ///< Ethernet Hardware Address, also known as MAC Address

    boolean_t bLinkConfigured; ///< Used to determine when a link goes down

    boolean_t bPromiscuousMode; ///< If enabled, interface will receive all frames, including those not addressed to it
} stc_emac_intern_data_t ;





/// EMAC instance data type
typedef struct stc_emac_instance_data
{
    volatile FM_ETHERNET_MAC_TypeDef*  pstcInstance;  ///< pointer to registers of an instance
    stc_emac_intern_data_t stcInternData; ///< module internal data of instance
} stc_emac_instance_data_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/// Look-up table for all enabled EMAC instances and their internal data
extern stc_emac_instance_data_t m_astcEmacInstanceDataLut[];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/

en_result_t Emac_Init(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                      stc_emac_config_t*    pstcConfig
                     );

en_result_t Emac_DeInit(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

en_result_t Emac_TxFrame(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                         uint8_t * pu8Buf,
                         uint16_t u16Len
                        );

en_result_t Emac_TxFrameDirect(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                               uint8_t * pu8Buf,
                               uint16_t u16Len
                              );

uint16_t Emac_GetFrameLength(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

uint16_t Emac_RxFrame(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                      uint8_t * pu8Buf
                     );

en_result_t Emac_SetLinkUp(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

en_emac_link_status_t Emac_GetLinkStatus(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

en_emac_link_mode_t Emac_GetLinkMode(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

en_emac_link_mode_t Emac_SetLinkMode(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                      en_emac_link_mode_t enLinkMode
                                    );

en_emac_link_status_t Emac_Autonegotiate(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

#if ((EMAC0_BUFFERS_IN_DRIVERSPACE != PDL_ON) || (EMAC1_BUFFERS_IN_DRIVERSPACE != PDL_ON))

en_result_t Emac_SetDescToTxBuf(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                uint16_t u16DescNumber,
                                volatile uint8_t * pu8UserBuf,
                                uint16_t u16BufLength
                               );

en_result_t Emac_SetDescToRxBuf(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                uint16_t u16DescNumber,
                                volatile uint8_t * pu8UserBuf,
                                uint16_t u16BufLength
                               );

en_result_t Emac_SetCurrentDescToRxBuf( volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                                        volatile uint8_t *    pu8UserBuf,
                                        uint16_t              u16BufLength
                                      );
#endif

#if EMAC_ECOUT
en_result_t Emac_SetEcout(void);
#endif

void * Emac_TxFrame_GetBufPtr(void);

void Emac_TxFrame_ReleaseBuf(void);

void * Emac_RxFrame_GetBufPtr(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

void Emac_RxFrame_ReleaseBuf (volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

#if (EMAC_INTERRUPT_MODE == PDL_ON)
void EmacIrqHandler(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac,
                    stc_emac_intern_data_t* pstcEmacInternData
                   );
#endif

en_emac_link_mode_t EMAC_AUTONEG_FUNCTION(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

//@} // EmacGroup


/**
 ******************************************************************************
 ** \defgroup EthphyGroup Ethernet PHY (ETHPHY)
 **
 ** Provided functions of ETHPHY module:
 ** - Ethphy_Read()
 ** - Ethphy_Write()
 ** - Ethphy_Reset()
 **
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \brief IEEE standard PHY register addresses
 ******************************************************************************/
#define ETHPHY_BASIC_CONTROL_REGISTER                         0x00
#define ETHPHY_BASIC_STATUS_REGISTER                          0x01
#define ETHPHY_PHY_IDENTIFIER1                                0x02
#define ETHPHY_PHY_IDENTIFIER2                                0x03
#define ETHPHY_AUTONEGOTIATION_ADVERTISEMENT_REGISTER         0x04
#define ETHPHY_AUTONEGOTIATION_LINK_PARTNER_ABILITY_REGISTER  0x05
#define ETHPHY_AUTONEGOTIATION_EXPANSION_REGISTER             0x06

/**
 ******************************************************************************
 ** \brief PHY vendor specific registers
 **
 ** These registers are not standardized by the IEEE. The definitions must
 ** be adapted to your specific device in order to make auto-negotiation working.
 ******************************************************************************/
#define ETHPHY_STATUS_REGISTER                                0x10
#define ETHPHY_MODE_CONTROL_STATUS_REGISTER                   0x11
#define ETHPHY_SPECIAL_MODES                                  0x12
#define ETHPHY_SYMBOL_ERROR_COUNTER_REGISTER                  0x1A
#define ETHPHY_CONTROL_STATUS_INDICATION_REGISTER             0x1B
#define ETHPHY_INTERRUPT_SOURCE_REGISTER                      0x1C
#define ETHPHY_INTERRUPT_MASK_REGISTER                        0x1D
#define ETHPHY_PHY_SPECIAL_CONTROL_STATUS_REGISTER            0x1E

/**
 ******************************************************************************
 ** \brief PHY basic control register bit positions
 ******************************************************************************/
#define ETHPHY_BCR_SOFT_RESET               15
#define ETHPHY_BCR_LOOPBACK                 14
#define ETHPHY_BCR_SPEED_SELECT             13
#define ETHPHY_BCR_AUTO_NEGOTIATION_ENABLE  12
#define ETHPHY_BCR_POWER_DOWN               11
#define ETHPHY_BCR_ISOLATE                  10
#define ETHPHY_BCR_RESTART_AUTO_NEGOTIATE    9
#define ETHPHY_BCR_DUPLEX_MODE               8
#define ETHPHY_BCR_COLLISION_TEST            7

/**
 ******************************************************************************
 ** \brief PHY basic status register bit positions
 ******************************************************************************/
#define ETHPHY_BSR_100_BASE_T4              15
#define ETHPHY_BSR_100_BASE_TX_FULL_DUPLEX  14
#define ETHPHY_BSR_100_BASE_TX_HALF_DUPLEX  13
#define ETHPHY_BSR_10_BASE_T_FULL_DUPLEX    12
#define ETHPHY_BSR_10_BASE_T_HALF_DUPLEX    11
#define ETHPHY_BSR_MF_PREAMBLE_SUPPRESSION   6 ///<Available only in some PHYs
#define ETHPHY_BSR_AUTO_NEGOTIATE_COMPLETE   5
#define ETHPHY_BSR_REMOTE_FAULT              4
#define ETHPHY_BSR_AUTO_NEGOTIATE_ABILITY    3
#define ETHPHY_BSR_LINK_STATUS               2
#define ETHPHY_BSR_JABBER_DETECT             1
#define ETHPHY_BSR_EXTENDED_CAPABILITIES     0


/**
 ******************************************************************************
 ** \brief Vendor specific PHY status register bit positions - register address-0x10
 **
 ** This register is vendor specific, i.e. not standardized by the IEEE.
 ** The definitions must be adapted to your specific device in order to make auto-negotiation working.
 ******************************************************************************/
#define ETHPHY_SR_AUTO_NEGOTIATE_COMPLETE    4
#define ETHPHY_SR_DUPLEX_STATUS              2
#define ETHPHY_SR_SPEED_STAUTS               1
#define ETHPHY_SR_LINK_STATUS                0

/**
 ******************************************************************************
 ** \brief PHY access timeout
 ******************************************************************************/
#define ETHPHY_TIMEOUT               200000000

/**
 ******************************************************************************
 ** \brief PHY error return values
 ******************************************************************************/
#define ETHPHY_ERROR_INVALID_ADDRESS 0xFFFE
#define ETHPHY_ERROR_TIMEOUT 0xFFFF

/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
uint16_t    Ethphy_Read(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac, uint8_t u8PhyReg);
en_result_t Ethphy_Write(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac, uint8_t u8PhyReg, uint16_t u16Val);
en_result_t Ethphy_Reset(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac);

//@} // EthphyGroup

#ifdef __cplusplus
}
#endif

#endif /* __EMAC_H__ */
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
