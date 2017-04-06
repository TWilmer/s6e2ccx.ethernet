/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "mcu.h"

#include "emac.h"

// Board dependent settings
static void ConfigureEthernetPins(void)
{
    // Configure pin settings to use Ethernet functionality
    FM4_GPIO->PFRC |= 0xF7FF; // MAC0 pins
    FM4_GPIO->PFRD |= 0x0007;

    FM4_GPIO->EPFR14 |= (0x7FF << 18);  /// Enable MAC0

    // nRST of PHY: P6A at MCU
    FM4_GPIO->PFR6 &= ~(1u << 0xA);      // GPIO, not special function
    FM4_GPIO->DDR6 |=  (1u << 0xA);      // Set nRST pin as Output

    // INT02_0, PHY Interrupt signal: PA8 at MCU
    // \todo Implement Ethernet PHY IRQ

    // Ethernet user LED: P6E at MCU
    FM4_GPIO->PFR6 &= ~(1u << 0xE);      // GPIO, not special function
    FM4_GPIO->DDR6 |=  (1u << 0xE);      // Set Ethernet user LED pin as Output


 
}



/**
 ******************************************************************************
 ** \brief With this routine you can send an UDP packet without a full-fledged
 **        TCP/IP stack
 **
 ** Here you can write an Ethernet frame into pu8TxBuffer that is sent by MAC
 ** unit with  EMAC_Send() function.
 **
 ** \param  pu8TxBuffer       Buffer containing the complete Ethernet frame
 ** \param  pu8UdpPayload     only the data to be sent
 ** \param  u32PayloadLength  Length of payload data in bytes
 **
 ******************************************************************************/

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
#define MAC0HWADDR0 (0x00)
#define MAC0HWADDR1 (0x01)
#define MAC0HWADDR2 (0x01)
#define MAC0HWADDR3 (0x66)
#define MAC0HWADDR4 (0x73)
#define MAC0HWADDR5 (0x42)

//00:60:6e:58:00:f9

// Destination HW Address
#define DESTMACADDR0 0x00;
#define DESTMACADDR1 0x60;
#define DESTMACADDR2 0x6e;
#define DESTMACADDR3 0x58;
#define DESTMACADDR4 0x00;
#define DESTMACADDR5 0xf9;

// Source IP Address
#define SRCIPADDR0 192;
#define SRCIPADDR1 168;
#define SRCIPADDR2   1;
#define SRCIPADDR3  42;

// Destination IP Address
#define DESTIPADDR0 255;
#define DESTIPADDR1 255;
#define DESTIPADDR2 255;
#define DESTIPADDR3 255;


void TxBufferUDPFill(uint8_t *pu8TxBuffer, uint8_t * pu8UdpPayload, uint32_t u32PayloadLength)
{
	uint32_t u32Index;
	
  // Ethernet header//////////////////////////////////////////////////
  // Destination Address -- now: broadcast
  pu8TxBuffer[0]  = DESTMACADDR0;
  pu8TxBuffer[1]  = DESTMACADDR1;
  pu8TxBuffer[2]  = DESTMACADDR2;
  pu8TxBuffer[3]  = DESTMACADDR3;
  pu8TxBuffer[4]  = DESTMACADDR4;
  pu8TxBuffer[5]  = DESTMACADDR5;

  // Source Address
  pu8TxBuffer[6]  = EMAC0_MAC_ADDRESS0;
  pu8TxBuffer[7]  = EMAC0_MAC_ADDRESS1;
  pu8TxBuffer[8]  = EMAC0_MAC_ADDRESS2;
  pu8TxBuffer[9]  = EMAC0_MAC_ADDRESS3;
  pu8TxBuffer[10] = EMAC0_MAC_ADDRESS4;
  pu8TxBuffer[11] = EMAC0_MAC_ADDRESS5;

  // Type/Length
  pu8TxBuffer[12] = 0x08; // Internet Protocol
  pu8TxBuffer[13] = 0x00;

  // IP header ////////////////////////////////////////////////////////
  pu8TxBuffer[14] = (4 << 4) | 5; // IPv4, Headerlength = 5 32-bit words
  pu8TxBuffer[15] = 0; // Type of Service - pointless as ignored by any router under the sun
  pu8TxBuffer[16] = (20 + 8 + u32PayloadLength) / 255; // total length high
  pu8TxBuffer[17] = (20 + 8 + u32PayloadLength) % 255; // total length low

  pu8TxBuffer[18] = 0; // Identification (fragmentation)
  pu8TxBuffer[19] = 0; // Identification (fragmentation)
  pu8TxBuffer[20] = (1 << 6); // fragmentation: don't fragment bit
  pu8TxBuffer[21] = 0; // fragment offset

  pu8TxBuffer[22] = 255; // time to live
  pu8TxBuffer[23] = 17; // protocol: UDP
  pu8TxBuffer[24] = 0x00; // header checksum, handled by COE (Checksum Offload Engine)
  pu8TxBuffer[25] = 0x00; // header checksum, handled by COE (Checksum Offload Engine)

  pu8TxBuffer[26] = SRCIPADDR0;  // source address
  pu8TxBuffer[27] = SRCIPADDR1;
  pu8TxBuffer[28] = SRCIPADDR2;
  pu8TxBuffer[29] = SRCIPADDR3;

  pu8TxBuffer[30] = DESTIPADDR0;  // destination address
  pu8TxBuffer[31] = DESTIPADDR1;
  pu8TxBuffer[32] = DESTIPADDR2;
  pu8TxBuffer[33] = DESTIPADDR3;

  pu8TxBuffer[24] = 0; // header checksum, handled by COE (Checksum Offload Engine)
  pu8TxBuffer[25] = 0; // header checksum, handled by COE (Checksum Offload Engine)


  // UDP header ///////////////////////////////////////////////////////
  pu8TxBuffer[34] = 0xFF; // source port
  pu8TxBuffer[35] = 0xBB;
  pu8TxBuffer[36] = 0xFF; // destination port
  pu8TxBuffer[37] = 0xAA;

  pu8TxBuffer[38] = 0;    // UDP length high \todo
  pu8TxBuffer[39] = 8 + u32PayloadLength; // UDP length low \todo
  pu8TxBuffer[40] = 0x00; // header checksum \todo
  pu8TxBuffer[41] = 0x00; // header checksum \todo

  // Copy payload
  for (u32Index = 0; u32Index < u32PayloadLength; ++u32Index)
  {
    pu8TxBuffer[42 + u32Index] = pu8UdpPayload[u32Index];
  }
} // TxBufferUDPFill


uint8_t ua8EthBuf[1500];
uint8_t ua8Payload[] = "Konnichiwa World! Welcome to SPANSION Ethernet Driver! [x]Konnichiwa World! Welcome to SPANSION Ethernet Driver! [x]Konnichiwa World! Welcome to SPANSION Ethernet Driver! [x]Konnichiwa World! Welcome to SPANSION Ethernet Driver! [x]Konnichiwa World! Welcome to SPANSION Ethernet Driver! [x]";
#include "gpio/gpio.h"

int main(void)
{



    /* Place your initialization/startup code here (e.g. Drv_Init()) */
  stc_emac_config_t stcEmacConfig;
    static uint32_t u32TxFrames = 0;


  // Define Ethernet MAC 0 Configuration
  PDL_ZERO_STRUCT(stcEmacConfig);
  stcEmacConfig.au8MacAddress[0] = EMAC0_MAC_ADDRESS0;
  stcEmacConfig.au8MacAddress[1] = EMAC0_MAC_ADDRESS1;
  stcEmacConfig.au8MacAddress[2] = EMAC0_MAC_ADDRESS2;
  stcEmacConfig.au8MacAddress[3] = EMAC0_MAC_ADDRESS3;
  stcEmacConfig.au8MacAddress[4] = EMAC0_MAC_ADDRESS4;
  stcEmacConfig.au8MacAddress[5] = EMAC0_MAC_ADDRESS5;



  FM4_FLASH_IF->FBFCR = 0x01;                      /* Trace Buffer enable */


  // Ethernet MAC 0 initialization
  ConfigureEthernetPins();
  Emac_Init(&EMAC0, &stcEmacConfig);
  Emac_Autonegotiate(&EMAC0);
  while(Emac_GetLinkStatus(&EMAC0)!=EMAC_LinkStatusLinkUp);

 
 

    for(;;)
    {
  Gpio1pin_InitOut(GPIO1PIN_PB2, Gpio1pin_InitVal(1u));
	 Gpio1pin_InitOut(GPIO1PIN_P18, Gpio1pin_InitVal(1u));
	
		   TxBufferUDPFill(ua8EthBuf, ua8Payload, sizeof(ua8Payload));
                    ua8EthBuf[42 + sizeof(ua8Payload) - 3] = (u32TxFrames%10 + '0');  // Replace 'x' in string
              en_result_t res;
	//do{
		res=   Emac_TxFrame(&EMAC0, ua8EthBuf, 42 + sizeof(ua8Payload));
        //}while(res==ErrorOperationInProgress);
	if(res==Ok){
	  Gpio1pin_InitOut(GPIO1PIN_PB2, Gpio1pin_InitVal(0u));
	u32TxFrames++;
       }else {
	 Gpio1pin_InitOut(GPIO1PIN_P18, Gpio1pin_InitVal(0u));
	 Emac_Autonegotiate(&EMAC0);
	}
	if(u32TxFrames % 10 ==0)
		 Emac_Autonegotiate(&EMAC0);
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
