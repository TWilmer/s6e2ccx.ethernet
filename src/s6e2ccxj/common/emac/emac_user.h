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
/** \file emac_user.h
 **
 ** User settings headerfile for EMAC driver.
 **
 **
 ** History:
 **   - 2012-11-20  1.0  Fujitsu  First version.
 **   - 2012-11-26  1.1  CNo Improved documentation.
 **   - 2012-12-10  1.1a CNo Work-around for LwIP and uIP ICMP echo bug
 **   - 2012-12-18  1.2  CNo Support for multicast
 **   - 2014-07-15  1.4  CNo MAC address now set in stc_emac_config_t
 **   - 2014-10-14  1.5  CNo Extended for PDL and FM4
 **   - 2015-01-09  1.6  CNo Improved autonegotiation feature for universal use
 **   - 2015-07-27  1.7  CNo Added support for SMSC LAN8710A PHY on SK-FM3-176PMC-ETHERNET
 **
 ******************************************************************************/

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief Should Spansion Peripheral Drivers Library be used or not
 **
 ** Possible definitions are 1 or 0
 **
 ** 0: Use driver stand-alone
 ** 1: Use driver together with PDL
 **
 ******************************************************************************/
#define EMAC_USE_PDL 0

/**
 ******************************************************************************
 ** \brief User Defines for EMAC resource enable
 **
 ** Possible definitions are PDL_ON and PDL_OFF.
 **
 ******************************************************************************/
#define PDL_PERIPHERAL_ENABLE_EMAC0  PDL_ON
#define PDL_PERIPHERAL_ENABLE_EMAC1  PDL_OFF

/**
 ******************************************************************************
 ** \brief Activate IRQ support for Ethernet driver
 **
 ** Possible definitions are PDL_ON and PDL_OFF.
 **
 ******************************************************************************/
#define EMAC_INTERRUPT_MODE     PDL_OFF

/**
 ******************************************************************************
 ** \brief User Emac Interrupt level settings
 **
 ** Possible values are 0 (high priority) to 15 (low priority)
 ******************************************************************************/
#define PDL_IRQ_LEVEL_EMAC0             2
#define PDL_IRQ_LEVEL_EMAC1             4

/**
 ******************************************************************************
 ** \brief EMAC0 User/Driver Buffer location
 **
 ** PDL_ON:  Driver buffer are used
 ** PDL_OFF: User has to provide buffer memory
 ******************************************************************************/
#define EMAC0_BUFFERS_IN_DRIVERSPACE PDL_ON


/**
 ******************************************************************************
 ** \brief Support for Spansion starterkits
 **
 ** Activate board specific settings by defining exactly one of these symbols:
 **
 ** SK_FM3_176PMC_ETHERNET from Spansion Europe:
 ** STARTERKIT_SK_FM3_176PMC_ETHERNET
 **
 ** If version 1.0 is used, please use add itionally:
 ** SK_FM3_176PMC_ETHERNET_BOARDVERSION10
 **
 ** FSSDC-9B618-EVB from Spansion China:
 ** STARTERKIT_FSSDC9B618EVB
 **
 ******************************************************************************/
//#define STARTERKIT_SK_FM3_176PMC_ETHERNET
//#define SK_FM3_176PMC_ETHERNET_BOARDVERSION10
//#define STARTERKIT_FSSDC9B618EVB
#define STARTERKIT_SK_FM4_216_ETHERNET


/**
 ******************************************************************************
 ** \brief EMAC0 Transmission/Reception Ring size
 ******************************************************************************/
#define EMAC0_TX_RING_SIZE     6
#define EMAC0_RX_RING_SIZE     4

/**
 ******************************************************************************
 ** \brief EMAC0 Transmission/Reception Buffer size
 **
 ** Only used if EMAC0_BUFFERS_IN_DRIVERSPACE == PDL_ON
 ** Must be multiple of 4, 13 Bit value, page 159
 **
 ******************************************************************************/
#define EMAC0_TX_BUF_SIZE     1536
#define EMAC0_RX_BUF_SIZE     1536

/**
 ******************************************************************************
 ** \brief EMAC1 User/Driver Buffer location
 **
 ** PDL_ON:  Driver buffer are used
 ** PDL_OFF: User has to provide buffer memory
 ******************************************************************************/
#define EMAC1_BUFFERS_IN_DRIVERSPACE PDL_ON

/**
 ******************************************************************************
 ** \brief EMAC1 Transmission/Reception Ring size
 ******************************************************************************/
#define EMAC1_TX_RING_SIZE     2
#define EMAC1_RX_RING_SIZE     4

/**
 ******************************************************************************
 ** \brief EMAC1 Transmission/Reception Buffer size
 **
 ** Only used if EMAC1_BUFFERS_IN_DRIVERSPACE == PDL_ON
 ** Must be multiple of 4, 13 Bit value, page 159
 ******************************************************************************/
#define EMAC1_TX_BUF_SIZE     1536
#define EMAC1_RX_BUF_SIZE     1536

/**
 ******************************************************************************
 ** \brief Addresses of the used external PHYs
 ******************************************************************************/
#if defined STARTERKIT_SK_FM3_176PMC_ETHERNET
/// \note PHY addresses for STARTERKIT_SK_FM3_176PMC_ETHERNET selected
#define EMAC0_PHY_ADDRESS 0
#define EMAC1_PHY_ADDRESS 1

#elif defined STARTERKIT_FSSDC9B618EVB
/// \note PHY addresses for hardware board with PN FSSDC-9B618-EVB selected
#define EMAC0_PHY_ADDRESS 1
#define EMAC1_PHY_ADDRESS 1

#elif defined STARTERKIT_SK_FM4_216_ETHERNET
/// \note PHY address for STARTERKIT_SK_FM4_216_ETHERNET selected (EMAC1 just defined to avoid undifined symbols)
#define EMAC0_PHY_ADDRESS 7
#define EMAC1_PHY_ADDRESS 0

#else
/// \note No Fujitsu starterkit is used, PHY address must match custom hardware
#define EMAC0_PHY_ADDRESS 0
#define EMAC1_PHY_ADDRESS 0
#error Undefined
#endif

/**
 ******************************************************************************
 ** \brief EMAC0 MAC address
 ** \note From driver version 1.4 onward, MAC address is configured via stc_emac_config_t!
 ** \warning  FM4 has UID, which here acts as a pseude random number generator. This is not a standards conform, valid MAC address! Use for evaluation only!
 ******************************************************************************/
#define EMAC0_MAC_ADDRESS0  0x00
#define EMAC0_MAC_ADDRESS1  (((FM4_UNIQUE_ID->UIDR0) >>  4)&0xFF)
#define EMAC0_MAC_ADDRESS2  (((FM4_UNIQUE_ID->UIDR0) >> 12)&0xFF)
#define EMAC0_MAC_ADDRESS3  (((FM4_UNIQUE_ID->UIDR0) >> 20)&0xFF)
#define EMAC0_MAC_ADDRESS4  (((FM4_UNIQUE_ID->UIDR1) >>  0)&0xFF)
#define EMAC0_MAC_ADDRESS5  (((FM4_UNIQUE_ID->UIDR1) >>  8)&0x1F)
// This is not a standards conform, valid MAC address! Use for evaluation only!

/**
 ******************************************************************************
 ** \brief EMAC1 MAC address
 ** \note From driver version 1.4 onward, MAC address is configured via stc_emac_config_t!
 ******************************************************************************/
#define EMAC1_MAC_ADDRESS0  0x00
#define EMAC1_MAC_ADDRESS1  0x01
#define EMAC1_MAC_ADDRESS2  0x01
#define EMAC1_MAC_ADDRESS3  0x66
#define EMAC1_MAC_ADDRESS4  0x73
#define EMAC1_MAC_ADDRESS5  0x38

/**
 ******************************************************************************
 ** \brief Multicast address
 ******************************************************************************/
#define MULTICAST_ADDRESS0  0x00
#define MULTICAST_ADDRESS1  0x00
#define MULTICAST_ADDRESS2  0x00
#define MULTICAST_ADDRESS3  0x00
#define MULTICAST_ADDRESS4  0x00
#define MULTICAST_ADDRESS5  0x00

/**
 ******************************************************************************
 ** \brief External PHY reset pins
 **
 ** Use GPIO addresses of the bit-band alias definitions of the device header
 ** file.
 ******************************************************************************/
#define EMAC0_PHY_RESET_PIN ((uint32_t*) &bFM_GPIO_PDOR6_P5)

/**
 ******************************************************************************
 ** \brief Enable Ethernet clock signal generation for port pin E_COUT
 **
 ** Configures USB/Ethernet Clock module with PLL to generate either 25MHz (MII mode)
 ** or 50MHz (RMII mode)
 **
 ** PDL_ON:  PLL is used for PHY clock
 ** PDL_OFF: PLL is not used for PHY clock
 ******************************************************************************/
#define EMAC_ECOUT  PDL_OFF

/**
 ******************************************************************************
 ** \brief Management Bus Definition
 **
 ** This definition allows one single management bus (MDC, MDIO) to be used
 ** for both Ethernet channels.
 **
 ** Possible Values: EMAC0 or EMAC1
 **
 ******************************************************************************/
#define EMAC0_MANAGEMENTBUS   EMAC0
#define EMAC1_MANAGEMENTBUS   EMAC1

/**
 ******************************************************************************
 ** \brief RMII PHY interface usage
 **
 ** PDL_OFF: MII is used (Media Independent Interface)
 ** PDL_ON: RMII is used (Reduced Media Independent Interface)
 **
 ******************************************************************************/
#define EMAC_PHYINTERFACE_RMII   PDL_OFF

/**
 ******************************************************************************
 ** \brief EMAC Buffer fragmentation
 **
 ** PDL_OFF: reserved, please only use PDL_ON
 ** PDL_ON: All buffers are large enough to contain a whole Ethernet frame (MTU size)
 ******************************************************************************/
#define EMAC_BUFFERS_NOT_FRAGMENTED   PDL_ON

/**
 ******************************************************************************
 ** \brief CIC (Checksum Insertion Control)
 **
 ** These bits control the checksum calculation and insertion. Bit encodings
 ** are as shown below.
 ** - 0  Checksum Insertion Disabled.
 ** - 1  Only IP header checksum calculation and insertion are enabled.
 ** - 2  IP header checksum and payload checksum calculation and insertion are
 **      enabled, but pseudo-header checksum is not calculated in hardware.
 ** - 3  IP Header checksum and payload checksum calculation and insertion are
 **      enabled, and pseudo-header checksum is calculated in hardware.
 ******************************************************************************/
#define EMAC_COE_MODE   3

/**
 ******************************************************************************
 ** \brief Activate work-around for bug in uIP and LwIP TCP/IP stacks
 **
 ** For the Checksum Offload Engine to work correctly, the checksum fields in
 ** the IP header, as well as in the TCP, UDP or ICMP header must be 0.
 **
 ** LwIP up to the recent stable 1.4.0 version, as well as current uIP have an option
 ** to disable software checksum calculation for IP, UDP and TCP.
 ** A similar option ICMP was (apparently by accident) omitted.
 **
 ** If EMAC_ENABLE_ICMP_CHECKSUM_BUG_WORKAROUND is set to PDL_ON,
 ** every frame to be sent will be checked if it is an ICMP echo reply
 ** and if so, its checksum field cleared.
 **
 ** This of course slows down every packet transmission a little and should be fixed in the
 ** original code. As a matter of fact, this is a known bug and is expected to be fixed
 ** in future releases of LwIP.
 **
 ** For other TCP/IP stacks, this work-around is usually not necessary and should be disabled.
 **
 ** Spansion tries to retain third-party software in software examples in its original state as far
 ** as possible in order to retain compatibility to the maximum possible extend.
 **
 ******************************************************************************/
#define EMAC_ENABLE_ICMP_CHECKSUM_BUG_WORKAROUND   PDL_OFF

/**
 ******************************************************************************
 ** \brief Activate multicast filtering
 **
 ** PDL_ON:  Multicast filtering is used
 ** PDL_OFF: Multicast filtering is not used
 ******************************************************************************/
#define EMAC_MULTICAST_FILTER  PDL_OFF

/**
 ******************************************************************************
 ** \brief Select callback function to read out autonegotiation result from PHY
 **
 ** Provided as a reference are callback functions for PHYs that are assembled
 ** on eval boards SK-FM3-176PMC-ETHERNET and SK-FM4-216-ETHERNET
 ******************************************************************************/
#define EMAC_AUTONEG_FUNCTION  EmacUser_AutoNegotiatePhy_Micrel_KSZ8091_CB



/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
