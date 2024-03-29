/******************************************************************************
  Filename:        cc112x_easy_link.c
  
  Description:    This program sets up an easy link between two trxEB's with
                  CC112x EM's connected. 
                  The program can take any recomended register settings exported
                  from SmartRF Studio 7 without any modification with exeption 
                  from the assumtions decribed below.
  
  Notes:          The following asumptions must be fulfilled for the program
                  to work:
                  
                  1. GPIO3 has to be set up with GPIOx_CFG = 0x06
                     PKT_SYNC_RXTX for correct interupt
                  2. Packet engine has to be set up with status bytes enabled 
                     PKT_CFG1.APPEND_STATUS = 1
  
******************************************************************************/


/*****************************************************************************
* INCLUDES
*/

#include "System.h"	 //"系统" 常用功能函数集-头文件

/////////////////////////////////
#include "msp430.h"

#include "hal_spi_rf_trxeb.h"

#include "cc112x_spi.h"
#include "stdlib.h"
#include "cc112x_easy_link.h"  //"CC112x 低功耗无线射频收发器"-驱动程序(外部资源)-头文件



/******************************************************************************
 * CONSTANTS
 */ 

/******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0

//#define PKTLEN              15 // Packet length has to be within fifo limits ( 1 - 127 bytes)

#define RX_FIFO_ERROR       0x11


////////////////////////////////////////////////////////////////////////////
//==**全局变量定义**Global variables**========================//
////////////////////////////////////////////////////////////////////////////
unsigned int CC112x_RX_PacketCnt = 0;
unsigned int CC112x_TX_PacketCnt = 0;
unsigned char CC112x_TxBuf[128] = {0}; //  发送缓冲区总字节数<128



/******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore = 0;


/****************************************************************************
*函数名-Function:	static void Initial_GPIO_Interrupt(void)
*描述- Description:		初始化: SPI串行接口(CC112x)
*输入参数-Input:	None
*输出参数-output:	None
*注意事项-Note：	▲01)    	▲02)    	▲03)    ▲04)  
*****************************************************************************/
static void Initial_SPI_CC112x(void)  //初始化: SPI串行接口(CC112x)
{
	//初始化SPI 串行接口
		// Instantiate tranceiver RF spi interface to SCLK ~ 4 MHz */
		//input clockDivider - SMCLK/clockDivider gives SCLK frequency
				//trxRfSpiInterfaceInit(0x02);
	trxRfSpiInterfaceInit(0x01);

////////////////////////////////////////////////////////////////////////////
//care--下面程序，只适用于MSP430F6638实验板----------------//
	//--care--一定要加上这个语句//"LCD_B 端口控制寄存器"相关配置
	LCDBPCTL0 = 0;

		//配置多路复用器TS3A5017: 选择IN2 、IN1
		//IN2=0; IN1=1; 对应SPI_B 引脚(CLK_B; SPI_MOSI_B; SPI_MISO_B)
	mConfig_TS3A5017_01_IN2(0);  //IN2 = 0
	mConfig_TS3A5017_01_IN1(1);  //IN1 = 1
}



/****************************************************************************
*函数名-Function:	static void Initial_GPIO_Int_CC112x(void)
*描述- Description:		初始化:  GPIO电平中断(CC112x)
*输入参数-Input:	None
*输出参数-output:	None
*注意事项-Note：	▲01)    	▲02)    	▲03)    ▲04)  
*****************************************************************************/
static void Initial_GPIO_Int_CC112x(void)  //初始化:  GPIO电平中断
{
	TRXEM_GPIO3_SEL &= ~ TRXEM_GPIO3_IO;  //配置引脚为"IO"引脚			
	TRXEM_GPIO3_DIR &= ~ TRXEM_GPIO3_IO;  //配置引脚为"输入"
	TRXEM_GPIO3_IES |= TRXEM_GPIO3_IO;	  // 下降沿时置位各自的PxIFGx标志
	TRXEM_GPIO3_IFG &= ~ TRXEM_GPIO3_IO;  // 清除中断标志位
	TRXEM_GPIO3_IE |= TRXEM_GPIO3_IO;	  // 打开 P1.0 中断
}


/****************************************************************************
*函数名-Function:	void Initial_CC112x(void)
*描述- Description:		初始化设置:  "CC112x 低功耗无线射频收发器"
*输入参数-Input:	None
*输出参数-output:	None
*注意事项-Note：	▲01)    	▲02)    	▲03)    ▲04)  
*****************************************************************************/
void Initial_CC112x(void)  //初始化设置:  "CC112x 低功耗无线射频收发器"
{
////////////////////////////////////////////////////////////////////////////
//==**SPI_B 模块初始化--接到CC112x的SPI引脚**===//
	Initial_SPI_CC112x();  //初始化: SPI串行接口(CC112x)

////////////////////////////////////////////////////////////////////////////
//==**初始化:  GPIO电平中断--CC112x的GPIO3作为中断源**===//
	Initial_GPIO_Int_CC112x();  //初始化:  GPIO电平中断

////////////////////////////////////////////////////////////////////////////
//==**初始化CC112x的配置寄存器--Write radio registers**=========//
	registerConfig(); 

////////////////////////////////////////////////////////////////////////////
//==**根据勘误表校准CC112x--Calibrate radio according to errata**=====//
	manualCalibration();

////////////////////////////////////////////////////////////////////////////
//==**初始配置CC112x 处于接收状态--Set radio in RX**==========//
	trxSpiCmdStrobe(CC112X_SRX);
}



/****************************************************************************
*函数名-Function:	void Deal_RX_CC112x(void)
*描述- Description:		处理接收部分"CC112x 低功耗无线射频收发器"
*输入参数-Input:	None
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	▲02) 	▲03) 	▲04)  
*****************************************************************************/
void Deal_RX_CC112x(void)  //处理接收部分"CC112x 低功耗无线射频收发器"
{
	unsigned char rxBuffer[128] = {0};
	unsigned char rxBytes;
	unsigned char marcStatus;


	//判断CC112x是否接收到数据 //Wait for packet received interrupt 
	//当接收成功一个数据包时，相应的GPIOx产生"下降沿中断"(此驱动选用GPIO3)
	if(packetSemaphore == ISR_ACTION_REQUIRED)
	{
		//读出接收到的数据字节数// Read number of bytes in rx fifo
		cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);

		//如果字节数不为0，则从RX FIFO读出接收到的数据// Check that we have bytes in fifo
		if(rxBytes != 0)
		{
			//读取MARCSTATE寄存值，查看是否发生"接收错误"// Read marcstate to check for RX FIFO error
			cc112xSpiReadReg(CC112X_MARCSTATE, &marcStatus, 1);

			//如果发生"接收错误"，则清空RX FIFO缓冲区// Mask out marcstate bits and check if we have a RX FIFO error
			if((marcStatus & 0x1F) == RX_FIFO_ERROR)
			{
				//清空RX FIFO缓冲区// Flush RX Fifo
				trxSpiCmdStrobe(CC112X_SFRX);
			}
			else   //没有发生"接收错误"，则从RX FIFO缓冲区读出接收到的n个数据
			{
				//从RX FIFO读出接收到的n个数据// Read n bytes from rx fifo
				cc112xSpiReadRxFifo(rxBuffer, rxBytes);  

				// RX FIFO 最后两个字节为CRC校验字
				// Check CRC ok (CRC_OK: bit7 in second status byte)
				// This assumes status bytes are appended in RX_FIFO
				// (PKT_CFG1.APPEND_STATUS = 1.)
				// If CRC is disabled the CRC_OK field will read 1
				//校验字的最后一个字节的最高位为CRC_OK
				//CRC_OK=1时，表示数据包正确接收。
				if(rxBuffer[rxBytes-1] & 0x80)
				{
					// Update packet counter
					CC112x_RX_PacketCnt++;
				}
			}
		}


		//处理完接收到的数据包，复位数据包状态变量// Reset packet semaphore
		packetSemaphore = ISR_IDLE;

		//配置CC112x 处于接收状态// Set radio back in RX
		trxSpiCmdStrobe(CC112X_SRX);

	}


}


/****************************************************************************
*函数名-Function:	void TX_String_CC112x(void)
*描述- Description:		CC112x发送数据(字符串)-- "CC112x 低功耗无线射频收发器"
*输入参数-Input:	pData: 要发送的字符串数组;  len:发送的总字节数
*输出参数-output:	None
*注意事项-Note： 
	▲01) len <= 128
	▲02) 	▲03) 	▲04)  
*****************************************************************************/
void TX_String_CC112x(unsigned char *pData, unsigned char len) //CC112x发送数据(字符串)-- "CC112x 低功耗无线射频收发器"
{
	//将要发送的数据，写入TX FIFO 缓冲器// Write packet to tx fifo
	cc112xSpiWriteTxFifo(pData,len);
	
	//配置CC112x为发送状态，启动发送// Strobe TX to send packet
	trxSpiCmdStrobe(CC112X_STX);
	
	// Wait for interrupt that packet has been sent. 
	// (Assumes the GPIO connected to the radioRxTxISR function is set 
	// to GPIOx_CFG = 0x06)
	while(!packetSemaphore);  //等待数据包发送完成
	
	//发送完数据包，复位数据包状态变量// Clear semaphore flag
	packetSemaphore = ISR_IDLE;
	
	//CC112x 发送完数据包后，返回接收状态// Set radio in RX
	trxSpiCmdStrobe(CC112X_SRX);


}



/****************************************************************************
*函数名-Function:	void Config_FrequencyBand(unsigned char bandKind)
*描述- Description:		配置:频段--"CC112x 低功耗无线射频收发器"
*输入参数-Input:	bandKind
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	默认配置为410.0 - 480.0 MHz频段
	▲02) 	1<= bandKind <=6
	▲03) 	▲04)  
*****************************************************************************/
void Config_FrequencyBand(unsigned char bandKind)  //配置:频段--"CC112x 低功耗无线射频收发器"
{
	unsigned char bandSelect = 0;


	switch(bandKind) 
	{
		case CC112X_820_band:  //0010=820.0 - 960.0 MHz band
			bandSelect = 0x02;
			break;	

		case CC112X_410_band: //0100=410.0 - 480.0 MHz band
			bandSelect = 0x04;
			break;	
			
		case CC112X_273_3_band: //0110=273.3 - 320.0 MHz band 
			bandSelect = 0x06;
			break;		

		case CC112X_205_band: //1000=205.0 - 240.0 MHz band 
			bandSelect = 0x08;
			break;
			
		case CC112X_164_band: //1010=164.0 - 192.0 MHz band 
			bandSelect = 0x0A;
			break;		
		
		case CC112X_136_7_band: //1011=136.7 - 160.0 MHz band
			bandSelect = 0x0B;
			break;

////////////////////////////////////////////////////////////////////////////
		default:    //默认配置为410.0 - 480.0 MHz频段
			bandSelect = 0x04;  //0100=410.0 - 480.0 MHz band
			break;
	}

	//修改相应的寄存器值
	cc112xSpiWriteReg(CC112X_FS_CFG, &bandSelect, 1);  //频率合成器的配置

}


/****************************************************************************
*函数名-Function:	void Config_CarrierFrequency(unsigned char frenquency_2,unsigned char frenquency_1,unsigned char frenquency_0)
*描述- Description:		配置:载波频率--"CC112x 低功耗无线射频收发器"
*输入参数-Input:	frenquency_2,frenquency_1,frenquency_0
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	只适用于410.0 - 480.0 MHz频段
	▲02) 	载波频率的计算公式，请参阅CC112x User's Guide
	▲03) 	434Mhz对应的寄存器值CC112X_FREQ2=0x6C，CC112X_FREQ1=0x80，CC112X_FREQ0=0x00
	▲04)  
*****************************************************************************/
void Config_CarrierFrequency(unsigned char frenquency_2,unsigned char frenquency_1,unsigned char frenquency_0)  //配置:载波频率--"CC112x 低功耗无线射频收发器"
{
	//修改相应的寄存器值
	cc112xSpiWriteReg(CC112X_FREQ2, &frenquency_2, 1);  //载波频率配置
	cc112xSpiWriteReg(CC112X_FREQ1, &frenquency_1, 1);
	cc112xSpiWriteReg(CC112X_FREQ0, &frenquency_0, 1);

}


/****************************************************************************
*函数名-Function:	void Config_RxFilterBW(unsigned char rxFilterBW)
*描述- Description:		配置:接收滤波器带宽--"CC112x 低功耗无线射频收发器"
*输入参数-Input:	rxFilterBW
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	接收滤波器带宽的计算公式，请参阅CC112x User's Guide
	▲02) 	对于CC1120，接收滤波器带宽25kbps 对应的寄存器值CC112X_CHAN_BW=0x08
	▲03) 	  Bite rate(DataRate) < 	RX filter BW /2
	▲04)  
*****************************************************************************/
void Config_RxFilterBW(unsigned char rxFilterBW)  //配置:接收滤波器带宽--"CC112x 低功耗无线射频收发器"
{
	//修改相应的寄存器值
	cc112xSpiWriteReg(CC112X_CHAN_BW, &rxFilterBW, 1);  //信道滤波器配置
}


/****************************************************************************
*函数名-Function:	void Config_DataRate(unsigned char dataRate_2,unsigned char dataRate_1,unsigned char dataRate_0)
*描述- Description:		配置:数据速率--"CC112x 低功耗无线射频收发器"
*输入参数-Input:	dataRate_2,dataRate_1,dataRate_0
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	数据速率的计算公式，请参阅CC112x User's Guide
	▲02) 	1.2kbps 对应的寄存器值CC112X_DRATE2=0x43，CC112X_DRATE1=0xA9，CC112X_DRATE0=0x2A
	▲03) 	  Bite rate(DataRate) < 	RX filter BW /2
	▲04)  
*****************************************************************************/
void Config_DataRate(unsigned char dataRate_2,unsigned char dataRate_1,unsigned char dataRate_0)  //配置:数据速率--"CC112x 低功耗无线射频收发器"
{
	//修改相应的寄存器值
	cc112xSpiWriteReg(CC112X_DRATE2, &dataRate_2, 1);  //数据速率配置
	cc112xSpiWriteReg(CC112X_DRATE1, &dataRate_1, 1);
	cc112xSpiWriteReg(CC112X_DRATE0, &dataRate_0, 1);

}


/****************************************************************************
*函数名-Function:	void Config_ModulationFormat_Deviation(unsigned char modulation,unsigned char deviation)
*描述- Description:		配置:调制格式和频率偏差--"CC112x 低功耗无线射频收发器"
*输入参数-Input:	modulation,deviation
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	频率偏差的计算公式，请参阅CC112x User's Guide
	▲02) 	MODCFG_DEV_E-调制格式和频率偏差配置//复位值=0x03
				//MODEM_MODE 调制解调器模式配置= 00 =普通模式
				//MOD_FORMAT 调制格式=000= "2-FSK"    //001=2-GFSK //010=Reserved //011=ASK/OOK //100=4-FSK //101=4-GFSK //110=SC-MSK unshaped (CC1125, TX only). For CC1120, CC1121, and CC1175this setting is reserved  //111=SC-MSK shaped (CC1125, TX only). For CC1120, CC1121, and CC1175 thissetting is reserved
				//DEV_E  频率偏差（指数部分）= 011
	▲03) 	默认MOD_FORMAT 调制格式=000= "2-FSK", 频率偏差= 3.99780 kHz
	               对应的modulation=0x00，deviation=0x03
	▲04)  
*****************************************************************************/
void Config_ModulationFormat_Deviation(unsigned char modulation,unsigned char deviation)  //配置:调制格式和频率偏差--"CC112x 低功耗无线射频收发器"
{
	unsigned char configData;

	switch(modulation) 
	{
		case CC112X_2_FSK_mode:	//bit5:3=000="2-FSK"
			configData = 0x00;
			break;	

		case CC112X_2_GFSK_mode: //bit5:3=001=2-GFSK 
			configData = 0x08;
			break;	

		case CC112X_ASK_OOK_mode: //bit5:3=011=ASK/OOK
			configData = 0x18;
			break;	
			
		case CC112X_4_FSK_mode: //bit5:3=100=4-FSK
			configData = 0x20;
			break;	
			
		case CC112X_4_GFSK_mode: //bit5:3=101=4-GFSK  
			configData = 0x28;
			break;	

////////////////////////////////////////////////////////////////////////////
		default:  //默认MOD_FORMAT 调制格式=000= "2-FSK"
			configData = 0x00; //bit5:3=000="2-FSK"
			break;
	}

	configData += deviation;

	//修改相应的寄存器值
	cc112xSpiWriteReg(CC112X_MODCFG_DEV_E, &configData, 1);  //数据速率配置
}



/****************************************************************************
*函数名-Function:	void Config_PA_TxPower(unsigned char txPower)
*描述- Description:		配置:发射功率(功率放大器)--"CC112x 低功耗无线射频收发器"
*输入参数-Input:	txPower
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	默认发射功率 = 15 dBm  (最大功率)
	▲02) 		▲03) 		▲04)  
*****************************************************************************/
void Config_PA_TxPower(unsigned char txPower)  //配置:发射功率(功率放大器)--"CC112x 低功耗无线射频收发器"
{
	unsigned char configData;

	switch(txPower) 
	{
		case CC112X_15dBm_TxPower:	//0x7F=发射功率= 15dBm
			configData = 0x7F;
			break;	

		case CC112X_14dBm_TxPower: //0x7D=发射功率= 14dBm
			configData = 0x7D;
			break;	

		case CC112X_13dBm_TxPower: //0x7B=发射功率= 13dBm
			configData = 0x7B;
			break;	
			
		case CC112X_12dBm_TxPower: //0x79=发射功率= 12dBm
			configData = 0x79;
			break;	

		case CC112X_11dBm_TxPower: //0x77=发射功率= 11dBm
			configData = 0x77;
			break;	
			
		case CC112X_10dBm_TxPower: //0x74=发射功率= 10dBm
			configData = 0x74;
			break;	
		
		case CC112X_09dBm_TxPower: //0x72=发射功率= 09dBm
			configData = 0x72;
			break;	
			
		case CC112X_08dBm_TxPower: //0x6F=发射功率= 08dBm
			configData = 0x6F;
			break;	
				
		case CC112X_07dBm_TxPower: //0x6D=发射功率= 07dBm
			configData = 0x6D;
			break;	
		
		case CC112X_06dBm_TxPower: //0x6B=发射功率= 06dBm
			configData = 0x6B;
			break;	
			
		case CC112X_05dBm_TxPower: //0x69=发射功率= 05dBm
			configData = 0x69;
			break;	

		case CC112X_04dBm_TxPower: //0x66=发射功率= 04dBm
			configData = 0x66;
			break;	
			
		case CC112X_03dBm_TxPower: //0x64=发射功率= 03dBm
			configData = 0x64;
			break;	
		
		case CC112X_02dBm_TxPower: //0x62=发射功率= 02dBm 
			configData = 0x62;
			break;	
			
		case CC112X_01dBm_TxPower: //0x5F=发射功率= 01dBm
			configData = 0x5F;
			break;	
			
		case CC112X_00dBm_TxPower: //0x5D=发射功率= 00dBm 
			configData = 0x5D;
			break;	
	
		case CC112X__3dBm_TxPower: //0x56=发射功率= -3dBm
			configData = 0x56;
			break;	
			
		case CC112X__6dBm_TxPower: //0x4F=发射功率= -6dBm
			configData = 0x4F;
			break;	
			
		case CC112X__11dBm_TxPower: //0x43=发射功率= -11dBm
			configData = 0x43;
			break;	

////////////////////////////////////////////////////////////////////////////
		default:  //默认发射功率 = 15 dBm  (最大功率)
			configData = 0x7F;  //0x7F=发射功率= 15dBm
			break;
	}

	//修改相应的寄存器值
	cc112xSpiWriteReg(CC112X_PA_CFG2, &configData, 1);  //功率放大器配置
}



/****************************************************************************
*函数名-Function:	void Config_DeviceAddress(unsigned char deviceAddress) 
*描述- Description:		配置:设备地址--"CC112x 低功耗无线射频收发器"
*输入参数-Input:	deviceAddress
*输出参数-output:	None
*注意事项-Note： 
	▲01) 	默认设备地址Device Address = 0 
	▲02) 	设备地址为1个字节
	▲03)  	在接收数据包过滤时使用的地址
	▲04)  根据CC112X_PKT_CFG1.ADDR_CHECK_CFG位段的值，决定是否“使能地址检查”
*****************************************************************************/
void Config_DeviceAddress(unsigned char deviceAddress)  //配置:设备地址--"CC112x 低功耗无线射频收发器"
{
	//修改相应的寄存器值
	cc112xSpiWriteReg(CC112X_DEV_ADDR, &deviceAddress, 1);  //设备地址配置
}






/****************************************************************************
*函数名-Function:	void TX_String_CC112x(void)
*描述- Description:		CC112x发送数据(字符串)-- "CC112x 低功耗无线射频收发器"
*输入参数-Input:	pData: 要发送的字符串数组;  len:发送的总字节数
*输出参数-output:	None
*注意事项-Note： 
	▲01)   	▲02) 	▲03) 	▲04)  
******************************
void TX_String_CC112x(unsigned char *pData, unsigned char len) //CC112x发送数据(字符串)-- "CC112x 低功耗无线射频收发器"
{

	
	// Create a random packet with PKTLEN + 2 byte packet counter + n x random bytes
	createPacket(txBuffer);
	
	// Write packet to tx fifo
	cc112xSpiWriteTxFifo(txBuffer,sizeof(txBuffer));
	
	//配置CC112x为发送状态// Strobe TX to send packet
	trxSpiCmdStrobe(CC112X_STX);
	
	// Wait for interrupt that packet has been sent. 
	// (Assumes the GPIO connected to the radioRxTxISR function is set 
	// to GPIOx_CFG = 0x06)
	while(!packetSemaphore);
	
	//发送完数据包，复位数据包状态变量// Clear semaphore flag
	packetSemaphore = ISR_IDLE;
	
	//CC112x 发送完数据包后，返回接收状态// Set radio in RX
	trxSpiCmdStrobe(CC112X_SRX);


}
***********************************************/


/*******************************************************************************
* @fn          registerConfig
*
* @brief       Write register settings as given by SmartRF Studio found in
*              cc112x_easy_link_reg_config.h
*
* @param       none
*
* @return      none
*/
static void registerConfig(void){
  
  uint8 writeByte;
  
  // Reset radio
  trxSpiCmdStrobe(CC112X_SRES);
  
  // Write registers to radio
  for(uint16 i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) 
  {  
    writeByte =  preferredSettings[i].data; 
    cc112xSpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
  }
}
/******************************************************************************
 * @fn          createPacket
 *
 * @brief       This function is called before a packet is transmitted. It fills
 *              the txBuffer with a packet consisting of a length byte, two
 *              bytes packet counter and n random bytes.
 *
 *              The packet format is as follows:
 *              |--------------------------------------------------------------|
 *              |           |           |           |         |       |        |
 *              | pktLength | pktCount1 | pktCount0 | rndData |.......| rndData|
 *              |           |           |           |         |       |        |
 *              |--------------------------------------------------------------|
 *               txBuffer[0] txBuffer[1] txBuffer[2]  ......... txBuffer[PKTLEN]
 *                
 * @param       pointer to start of txBuffer
 *
 * @return      none

static void createPacket(uint8 txBuffer[]){
  
  txBuffer[0] = PKTLEN;                     // Length byte
//  txBuffer[1] = (uint8) (packetCounter >> 8); // MSB of packetCounter
//  txBuffer[2] = (uint8) packetCounter;      // LSB of packetCounter
  
  // Fill rest of buffer with random bytes
  for(uint8 i =3; i< (PKTLEN+1); i++)
  {
    txBuffer[i] = (uint8)rand();
  }
}
*/


/******************************************************************************
 * @fn          manualCalibration
 *
 * @brief       calibrates radio according to CC112x errata
 *                
 * @param       none
 *
 * @return      none
 */
#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2
static void manualCalibration(void) {
  
    uint8 original_fs_cal2;
    uint8 calResults_for_vcdac_start_high[3];
    uint8 calResults_for_vcdac_start_mid[3];
    uint8 marcstate;
    uint8 writeByte;
    
    // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    
    // 2) Start with high VCDAC (original VCDAC_START + 2):
    cc112xSpiReadReg(CC112X_FS_CAL2, &original_fs_cal2, 1);
    writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);
    
    // 3) Calibrate and wait for calibration to be done (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);
    
    do 
    {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);
    
    // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with high VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2, &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4, &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP, &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);
    
    // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    
    // 6) Continue with mid VCDAC (original VCDAC_START):
    writeByte = original_fs_cal2;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);
    
    // 7) Calibrate and wait for calibration to be done (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);
    
    do 
    {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);
    
    // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with mid VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2, &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4, &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP, &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);
    
    // 9) Write back highest FS_VCO2 and corresponding FS_VCO and FS_CHP result
    if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] > calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) 
    {
        writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    }
    else 
    {
        writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    }
}


// PORT1 中断服务程序 PORT1 interrupt service routine 
#pragma vector=PORT1_VECTOR
__interrupt void port_1(void)
{
	// Set packet semaphore
	packetSemaphore = ISR_ACTION_REQUIRED;	
	
	TRXEM_GPIO3_IFG &= ~ TRXEM_GPIO3_IO; //清除中断标志位  Clear isr flag
	mNop;
}



/***********************************************************************************
  Copyright 2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
