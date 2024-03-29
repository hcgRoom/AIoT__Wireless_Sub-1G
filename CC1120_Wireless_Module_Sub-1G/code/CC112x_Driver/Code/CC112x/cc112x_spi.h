/******************************************************************************
    Filename: CC112x_spi.h  
    
    Description: header file that defines a minimum set of neccessary functions
                 to communicate with CC112X over SPI as well as the regsister 
                 mapping. 
*******************************************************************************/

#ifndef CC112x_SPI_H
#define CC112x_SPI_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "hal_spi_rf_trxeb.h"

/******************************************************************************
 * CONSTANTS
 */
 
////////////////////////////////////////////////////////////////////////////
//配置寄存器/* configuration registers */
#define CC112X_IOCFG3                   0x0000  //IOCFG3 -GPIO3引脚配置//复位值=0x06 (复位时选"PKT_SYNC_RXTX" )
#define CC112X_IOCFG2                   0x0001	//IOCFG2 -GPIO2引脚配置//复位值=0x07(复位时选"PKT_CRC_OK" )
#define CC112X_IOCFG1                   0x0002	//IOCFG1 -GPIO1引脚配置//复位值=0x30(复位时选"HIGHZ"=  高阻抗 )
#define CC112X_IOCFG0                   0x0003	//IOCFG0 -GPIO0引脚配置//复位值=0x3C(复位时选"EXT_OSC_EN"=  使能外部振荡器 )

	//SYNC0 ~ SYNC3-32位同步字
#define CC112X_SYNC3                    0x0004	//SYNC3 //复位值=0x93
#define CC112X_SYNC2                    0x0005	//SYNC2 //复位值=0x0B
#define CC112X_SYNC1                    0x0006	//SYNC1 //复位值=0x51
#define CC112X_SYNC0                    0x0007	//SYNC0 //复位值=0xDE

#define CC112X_SYNC_CFG1                0x0008	//SYNC_CFG1-同步字检测配置//复位值=0x0A
#define CC112X_SYNC_CFG0                0x0009	//SYNC_CFG0 - 同步字长度配置//复位值=0x17H

#define CC112X_DEVIATION_M              0x000A	//DEVIATION_M -频率偏差配置//复位值=0x06
#define CC112X_MODCFG_DEV_E             0x000B	//MODCFG_DEV_E-调制格式和频率偏差配置//复位值=0x03

#define CC112X_DCFILT_CFG               0x000C	//DCFILT_CFG-数字直流去除配置//复位值=0x4C

#define CC112X_PREAMBLE_CFG1            0x000D	//PREAMBLE_CFG1-前导码长度配置//复位值=00010100
#define CC112X_PREAMBLE_CFG0            0x000E	//PREAMBLE_CFG0-前导码长度配置//复位值=00101010

#define CC112X_FREQ_IF_CFG              0x000F	//FREQ_IF_CFG-RX混频器频率配置//复位值=0x40
#define CC112X_IQIC                     0x0010	//IQIC-数字图像信道补偿配置//复位值=11000100

#define CC112X_CHAN_BW                  0x0011	//CHAN_BW-信道滤波器配置//复位值=0x14	

#define CC112X_MDMCFG1                  0x0012	//MDMCFG1-通用调制解调器参数配置//复位值=01000110
#define CC112X_MDMCFG0                  0x0013	//MDMCFG0-通用调制解调器参数配置//复位值=00001101

#define CC112X_DRATE2                   0x0014	//DRATE2-数据速率配置指数和尾数//复位值=0x43
#define CC112X_DRATE1                   0x0015	//DRATE1-数据速率配置尾数[15:8]//复位值=0xA9
#define CC112X_DRATE0                   0x0016	//DRATE0-数据速率配置尾数[7:0]//复位值=0x2A

#define CC112X_AGC_REF                  0x0017	//AGC_REF-AGC参考电平配置//复位值=0x36	
#define CC112X_AGC_CS_THR               0x0018	//AGC_CS_THR-载波检测阈值配置//复位值=0x00
#define CC112X_AGC_GAIN_ADJUST          0x0019	//AGC_GAIN_ADJUST-RSSI偏移配置//复位值=0x00
#define CC112X_AGC_CFG3                 0x001A	//AGC_CFG3-AGC配置//复位值=10010001
#define CC112X_AGC_CFG2                 0x001B	//AGC_CFG2-AGC配置//复位值=00100000
#define CC112X_AGC_CFG1                 0x001C	//AGC_CFG1-AGC配置//复位值=10101010=0xAA
#define CC112X_AGC_CFG0                 0x001D	//AGC_CFG0-AGC配置//复位值=11000011=0xC3	

#define CC112X_FIFO_CFG                 0x001E	//FIFO_CFG-FIFO配置//复位值=0x80

#define CC112X_DEV_ADDR                 0x001F	//DEV_ADDR-设备地址配置//复位值=0x00

#define CC112X_SETTLING_CFG             0x0020	//SETTLING_CFG-//复位值=00001011

#define CC112X_FS_CFG                   0x0021	//FS_CFG-频率合成器的配置//复位值=0x02

#define CC112X_WOR_CFG1                 0x0022	//WOR_CFG1-eWOR配置//复位值=00001000
#define CC112X_WOR_CFG0                 0x0023	//WOR_CFG0-eWOR配置//复位值=00100001
#define CC112X_WOR_EVENT0_MSB           0x0024	//WOR_EVENT0_MSB-事件0配置//复位值=0x00
#define CC112X_WOR_EVENT0_LSB           0x0025	//WOR_EVENT0_LSB-事件0配置//复位值=0x00

#define CC112X_PKT_CFG2                 0x0026	//PKT_CFG2-包配置//复位值=00000100
#define CC112X_PKT_CFG1                 0x0027	//PKT_CFG1-包配置//复位值=00000101
#define CC112X_PKT_CFG0                 0x0028	//PKT_CFG0-包配置//复位值=0x00

#define CC112X_RFEND_CFG1               0x0029	//RFEND_CFG1 -RFEND配置//复位值=00001111
#define CC112X_RFEND_CFG0               0x002A	//RFEND_CFG0 -RFEND配置//复位值=0x00

#define CC112X_PA_CFG2                  0x002B	//PA_CFG2 -功率放大器配置//复位值=0111111
#define CC112X_PA_CFG1                  0x002C	//PA_CFG1 -功率放大器配置//复位值=01010110
#define CC112X_PA_CFG0                  0x002D	//PA_CFG0 -功率放大器配置//复位值=01111100

#define CC112X_PKT_LEN                  0x002E	//PKT_LEN-数据包长度配置//复位值=0x03

////////////////////////////////////////////////////////////////////////////
//扩展配置寄存器部分/* Extended Configuration Registers */
#define CC112X_IF_MIX_CFG               0x2F00	//IF_MIX_CFG-IF混合配置//复位值=0x04
#define CC112X_FREQOFF_CFG              0x2F01	//FREQOFF_CFG-频率偏移纠正配置//复位值=0x20=00100000
#define CC112X_TOC_CFG                  0x2F02	//TOC_CFG -定时偏移校正配置//复位值=00001011
#define CC112X_MARC_SPARE               0x2F03	//MARC_SPARE -MARC备用//复位值=0x00
#define CC112X_ECG_CFG                  0x2F04	//ECG_CFG -外部时钟频率配置//复位值=0x00
#define CC112X_SOFT_TX_DATA_CFG         0x2F05	//SOFT_TX_DATA_CFG -软件配置TX数据//复位值=0x00
#define CC112X_EXT_CTRL                 0x2F06	//EXT_CTRL -外部控制配置//复位值=0x01

#define CC112X_RCCAL_FINE               0x2F07	//RCCAL_FINE-RC振荡器校准（精）//复位值=0x00
#define CC112X_RCCAL_COARSE             0x2F08	//RCCAL_COARSE-RC振荡器校准（粗）//复位值=0x00
#define CC112X_RCCAL_OFFSET             0x2F09	//RCCAL_OFFSET-RC振荡器校准时钟偏移//复位值=0x00

#define CC112X_FREQOFF1                 0x2F0A	//FREQOFF1 - 频率偏移（MSB）//复位值=0x00
#define CC112X_FREQOFF0                 0x2F0B	//FREQOFF0 - 频率偏移（LSB）//复位值=0x00

#define CC112X_FREQ2                    0x2F0C	//FREQ2-频率配置[23:16]//复位值=0x00
#define CC112X_FREQ1                    0x2F0D	//FREQ1-频率配置[23:16]//复位值=0x00
#define CC112X_FREQ0                    0x2F0E	//FREQ0 - 频率配置[7:0]//复位值=0x00

#define CC112X_IF_ADC2                  0x2F0F	//IF_ADC2-模拟到数字转换器配置//复位值=0x02
#define CC112X_IF_ADC1                  0x2F10	//IF_ADC1-模拟到数字转换器配置//复位值=0xA6
#define CC112X_IF_ADC0                  0x2F11	//IF_ADC0-模拟到数字转换器配置//复位值=0x04

#define CC112X_FS_DIG1                  0x2F12	//FS_DIG1-//复位值=0x08
#define CC112X_FS_DIG0                  0x2F13	//FS_DIG0-//复位值=01011010
#define CC112X_FS_CAL3                  0x2F14	//FS_CAL3 -//复位值=0x00
#define CC112X_FS_CAL2                  0x2F15	//FS_CAL2 -//复位值=0x20
#define CC112X_FS_CAL1                  0x2F16	//FS_CAL1-//复位值=0x00
#define CC112X_FS_CAL0                  0x2F17	//FS_CAL0-//复位值=0x00
#define CC112X_FS_CHP                   0x2F18	//FS_CHP - 电荷泵配置//复位值=0x28
#define CC112X_FS_DIVTWO                0x2F19	//FS_DIVTWO-除以2//复位值=0x01
#define CC112X_FS_DSM1                  0x2F1A	//FS_DSM1- 数字频率合成器模块配置//复位值=0x00
#define CC112X_FS_DSM0                  0x2F1B	//FS_DSM0-数字频率合成器模块配置//复位值=0x03
#define CC112X_FS_DVC1                  0x2F1C	//FS_DVC1 - 分频器链配置//复位值=0xFF
#define CC112X_FS_DVC0                  0x2F1D	//FS_DVC0 - 分频器链配置//复位值=0x1F
#define CC112X_FS_LBI                   0x2F1E	//FS_LBI - 本地偏置配置//复位值=0x00
#define CC112X_FS_PFD                   0x2F1F	//FS_PFD - 相位频率检测器配置//复位值=0x51
#define CC112X_FS_PRE                   0x2F20	//FS_PRE - 预分频器配置//复位值=0x2C
#define CC112X_FS_REG_DIV_CML           0x2F21	//FS_REG_DIV_CML- //复位值=0x11
#define CC112X_FS_SPARE                 0x2F22	//FS_SPARE- //复位值=0x00
#define CC112X_FS_VCO4                  0x2F23	//FS_VCO4-//复位值=0x14
#define CC112X_FS_VCO3                  0x2F24	//FS_VCO3-//复位值=0x00
#define CC112X_FS_VCO2                  0x2F25	//FS_VCO2-//复位值=0x00
#define CC112X_FS_VCO1                  0x2F26	//FS_VCO1-//复位值=0x00
#define CC112X_FS_VCO0                  0x2F27	//FS_VCO0- //复位值=10000001

	//GBIAS6～GBIAS0- 通用偏置配置
#define CC112X_GBIAS6                   0x2F28	//GBIAS6 //复位值=0x00
#define CC112X_GBIAS5                   0x2F29	//GBIAS5 //复位值=0x02
#define CC112X_GBIAS4                   0x2F2A	//GBIAS4 //复位值=0x00
#define CC112X_GBIAS3                   0x2F2B	//GBIAS3 //复位值=0x00
#define CC112X_GBIAS2                   0x2F2C	//GBIAS2 //复位值=0x10
#define CC112X_GBIAS1                   0x2F2D	//GBIAS1 //复位值=0x00
#define CC112X_GBIAS0                   0x2F2E	//GBIAS0 //复位值=0x00

#define CC112X_IFAMP                    0x2F2F	//IFAMP-中频放大器配置//复位值=0x01
#define CC112X_LNA                      0x2F30	//LNA -低噪声放大器配置//复位值=0x01
#define CC112X_RXMIX                    0x2F31	//RXMIX - RX混频器配置//复位值=0x01

#define CC112X_XOSC5                    0x2F32	//XOSC5-晶体振荡器配置 //复位值=0x0C
#define CC112X_XOSC4                    0x2F33	//XOSC4-晶体振荡器配置//复位值=0xA0
#define CC112X_XOSC3                    0x2F34	//XOSC3-晶体振荡器配置//复位值=0x03
#define CC112X_XOSC2                    0x2F35	//XOSC2-晶体振荡器配置//复位值=00000100
#define CC112X_XOSC1                    0x2F36	//XOSC1-晶体振荡器配置//复位值=0x00
#define CC112X_XOSC0                    0x2F37	//XOSC0-晶体振荡器配置//复位值=0x00

#define CC112X_ANALOG_SPARE             0x2F38	//ANALOG_SPARE- //复位值=0x00
#define CC112X_PA_CFG3                  0x2F39	//PA_CFG3- 功率放大器配置//复位值=0x00
#define CC112X_IRQ0M                    0x2F3F	
#define CC112X_IRQ0F                    0x2F40	

////////////////////////////////////////////////////////////////////////////
//状态寄存器/* Status Registers */
#define CC112X_WOR_TIME1                0x2F64	//WOR_TIME1-eWOR定时器状态（MSB） //复位值=0x00
#define CC112X_WOR_TIME0                0x2F65	//WOR_TIME0-eWOR定时器状态（LSB） //复位值=0x00
#define CC112X_WOR_CAPTURE1             0x2F66	//WOR_CAPTURE1 - eWOR定时器捕捉（MSB）//复位值=0x00
#define CC112X_WOR_CAPTURE0             0x2F67	//WOR_CAPTURE0 - eWOR定时器捕捉（LSB）//复位值=0x00
#define CC112X_BIST                     0x2F68	//BIST - MARC BIST- //复位值=0x00
#define CC112X_DCFILTOFFSET_I1          0x2F69	//DCFILTOFFSET_I1 - 直流滤波器偏移I（MSB）//复位值=0x00
#define CC112X_DCFILTOFFSET_I0          0x2F6A	//DCFILTOFFSET_I0 - 直流滤波器偏移I（LSB）//复位值=0x00
#define CC112X_DCFILTOFFSET_Q1          0x2F6B	//DCFILTOFFSET_Q1 -直流滤波器偏移 Q（MSB）//复位值=0x00
#define CC112X_DCFILTOFFSET_Q0          0x2F6C	//DCFILTOFFSET_Q0 -直流滤波器偏移 Q（LSB）//复位值=0x00
#define CC112X_IQIE_I1                  0x2F6D	//IQIE_I1 - IQ不平衡值I（MSB）//复位值=0x00
#define CC112X_IQIE_I0                  0x2F6E	//IQIE_I0 - IQ不平衡值I（LSB）//复位值=0x00
#define CC112X_IQIE_Q1                  0x2F6F	//IQIE_Q1 - IQ不平衡值Q（MSB）//复位值=0x00
#define CC112X_IQIE_Q0                  0x2F70	//IQIE_Q0 - IQ不平衡值Q（LSB）//复位值=0x00

#define CC112X_RSSI1                    0x2F71	//RSSI1 - 接收信号强度指示器（MSB）(只读) //复位值=0x80
#define CC112X_RSSI0                    0x2F72	//RSSI0 - 接收信号强度指示器（LSB）(只读) //复位值=0x00

#define CC112X_MARCSTATE                0x2F73	//MARCSTATE - MARC 状态(只读) //复位值=01000001       //MARC (Main Radio Control)

#define CC112X_LQI_VAL                  0x2F74	//LQI_VAL - 链路质量指标值(只读) //复位值=0x00

#define CC112X_PQT_SYNC_ERR             0x2F75	//PQT_SYNC_ERROR-前导码和同步字错误(只读) //复位值=0xFF

#define CC112X_DEM_STATUS               0x2F76	//DEM_STATUS - 解调器状态(只读)//复位值=0x00

#define CC112X_FREQOFF_EST1             0x2F77	//FREQOFF_EST1 - 频偏估计（MSB）(只读) //复位值=0x00
#define CC112X_FREQOFF_EST0             0x2F78	//FREQOFF_EST0 - 频偏估计（LSB）(只读) //复位值=0x00
#define CC112X_AGC_GAIN3                0x2F79	//AGC_GAIN3-AGC增益//复位值=0x00   //AGC_FRONT_END_GAIN =00  //AGC前端增益。实际应用的分辨率为1 dB增益
#define CC112X_AGC_GAIN2                0x2F7A	//AGC_GAIN2-AGC增益//复位值=11101001  //AGC_DRIVES_FE_GAIN  覆盖AGC增益控制=1=AGC控制前端增益  //AGC_LNA_CURRENT  //AGC_LNA_R_DEGEN
#define CC112X_AGC_GAIN1                0x2F7B	//AGC_GAIN1-AGC增益//复位值=0x00   //AGC_LNA_R_LOAD //AGC_LNA_R_RATIO
#define CC112X_AGC_GAIN0                0x2F7C	//AGC_GAIN0-AGC增益//复位值=00111111  //AGC_IF_MODE  //AGC_IFAMP_GAIN
#define CC112X_SOFT_RX_DATA_OUT         0x2F7D	//SOFT_RX_DATA_OUT-软件设置RX数据输出//复位值=0x00   //SOFT_RX_DATA
#define CC112X_SOFT_TX_DATA_IN          0x2F7E	//SOFT_TX_DATA_OUT-软件设置TX数据输入//复位值=0x00   //SOFT_TX_DATA
#define CC112X_ASK_SOFT_RX_DATA         0x2F7F	//ASK_SOFT_RX_DATA - AGC ASK 软件配置输出//复位值=0x30  //ASK_SOFT_RX_DATA_RESERVED5_0 
#define CC112X_RNDGEN                   0x2F80	//RNDGEN - 随机数值//复位值=0x7F
#define CC112X_MAGN2                    0x2F81	//MAGN2 - CORDIC后的信号幅度[16](只读)//复位值=0x00  //DEM_MAGN_16=0  //CORDIC后的瞬时信号幅度，17位[16]
#define CC112X_MAGN1                    0x2F82	//MAGN1 - CORDIC后的信号幅度[15:8](只读)//复位值=0x00  //DEM_MAGN_15_8=0  //CORDIC后的瞬时信号幅度[15:8]
#define CC112X_MAGN0                    0x2F83	//MAGN0 - CORDIC后的信号幅度[7:0](只读)//复位值=0x00  //DEM_MAGN_7_0=0  //CORDIC后的瞬时信号幅度[7:0]
#define CC112X_ANG1                     0x2F84	//ANG1 - CORDIC后的信号角度[9:8](只读)//复位值=0x00  //CORDIC后的瞬时信号的角度
#define CC112X_ANG0                     0x2F85	//ANG0 - CORDIC后的信号角度[7:0](只读)//复位值=0x00  //CORDIC后的瞬时信号的角度
#define CC112X_CHFILT_I2                0x2F86	//CHFILT_I2 - 通道筛选数据的实部[18:16](只读)//复位值=0x80  //DEM_CHFILT_STARTUP_VALID=1=信道滤波器数据有效（后置16信道滤波器样品）  //DEM_CHFILT_I_18_16
#define CC112X_CHFILT_I1                0x2F87	//CHFILT_I1 - 通道筛选数据的实部[15:8](只读)//复位值=0x00   //DEM_CHFILT_I_15_8
#define CC112X_CHFILT_I0                0x2F88	//CHFILT_I0 - 通道筛选数据的实部[7:0](只读)//复位值=0x00    //DEM_CHFILT_I_7_0	
#define CC112X_CHFILT_Q2                0x2F89	//CHFILT_Q2 - 通道筛选数据的虚部[18:16](只读)//复位值=0x00  //DEM_CHFILT_Q_18_16
#define CC112X_CHFILT_Q1                0x2F8A	//CHFILT_Q1 - 通道筛选数据的实部[15:8](只读)//复位值=0x00   //DEM_CHFILT_Q_15_8
#define CC112X_CHFILT_Q0                0x2F8B	//CHFILT_Q0 - 通道筛选数据的实部[7:0](只读)//复位值=0x00    //DEM_CHFILT_Q_7_0

#define CC112X_GPIO_STATUS              0x2F8C	//GPIO_STATUS - GPIO 状态//复位值=0x00
#define CC112X_FSCAL_CTRL               0x2F8D	//FSCAL_CTRL-//复位值=0x01
#define CC112X_PHASE_ADJUST             0x2F8E	//PHASE_ADJUST-//复位值=0x00  //仅用于测试目的

#define CC112X_PARTNUMBER               0x2F8F	//PARTNUMBER -芯片型号//复位值=0x00
#define CC112X_PARTVERSION              0x2F90	//PARTVERSION- 部分调整//复位值=0x00

#define CC112X_SERIAL_STATUS            0x2F91	//SERIAL_STATUS - 串行状态//复位值=0x00

#define CC112X_RX_STATUS                0x2F92	//RX_STATUS- RX  状态(只读) //复位值=0x01

#define CC112X_TX_STATUS                0x2F93	//TX_STATUS-TX 状态(只读)//复位值=0x00

#define CC112X_MARC_STATUS1             0x2F94	//MARC_STATUS1 -MARC 状态(只读)//复位值=0x00  //用于记录是什么信号产生MARC_MCU_WAKEUP信号
#define CC112X_MARC_STATUS0             0x2F95	//MARC_STATUS0 -MARC 状态(只读)//复位值=0x00  

#define CC112X_PA_IFAMP_TEST            0x2F96	//PA_IFAMP_TEST-//复位值=0x00 //PA_IFAMP_TEST_RESERVED4_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_FSRF_TEST                0x2F97	//FSRF_TEST-//复位值=0x00     //FSRF_TEST_RESERVED6_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_PRE_TEST                 0x2F98	//PRE_TEST-//复位值=0x00     //PRE_TEST_RESERVED4_0   //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_PRE_OVR                  0x2F99	//PRE_OVR-//复位值=0x00   //PRE_TEST_RESERVED4_0   //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_ADC_TEST                 0x2F9A	//ADC_TEST - ADC Test //复位值=0x00  //ADC_TEST_RESERVED5_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_DVC_TEST                 0x2F9B	//DVC_TEST - DVC Test //复位值=0x0B  //DVC_TEST_RESERVED4_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_ATEST                    0x2F9C	//ATEST-//复位值=0x40    //ATEST_RESERVED6_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_ATEST_LVDS               0x2F9D	//ATEST_LVDS-//复位值=0x00   //ATEST_LVDS_RESERVED3_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_ATEST_MODE               0x2F9E	//ATEST_MODE-//复位值=0x00   //ATEST_MODE_RESERVED7_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_XOSC_TEST1               0x2F9F	//XOSC_TEST1-//复位值=0x3C   //XOSC_TEST1_RESERVED7_0  //仅用于测试目的，使用值来自SmartRF套件
#define CC112X_XOSC_TEST0               0x2FA0	//XOSC_TEST0-//复位值=0x00   //XOSC_TEST0_RESERVED7_0  //仅用于测试目的，使用值来自SmartRF套件
                                        
#define CC112X_RXFIRST                  0x2FD2	//RXFIRST - RX FIFO Pointer (first entry)//复位值=0x00  //指向第一个写进RX FIFO的字节   
#define CC112X_TXFIRST                  0x2FD3	//TXFIRST - TX FIFO Pointer (first entry)//复位值=0x00  //指向第一个写进TX FIFO的字节  
#define CC112X_RXLAST                   0x2FD4	//RXLAST - RX FIFO Pointer (last entry)//复位值=0x00    //指向最后一个写进RX FIFO的字节 
#define CC112X_TXLAST                   0x2FD5	//TXLAST - TX FIFO Pointer (last entry)//复位值=0x00    //指向最后一个写进TX FIFO的字节  
#define CC112X_NUM_TXBYTES              0x2FD6  //NUM_TXBYTES - TX FIFO状态(占用空间)//复位值=0x00  //TXBYTES  TX FIFO中的字节数/* Number of bytes in TXFIFO */ 
#define CC112X_NUM_RXBYTES              0x2FD7  //NUM_RXBYTES - RX FIFO状态(占用空间)//复位值=0x00  //RXBYTES  RX FIFO中的字节数/* Number of bytes in RXFIFO */
#define CC112X_FIFO_NUM_TXBYTES         0x2FD8  //FIFO_NUM_TXBYTES - TX FIFO状态(空白空间)//复位值=0x0F  //FIFO_TXBYTES  TX FIFO中空白空间的字节数。1111：表示还有>=15个字节的空间，可以被写入。
#define CC112X_FIFO_NUM_RXBYTES         0x2FD9  //FIFO_NUM_RXBYTES - RX FIFO状态(可获得的字节数)//复位值=0x00  //FIFO_RXBYTES  RX FIFO中可以被读出的字节数，1111：表示还有>=15个字节，可以读出

////////////////////////////////////////////////////////////////////////////                                                                                                                                             
//数据FIFO访问/* DATA FIFO Access */
#define CC112X_SINGLE_TXFIFO            0x003F      /*  TXFIFO  - Single accecss to Transmit FIFO */
#define CC112X_BURST_TXFIFO             0x007F      /*  TXFIFO  - Burst accecss to Transmit FIFO  */
#define CC112X_SINGLE_RXFIFO            0x00BF      /*  RXFIFO  - Single accecss to Receive FIFO  */
#define CC112X_BURST_RXFIFO             0x00FF      /*  RXFIFO  - Busrrst ccecss to Receive FIFO  */

#define CC112X_LQI_CRC_OK_BM            0x80
#define CC112X_LQI_EST_BM               0x7F


////////////////////////////////////////////////////////////////////////////
//命令选通寄存器/* Command strobe registers */
#define CC112X_SRES                     0x30      /*  SRES    - Reset chip. */
#define CC112X_SFSTXON                  0x31      /*  SFSTXON - Enable and calibrate frequency synthesizer. */
#define CC112X_SXOFF                    0x32      /*  SXOFF   - Turn off crystal oscillator. */
#define CC112X_SCAL                     0x33      /*  SCAL    - Calibrate frequency synthesizer and turn it off. */
#define CC112X_SRX                      0x34      /*  SRX     - Enable RX. Perform calibration if enabled. */
#define CC112X_STX                      0x35      /*  STX     - Enable TX. If in RX state, only enable TX if CCA passes. */
#define CC112X_SIDLE                    0x36      /*  SIDLE   - Exit RX / TX, turn off frequency synthesizer. */
#define CC112X_SWOR                     0x38      /*  SWOR    - Start automatic RX polling sequence (Wake-on-Radio) */
#define CC112X_SPWD                     0x39      /*  SPWD    - Enter power down mode when CSn goes high. */
#define CC112X_SFRX                     0x3A      /*  SFRX    - Flush the RX FIFO buffer. */
#define CC112X_SFTX                     0x3B      /*  SFTX    - Flush the TX FIFO buffer. */
#define CC112X_SWORRST                  0x3C      /*  SWORRST - Reset real time clock. */
#define CC112X_SNOP                     0x3D      /*  SNOP    - No operation. Returns status byte. */
#define CC112X_AFC                      0x37      /*  AFC     - Automatic Frequency Correction */

////////////////////////////////////////////////////////////////////////////
//芯片状态返回状态字节/* Chip states returned in status byte */
#define CC112X_STATE_IDLE               0x00
#define CC112X_STATE_RX                 0x10
#define CC112X_STATE_TX                 0x20
#define CC112X_STATE_FSTXON             0x30
#define CC112X_STATE_CALIBRATE          0x40
#define CC112X_STATE_SETTLING           0x50
#define CC112X_STATE_RXFIFO_ERROR       0x60
#define CC112X_STATE_TXFIFO_ERROR       0x70


/******************************************************************************
 * PROTPTYPES
 */ 

/* basic set of access functions */
rfStatus_t cc112xSpiReadReg(uint16 addr, uint8 *data, uint8 len);
rfStatus_t cc112xGetTxStatus(void);
rfStatus_t cc112xGetRxStatus(void);  
rfStatus_t cc112xSpiWriteReg(uint16 addr, uint8 *data, uint8 len);
rfStatus_t cc112xSpiWriteTxFifo(uint8 *pWriteData, uint8 len);
rfStatus_t cc112xSpiReadRxFifo(uint8 *pReadData, uint8 len);

#ifdef  __cplusplus
}
#endif
/******************************************************************************
  Copyright 2010 Texas Instruments Incorporated. All rights reserved.

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
*******************************************************************************/
#endif// CC112x_SPI_H