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
/** \file emac_user.c
 **
 ** A detailed description is available at
 ** @link EmacGroup EMAC Module description @endlink
 **
 ** History:
 **   - 2015-01-09  1.6  CNo        Initial version, improved autonegotiation feature for universal use
 **   - 2015-07-27  1.7  CNo        Added support for SMSC LAN8710A PHY on SK-FM3-176PMC-ETHERNET
 **
 ******************************************************************************/


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "emac_user.h"
#include "emac.h"

#if (defined(PDL_PERIPHERAL_EMAC_ACTIVE))

/**
 ******************************************************************************
 ** \addtogroup EmacGroup
 ******************************************************************************/
//@{

#if (EMAC_AUTONEG_FUNCTION == EmacUser_AutoNegotiatePhy_Micrel_KSZ8091_CB)
/**
 ******************************************************************************
 ** \brief PHY specific function for Micrel KSZ8091MNX to read out autonegotiation result
 **
 ** In IEEE 802.3 standard, there is defined a way to start autonegotiation but
 ** not how to handle the result. Therefore a specific callback function like this must
 ** be written for every used PHY type.
 **
 ** \param [in]  pstcEmac    Ethernet Mac instance
 **
 ** \retval enLinkMode       Any value of #en_emac_link_mode_t but not EMAC_LinkModeAutonegotiation.
 **                          EMAC_LinkModeFullDuplex100M as safe default value in case an error occured
 ******************************************************************************/
en_emac_link_mode_t EmacUser_AutoNegotiatePhy_Micrel_KSZ8091_CB(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
    en_emac_link_mode_t enLinkMode = EMAC_LinkModeAutonegotiation;
    uint16_t u16PhyRegVal = 0;

    // According to data sheet, three LSB bits of register 0x1E indicate auto-negotiation result
    // Read out auto-negotiation result from PHY
    u16PhyRegVal = Ethphy_Read(pstcEmac, 0x1E);
    u16PhyRegVal &= 0x7;

    switch (u16PhyRegVal)
    {
        case(1):
            enLinkMode = EMAC_LinkModeHalfDuplex10M;
            break;
        case(2):
            enLinkMode = EMAC_LinkModeHalfDuplex100M;
            break;
        case(5):
            enLinkMode = EMAC_LinkModeFullDuplex10M;
            break;
         case(6):
            enLinkMode = EMAC_LinkModeFullDuplex100M;
            break;
         default:
            //printf("Autonegotiation error!");
            // Setting 100M full duplex, as it is a common setting
            enLinkMode = EMAC_LinkModeFullDuplex100M;
            break;
    }
    return enLinkMode;
}
#endif // (EMAC_AUTONEG_FUNCTION == EmacUser_AutoNegotiatePhy_Micrel_KSZ8091_CB)




#if (EMAC_AUTONEG_FUNCTION == EmacUser_AutoNegotiatePhy_SMSC_LAN8710A_CB)
/**
 ******************************************************************************
 ** \brief PHY specific function for SMSC LAN8710A to read out autonegotiation result
 **
 ** In IEEE 802.3 standard, there is defined a way to start autonegotiation but
 ** not how to handle the result. Therefore a specific callback function like this must
 ** be written for every used PHY type.
 **
 ** \param [in]  pstcEmac    Ethernet Mac instance
 **
 ** \retval enLinkMode       Any value of #en_emac_link_mode_t but not EMAC_LinkModeAutonegotiation.
 **                          EMAC_LinkModeFullDuplex100M as safe default value in case an error occured
 ******************************************************************************/
en_emac_link_mode_t EmacUser_AutoNegotiatePhy_SMSC_LAN8710A_CB(volatile FM_ETHERNET_MAC_TypeDef* pstcEmac)
{
    en_emac_link_mode_t enLinkMode = EMAC_LinkModeAutonegotiation;
    uint16_t u16PhyRegVal = 0;

    // According to data sheet, three LSB bits of register 0x1E indicate auto-negotiation result
    // Read out auto-negotiation result from PHY
    u16PhyRegVal = Ethphy_Read(pstcEmac, 0x1F);
    u16PhyRegVal &= ((1 << 12)|(7 << 2));

    if (0 ==(u16PhyRegVal & (1 << 12)))
    {
        // Datasheet: "Auto-negotiation is not done or disabled (or not active)"
        enLinkMode = EMAC_LinkModeFullDuplex100M;
        // Setting 100M full duplex, as it is a common setting
    }
    else
    {
        u16PhyRegVal &= (7 << 2);

        switch (u16PhyRegVal)
        {
            case(1 << 2):
                enLinkMode = EMAC_LinkModeHalfDuplex10M;
                break;
            case(2 << 2):
                enLinkMode = EMAC_LinkModeHalfDuplex100M;
                break;
            case(5 << 2):
                enLinkMode = EMAC_LinkModeFullDuplex10M;
                break;
            case(6 << 2):
                enLinkMode = EMAC_LinkModeFullDuplex100M;
                break;
        default:
            //printf("Autonegotiation error!");
            // Setting 100M full duplex, as it is a common setting
            enLinkMode = EMAC_LinkModeFullDuplex100M;
            break;
        }
    }
    return enLinkMode;
}
#endif // (EMAC_AUTONEG_FUNCTION == EmacUser_AutoNegotiatePhy_SMSC_LAN8710A_CB)


//@} // EmacGroup

#endif // #if (defined(PDL_PERIPHERAL_EMAC_ACTIVE))

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

