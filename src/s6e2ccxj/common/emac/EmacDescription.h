/*
        *** THIS IS A FILE FOR DOXYGEN AND NOT A C HEADER FILE ***
*/

/**
 ******************************************************************************
 ** \mainpage Ethernet Driver for FM3 Documentation
 ** <hr>
 ** <center>Copyright (C) 2013 Spansion LLC. All Rights Reserved.</center>
 **
 ** This software is owned and published by:
 ** Spansion LLC, 915 DeGuigne Dr. Sunnyvale, CA  94088-3453 ("Spansion").
 **
 ** BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
 ** BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
 **
 ** This software contains source code for use with Spansion
 ** components. This software is licensed by Spansion to be adapted only
 ** for use in systems utilizing Spansion components. Spansion shall not be
 ** responsible for misuse or illegal use of this software for devices not
 ** supported herein.  Spansion is providing this software "AS IS" and will
 ** not be responsible for issues arising from incorrect user implementation
 ** of the software.
 **
 ** SPANSION MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
 ** REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
 ** ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
 ** WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
 ** WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
 ** WARRANTY OF NONINFRINGEMENT.
 ** SPANSION SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
 ** NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 ** LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
 ** LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
 ** INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
 ** INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
 ** SAVINGS OR PROFITS,
 ** EVEN IF SPANSION HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 ** YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
 ** INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
 ** FROM, THE SOFTWARE.
 **
 ** This software may be replicated in part or whole for the licensed use,
 ** with the restriction that this Disclaimer and Copyright notice must be
 ** included with each copy of this software, whether used in part or whole,
 ** at all times.
 **
 ** <hr>
 **
 ** <b>History</b>
 **  - 2012-11-20
 **    - Version 1.0
 **    - Author: CNo
 **    - Comment: First version
 **
 **  - 2012-11-26
 **    - Version 1.1
 **    - Author: CNo
 **    - Comment: Minor improvements on documentation
 **
 **  - 2012-12-18
 **    - Version 1.2
 **    - Author: CNo, YMo, QXu
 **    - Comment: Bugfixes, multicast, improved uIP and LwIP support
 **
 **  - 2013-08-09
 **    - Version 1.3
 **    - Author: CNo
 **    - Comment: Minor Bugfixes,
 **    - Added promiscuous mode for EMAC driver
 **    - Improved support for Flow Control
 **    - License ownership transfered to Spansion Inc.
 **
 **  - 2014-07-15
 **    - Version 1.4
 **    - Author: CNo, TCh
 **    - Comment: Fixed a bug with 10Mbit mode
 **    - Better IRQ support for Ethernet MCUs with and without CAN option
 **    - Selected CMSIS DAP as default debug interface
 **    - MMC counting to prevent lock-up
 **    - MAC address now set in stc_emac_config_t
 **    - Distinction MB9B610 and MB9BD10 now done automatically
 **    - Isolated link-setup routines from auto-negotiate function to reuse it for manual mode selection
 **
 **  - 2014-10-17
 **    - Version 1.5
 **    - Author: CNo
 **    - New function Emac_TxFrameDirect() for optimized throughput.
 **    - Adapted for PDL compatibility and FM4 support
 **    - IRQ error handling improved
 **
 **  - 2015-01-09
 **    - Version 1.6
 **    - Author CNo
 **    - Auto-negotiation feature improved
 **
 **  - 2015-07-27
 **    - Version 1.7
 **    - Author CHNO
 **    - Updated driver to be compliant with template structure verson 2
 **    - Added support for SMSC LAN8710A PHY on SK-FM3-176PMC-ETHERNET
 **    - Integrated ethphy.c into emac.c and ethphy.h into emac.h
 **
 **  - 2015-08-31
 **    - Version 1.8
 **    - Author CHNO
 **    - Bugfixes for operation with IRQ enabled operation
 **
 ** <hr>
 **
 ** <b>Implementations in Ethernet Driver:</b>
 **  - FM3 dual Ethernet hardware (EMAC - Ethernet Media Access Controller)
 **
 ** <hr><br>
 ** <b>Changes in Version 1.1:</b>
 **  - Included Doxygen generated output in package
 **  - L3 compatibility improvements in emac_reg.h
 **
 ** <br><hr>
 ** <b>Changes in Version 1.2:</b>
 **  - Fixed an error in Emac_Autonegotiate which caused reception filters
 **    to be misconfigurated sometimes
 **  - Reception of multicast frames supported
 **  - Introduced an option to enable a work-around for a bug in popular
 **    TCP/IP stacks. This might be activated if Checksum Offload Engine is enabled
 **    and ICMP echo (ping) reply packets carry a wrong checksum.
 **
 ** <br><hr>
 ** <b>Changes in Version 1.3:</b>
 **  - Minor Bugfixes,
 **  - Added promiscuous mode for EMAC driver
 **  - Improved support for Flow Control
 **
 ** <br><hr>
 ** <b>Changes in Version 1.4:</b>
 **    - Fixed a bug with 10Mbit mode in auto-negotiate mode
 **    - Better IRQ support for Ethernet MCUs with and without CAN option
 **    - Selected CMSIS DAP as default debug interface
 **    - MMC counting frozen to prevent core lock-up: The MMC counts Ethernet frames and generates interrupts. If unhandled, core will remain in interrupt handler.
 **    - MAC address now set in stc_emac_config_t instead of hard coded by compiler. This allows more flexible MAC address handling in mass production as device specific addresses can be programmed into designated memory locations rather than modifying the firmware binary. Change in user software necessary: set MAC address in configuration structure that is fed into Emac_Init()!
 **    - Distinction MB9B610 and MB9BD10 now done automatically. As CAN and Ethernet share interrupts, IRQ handler names must be adapted.
 **    - Isolated link-setup routines from auto-negotiate function to reuse it in for manual mode selection (New routine Emac_LinkSetUp(), called by Emac_Autonegotiate() and Emac_SetLinkMode(). No changes in user software necessary.
 **
 ** <br><hr>
 ** <b>Changes in Version 1.5:</b>
 **   - Goal is to have a consistent API for all FM Family microcontrollers. Therefore the PDL (Peripheral Drivers Library) was developed. For this low-level Ethernet driver not much has changed from former versions, the most visible difference is that you have to use PDL_ON and PDL_OFF instead of L3_ON and L3_OFF.
 **   - New function Emac_TxFrameDirect() for optimized throughput. Read the caveats in emac.c doxygen description before using it.
 **   - Error callback called when AIS Interrupt occured
 **   - Emac_TxFrame() and Emac_TxFrameDirect() do not clear interrupt flags anymore. This fixes a situation with missed IRQs under certain circumstances.
 **   - EmacIrqHandler() now clears IRQ cause flags earlier to make callbacks less time critical
 **
 ** <br><hr>
 ** <b>Changes in Version 1.6:</b>
 **   - Improved support for autonegotiation feature. Introduced a new file emac_user.c to make adaption to specific PHY chips easier. That way, users can provide a callback according to PHY vendor's datasheet. This is necessary as 802.3 standardizes autonegotiation start but the registers with results are manufacturer dependent.
 **
 ** <br><hr>
 ** <b>Changes in Version 1.7:</b>
 **  - Updated driver to be compliant with template structure verson 2.
 **  - This step was necessary to be compliant with further developed PDL
 **  - New templates are created with a better automated process and with much improved quality with several errors found and fixed.
 **        - Some name changes to be compliant to current CMSIS specification by ARM
 **            - e.g. stc_emacn_t is renamed to FM_ETHERNET_MAC_TypeDef
 **            - EMAC register bit accesses renamed from e.g. pstcEmac->stcMCR.RE to pstcEmac->MCR_f.RE
 **            - Due to changed register name designators, emac_reg.h became obsolete and was removed.
 **            - These definitions are now in MCU header file (e.g. s6e2ccxl.h)
 **  - Added support for SMSC LAN8710A PHY on SK-FM3-176PMC-ETHERNET
 **  - Integrated ethphy.c into emac.c and ethphy.h into emac.h in order to simplify code structure
 **
 ** <br><hr>
 ** <b>Changes in Version 1.8:</b>
 **  - Some register name changes from new template have not yet been reflected in driver version 1.7. This only affects IRQ operation.
.**
 ** <br>
 ******************************************************************************/
