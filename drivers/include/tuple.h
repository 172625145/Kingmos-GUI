/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef		__TUPLE_H
#define		__TUPLE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#pragma pack(1)

// Tuple Codes

#define TPL_TYPE_NULL         0x00
#define TPL_TYPE_DEVICE       0x01
//02h ~ 05h: reservd to future
//#define TPL_TYPE_LONGLINK_CB  0x02        // CardBus
//#define TPL_TYPE_CONFIG_CB    0x04        // CardBus
//#define TPL_TYPE_CFTABLE_ENTRY_CB 0x05    // CardBus
#define TPL_TYPE_LONGLINK_MFC 0x06        // Multiple Function
//#define TPL_TYPE_LONG_LINK_MFC 0x06       // Multiple Function
#define TPL_TYPE_CHECKSUM     0x10
#define TPL_TYPE_LONGLINK_A   0x11
#define TPL_TYPE_LONGLINK_C   0x12
#define TPL_TYPE_LINKTARGET   0x13
#define TPL_TYPE_NO_LINK      0x14
#define TPL_TYPE_VERS_1       0x15
#define TPL_TYPE_ALTSTR       0x16
#define TPL_TYPE_DEVICE_A     0x17
#define TPL_TYPE_JEDEC_C      0x18
#define TPL_TYPE_JEDEC_A      0x19
#define TPL_TYPE_CONFIG       0x1A
#define TPL_TYPE_CFTABLE_ENTRY 0x1B
#define TPL_TYPE_DEVICE_OC    0x1C
#define TPL_TYPE_DEVICE_OA    0x1D
#define TPL_TYPE_DEVICEGEO    0x1E
#define TPL_TYPE_DEVICEGEO_A  0x1F
#define TPL_TYPE_MANFID       0x20
#define TPL_TYPE_FUNCID       0x21
#define TPL_TYPE_FUNCE        0x22
#define TPL_TYPE_SWIL		  0x23	//PC卡软体交错因子
#define TPL_TYPE_VERS_2       0x40
#define TPL_TYPE_FORMAT       0x41
#define TPL_TYPE_GEOMETRY     0x42
#define TPL_TYPE_BYTEORDER    0x43
#define TPL_TYPE_DATE         0x44
#define TPL_TYPE_BATTERY      0x45
#define TPL_TYPE_ORG          0x46
#define TPL_TYPE_FORMAT_A	  0x47	//属性记忆体的资料记录格式
//80h ~ 8fh: 供厂商使用的特定栏位组
#define	TPL_TYPE_SPCL		  0x90	//DOS档案资讯
#define TPL_TYPE_END          0xFF


// 设备类型定义 Device Type Code Definitions
#define DTYPE_NULL     0x00
#define DTYPE_ROM      0x01
#define DTYPE_OTPROM   0x02
#define DTYPE_EPROM    0x03
#define DTYPE_EEPROM   0x04
#define DTYPE_FLASH    0x05
#define DTYPE_SRAM     0x06
#define DTYPE_DRAM     0x07
#define DTYPE_FUNCSPEC 0x0D
#define DTYPE_EXTEND   0x0E


// Device Size Byte Definitions
#define DSIZE_CODE  0x07
#define DSIZE_UNITS 0xF1

// Device Size Code Definitions
#define SCODE_512  0x00
#define SCODE_2K   0x01
#define SCODE_8K   0x02
#define SCODE_32K  0x03
#define SCODE_128K 0x04
#define SCODE_512K 0x05
#define SCODE_2M   0x06

// Device Information String Terminator Definition
#define TPL_L1V_END 0xFF	//level 1 version end

//
// CISTPL_FUNCID function codes
//
#define PCCARD_TYPE_VENDOR_SPECIFIC 0
#define PCCARD_TYPE_MEMORY        1
#define PCCARD_TYPE_SERIAL        2
#define PCCARD_TYPE_PARALLEL      3
#define PCCARD_TYPE_FIXED_DISK    4
#define PCCARD_TYPE_VIDEO         5
#define PCCARD_TYPE_NETWORK       6
#define PCCARD_TYPE_AIMS          7
#define PCCARD_TYPE_UNKNOWN       0xff

//*-----------------------------------------------------------------------------------------
//
//Below define is for configuration table 
//
// Configuration Table Index Byte Definitions

#define TPLCFT_INDEXNUM		0x3F			//Configuration index
#define TPLCFT_DEFAULT		0x40			//Default bit
#define TPLCFT_INTERFACE	0x80			//Inferface

// Configuration Table Entry Interface Description Field Definitions

#define TPLCFT_IFTYPE       0x0F			//interface type
#define TPLCFT_BVDSACTIVE   0x10			//电池电压侦测BVDs启动位元
#define TPLCFT_WPACTIVE     0x20			//写入保护启动位元
#define TPLCFT_RDYBSYACTIVE 0x40			//ready/busy 启动位元
#define TPLCFT_MWAITREQD    0x80			//记忆周期等待讯号需求 
											//1: 存取记忆装置时需要WAIT信号的支持

//
//Define for Interface type
//
#define	INTF_TYPE_MEMORY	0x00			//memory type
#define INTF_TYPE_MEMORY_IO	0x01			//memory or I/O type
//0x2 ~ 0x3: reserved
//0x4 ~ 0x7: custom type
//0x8 ~ 0xf: reseved						

// Configuration Table Entry Feature Selection Byte Definitions

#define TPLCFT_POWER    0x03				//Power feature
											//0: Use default set
											//1: only Vcc
											//2: Vcc and Vpp(Vpp1 = Vpp2)
											//3: Vcc , Vpp1 and Vpp2
#define TPLCFT_TIMING   0x04				//Timing feature
#define TPLCFT_IOSPACE  0x08				//I/O	space
#define TPLCFT_IRQ      0x10				//IRQ
#define TPLCFT_MEMSPACE 0x60				//Memory sqace
#define TPLCFT_MISC     0x80				//杂项

// Configuration Table Entry Power Desc Parameter Selection Byte Definitions

#define TPLCFT_PD_NOMV    0x01				//额定操作电压
#define TPLCFT_PD_MINV    0x02				//最低操作电压
#define TPLCFT_PD_MAXV    0x04				//最高操作电压
#define TPLCFT_PD_STATICI 0x08				//稳态电流值
#define TPLCFT_PD_AVGI    0x10				//最大平均电流值per 1Sec
#define TPLCFT_PD_PEAKI   0x20				//最大平均电流值per 10Sec
#define TPLCFT_PD_PDWNI   0x40				//开机电流供应值

// Configuration Table Entry Power Description Structure Parameter Definitions

//#define TPLCFT_PD_EXP   0x07
//#define TPLCFT_PD_MANT  0x71
#define TPLCFT_PD_INDEX	  0x07				//电源参数指数部分
#define TPLCFT_PD_MANT	  0x78				//电源参数小数部分
#define TPLCFT_PD_EXT   0x80				//Power parameter extend bit

#define TPLCFT_PD_EXTEN 0x7F				//power parameter exist bit

// Configuration Table Entry Power Description Voltage Index Definitions

#define PDINDEX_10UV    0x00
#define PDINDEX_100UV   0x01
#define PDINDEX_1MV     0x02
#define PDINDEX_10MV    0x03
#define PDINDEX_100MV   0x04
#define PDINDEX_1V      0x05
#define PDINDEX_10V     0x06
#define PDINDEX_100V    0x07

// Configuration Table Entry Power Description Current Exponent Definitions

#define PDINDEX_100NA   0x00
#define PDINDEX_1UA     0x01
#define PDINDEX_10UA    0x02
#define PDINDEX_100UA   0x03
#define PDINDEX_1MA     0x04
#define PDINDEX_10MA    0x05
#define PDINDEX_100MA   0x06
#define PDINDEX_1A      0x07

// Configuration Table Entry Power Description Mantissa Definitions

#define PDMANT_1_0    0x00
#define PDMANT_1_2    0x01
#define PDMANT_1_3    0x02
#define PDMANT_1_5    0x03
#define PDMANT_2_0    0x04
#define PDMANT_2_5    0x05
#define PDMANT_3_0    0x06
#define PDMANT_3_5    0x07
#define PDMANT_4_0    0x08
#define PDMANT_4_5    0x09
#define PDMANT_5_0    0x0A
#define PDMANT_5_5    0x0B
#define PDMANT_6_0    0x0C
#define PDMANT_7_0    0x0D
#define PDMANT_8_0    0x0E
#define PDMANT_9_0    0x0F


// Configuration Table Entry I/O Space Address Parameter Definitions
#define TPLCFT_IO_ADDRLINES 0x1F		//0:  卡片的选择与定址由系统负责
										//1~ 26: 由卡片解码
										//27~31: Reservd
#define TPLCFT_IO_BUS8      0x20		//D0 ~ D7
#define TPLCFT_IO_BUS16     0x40		//D0 ~ D15
#define TPLCFT_IO_RANGE     0x80		//1: I/O位址范围
										//0: Any I/O 位址

// Configuration Table Entry I/O Space Range Descriptor Definitions
#define TPLCFT_IO_NUMRANGES  0x0F		//Number of the I/O range
#define TPLCFT_IO_ADDRSIZE   0x30		//Size of I/O range 
#define TPLCFT_IO_LENGTHSIZE 0xC0		//length of I/O range

// Configuration Table Entry IRQ Interrupt Request Descriptor Definitions
#define TPLCFT_IR_IRQN  0x0F
#define TPLCFT_IR_NMI   0x01
#define TPLCFT_IR_LOCK  0x02
#define TPLCFT_IR_BERR  0x04
#define TPLCFT_IR_VEND  0x08			
#define TPLCFT_IR_MASK  0x10			//Mask of IRQ tuple byte number
#define TPLCFT_IR_LEVEL 0x20			//level of IRQ
#define TPLCFT_IR_PULSE 0x40			//edge  of IRQ
#define TPLCFT_IR_SHARE 0x80			//share the IRQ

//----------------------------------------------------------------------------------------*/



//
//  If the POWER_DESCR structure is changed, then PARSED_CFTABLE must be realigned
//
// @struct POWER_DESCR | Structure used for the power description fields
// VccDescr, Vpp1Descr and Vpp2Descr in a PARSED_CFTABLE structure.
//
// Note:
//  1) The voltages are in 0.1 volt units,
//
typedef struct _POWER_DESCR {
    UINT16   ValidMask;      // @field Describes which of the following fields are valid
                            // (bit 0 set implies NominalV is valid,
                            // bit 1 set implies MinV is valid)
                            // etc
    UINT16   NominalV;       // @field Nominal operating supply voltage
    UINT16   MinV;           // @field Minimum operating supply voltage
    UINT16   MaxV;           // @field Maximum operating supply voltage
    UINT16   StaticI;        // @field Continuous supply current required
    UINT16   AvgI;           // @field Max current required averaged over 1 second
    UINT16   PeakI;          // @field Max current required averaged over 10 milliseconds
    UINT16   PowerDownI;     // @field Power-down supply current required
}_PACKED_  POWER_DESCR, * PPOWER_DESCR;

//
// POWER_DESCR.ValidMask
// 
#define PWR_DESCR_NOMINALV  0x01
#define PWR_DESCR_MINV      0x02
#define PWR_DESCR_MAXV      0x04
#define PWR_DESCR_STATICI   0x08
#define PWR_DESCR_AVGI      0x10
#define PWR_DESCR_PEAKI     0x20
#define PWR_DESCR_POWERDWNI 0x40

#define PWR_AVAIL_NOMINALV  0x0100
#define PWR_AVAIL_MINV      0x0200
#define PWR_AVAIL_MAXV      0x0400

#define MAX_IO_RANGES   4   // limit this to 4, it can be as high as 16

//
// @struct PARSED_CONFIG | Structure returned by CardGetParsedTuple for a parsed
//                         CISTPL_CONFIG tuple
//
// Note:
//  1) only one byte of the register presence mask is used,
//  2) custom interface subtuples are not parsed
//
typedef struct _PARSED_CONFIG {
    UINT32  ConfigBase;         // @field Base address of the card configuration registers.
    UINT8   RegMask;            // @field Config register presence mask.
    UINT8   LastConfigIndex;    // @field Index of last entry in the card configuration table.
}_PACKED_  PARSED_CONFIG, * PPARSED_CONFIG;

#define MAX_IO_RANGES   4   // limit this to 4, it can be as high as 16
//
// @struct PARSED_CFTABLE | Structure used by CardGetParsedTuple for parsed
// CISTPL_CFTABLE_ENTRY tuples.
//
typedef struct _PARSED_CFTABLE {
    POWER_DESCR VccDescr;   // @field Power description of Vcc
    POWER_DESCR Vpp1Descr;  // @field Power description of Vpp1
    POWER_DESCR Vpp2Descr;  // @field Power description of Vpp2

    UINT32  IOLength[MAX_IO_RANGES];    // @field IOLength[i] is the number of bytes in 
    UINT32  IOBase[MAX_IO_RANGES];      // @field the I/O address range starting at IOBase[i]
                                        // There are NumIOEntries in each array.
    UINT8   NumIOEntries;   // @field Number of I/O address range entries in the two arrays:
                            // IOLength[] and IOBase[]

    UINT8   ConfigIndex;    // @field Value to be written to the configuration option
                            // register to enable this CFTABLE_ENTRY.
    UINT8   ContainsDefaults;   // @field True if this entry contains defaults to be used by subsequent entries.

    UINT8   IFacePresent;   // @field True if the following 5 fields are valid:
    UINT8   IFaceType;      // @field 0 = memory, 1 = I/O, 2,3,8-15 = rsvd, 4-7 = custom
    UINT8   BVDActive;      // @field True if BVD1 and BVD2 are active in the Pin Replacement Register (PRR)
    UINT8   WPActive;       // @field True if the card write protect is active in the PRR
    UINT8   ReadyActive;    // @field True if READY is active in the PRR
    UINT8   WaitRequired;   // @field True if the WAIT signal is required for memory cycles

    UINT8   IOAccess;       // @field 1 = 8 bit only, 2 = 8 bit access to 16 bit registers is not supported,
                            // 3 = 8 bit access to 16 bit registers is supported

    UINT8   NumIOAddrLines; // @field 0 = socket controller provides address decode
                            // 1-26 = number of address lines decoded
    UINT8   reserved;		// NOTE: fill to size of PARSED_CFTABLE is muiltipe of UINT16;
}_PACKED_ PARSED_CFTABLE, * PPARSED_CFTABLE;

#pragma pack()

#ifdef __cplusplus
}
#endif  // __cplusplus


#endif	//__TUPLE_H
