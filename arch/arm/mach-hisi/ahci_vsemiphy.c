#include "ahci_vsemiphy.h"
#include <linux/delay.h>

#define SERDES_DEBUG_OPEN	(0)
#define gulDebugOpen		(0)

static void hi_sata_init_ok(void)
{
	HI_IOCTRL1_IOSTAT1_0_UNION tmp_val;
	printk(" ******sata init start test 000********* ");

	do {
		tmp_val.uiIoctrl1Iostat10Reg = readl(HI_IOCTRL1_IOSTAT1_0_ADDR);
	}while((tmp_val.strIoctrl1Iostat10RegEach.ChTxMultiRdy != 0xf)
		||(tmp_val.strIoctrl1Iostat10RegEach.ChRxMultiRdy != 0xf)
		||(tmp_val.strIoctrl1Iostat10RegEach.ChRxMultiStatus != 0xf)
		||(tmp_val.strIoctrl1Iostat10RegEach.ChTxPmaStatus != 0xf));
	printk(" ******sata init start test 111********* ");
}

static void osSerdesWait(UINT32 ulLoop)
{
	while (ulLoop--)
		;
}

static void SERDES_REG_WRITE (UINT32 pRegBase, UINT32 ulRegIndex, UINT32  ulValue)
{
#if SERDES_DEBUG_OPEN
	/*printk( "addr:0x%8x,ulValue:0x%8x\n", pRegBase, ulValue );*/
	msleep(10);

#endif
	writel(ulValue, pRegBase + ulRegIndex);
}

#define SERDES_REG_READ(addr, index, data)	((data) = *(volatile unsigned int *)(addr+index))

static void osSerdes2Write(UINT32 ulPage, UINT32 ulType, UINT32 ulOffset, UINT32 ulValue)
{
	/*1、:write addr[ioctrl1_base_addr+0x48],data[bit31-27=all 0,*/
	/*bit26=0,bit25=1,bit24=1,bit23-16=x4,bit15-13=x1,bit12=x2,bit11-0=x3]*/
	UINT32 ulTemp = 0;
	UINT32 ulRdata = 0;
	INT32 ulLoop = 0x100000;
	UINT32 ulStepChoose = 0;
	ulTemp = ( ( ulTemp | 0x3000000 ) | ( ulValue << 16 ) | \
	       ( ulOffset & 0xfff ) | ( ulType << 12 ) | ( ulPage << 13 ) );
	SERDES_REG_WRITE( HI_IOCONTROL1_REG_BASE_ADDR + 0x48, 0, ulTemp );

	/*2:write addr[ioctrl1_base_addr+0x48],*/
	/*data[bit31-27=all 0,bit26=0,bit25=0,bit24=1,*/
	/*bit23-16=x4,bit15-13=x1,bit12=x2,bit11-0=x3]*/
	SERDES_REG_WRITE( HI_IOCONTROL1_REG_BASE_ADDR + 0x48, 0, ulTemp & 0xFDFFFFFF );

	/*第3步：read addr[ioctrl1_base_addr+0x428], 回读的数据为rdata*/
	SERDES_REG_READ( HI_IOCONTROL1_REG_BASE_ADDR + 0x428, 0 ,ulRdata);
#if SERDES_DEBUG_OPEN
	/*printk( "check HI_IOCONTROL1_REG_BASE_ADDR + 0x428 == 0x100\n" );*/
	msleep(20);
#else
	/*第4步：如果第3步中rdata[bit8==0],回到第3步；如果1ms内，*/
	/*rdata[bit8==1],走到第5步；如果超过1ms后，直接走到第6步*/
	while ( --ulLoop )
	{
		if ( 0x100 == ( ulRdata & 0x100 ) )
		{
			ulStepChoose = 5;
			break;
		}
		else
		{
			SERDES_REG_READ( HI_IOCONTROL1_REG_BASE_ADDR + 0x428, 0 ,ulRdata );
		}
	}
#endif
	if(gulDebugOpen)
	{

		if ( 5 == ulStepChoose )
		{
			printk( "serdes2 read ok page=%x sel=%x offset=%x rdata=%x", ulPage, ulType, ulOffset, ulRdata );
		}
	}
	if ( ulLoop <= 0 )
	{
		printk( "serdes2 read outoftime page=%x sel=%x offset=%x value=%x", ulPage, ulType, ulOffset, ulValue );
	}
	/*第7步：write addr[ioctrl1_base_addr+0x48],*/
	/*data[bit31-27=all 0,bit26=1,bit25=0,bit24=1,bit23-16=x4,bit15-13=x1,*/
	/*bit12=x2,bit11-0=x3]*/
	ulTemp = 0;
	ulTemp = ( ulTemp | 0x5000000 ) | ( ulPage << 13 ) | ( ulType << 12 ) | ( ulOffset & 0xfff ) | (ulValue << 16);
	SERDES_REG_WRITE( HI_IOCONTROL1_REG_BASE_ADDR + 0x48, 0, ulTemp );

	osSerdesWait(1000);
}

static void osSerdesBitWrite( UINT32 ulAddr, UINT32 ulOrMask, UINT32 ulAndMask )
{
	UINT32 ulValue = 0;
	SERDES_REG_READ(ulAddr, 0, ulValue);
	ulValue = ( ulValue | ulOrMask ) & ulAndMask;
	SERDES_REG_WRITE(ulAddr, 0, ulValue);
}

static void hi_sata_init_config_all(void)
{
	printk("hi_sata_init_config_all begin\n");

	/* 复位SATA */
	writel(0x30000,HI_SYS_CTL_REG_BASE_ADDR+0x600);
	msleep(30);
	printk("\r\n sata begin reset");
	/*关闭SATA的tx/rx时钟 */
	writel(0x0,HI_IOCONTROL1_REG_BASE_ADDR+0x8);
	msleep(30);

	/* 关闭SATA的AHB/AXI时钟 */
	writel(0x0,HI_SYS_CTL_REG_BASE_ADDR+0x310);
	writel(0x3,HI_SYS_CTL_REG_BASE_ADDR+0x314);
	msleep(30);
	printk("\r\n sata begin close AHB/AXI");

	/* serdes2  initialization */
	//1.    POR
	osSerdesBitWrite(HI_SYS_CTL_REG_BASE_ADDR+0x0588,0x80000,0xffffffff);
	//1(1) set multi-mode:
	osSerdesBitWrite(HI_IOCONTROL1_REG_BASE_ADDR+0xc,0x1400,0xFFFFF7FF);
	//1(2) set pcie-mode:
	//2.    Release POR
	osSerdesBitWrite(HI_SYS_CTL_REG_BASE_ADDR+0x058c,0x80000,0xffffffff);

	//4.	Hard Reset
	osSerdes2Write(0x0,0x0,0x2,0x2);
	osSerdes2Write(0x1,0x0,0x2,0x2);
	osSerdes2Write(0x2,0x0,0x2,0x2);
	osSerdes2Write(0x3,0x0,0x2,0x2);
	osSerdes2Write(0x4,0x0,0x2,0x1);

	// 5.	Load Different Data Rate Settings

	osSerdes2Write (0,0,101,201);
	osSerdes2Write (0,0,102,201);
	osSerdes2Write (0,0,103,7);
	osSerdes2Write (0,0,104,7);
	osSerdes2Write (0,0,105,24);
	osSerdes2Write (0,0,106,24);
	osSerdes2Write (0,0,107,1);
	osSerdes2Write (0,0,108,1);
	osSerdes2Write (0,0,109,34);
	osSerdes2Write (0,0,110,5);
	osSerdes2Write (1,0,101,201);
	osSerdes2Write (1,0,102,201);
	osSerdes2Write (1,0,103,7);
	osSerdes2Write (1,0,104,7);
	osSerdes2Write (1,0,105,24);
	osSerdes2Write (1,0,106,24);
	osSerdes2Write (1,0,107,1);
	osSerdes2Write (1,0,108,1);
	osSerdes2Write (1,0,109,34);
	osSerdes2Write (1,0,110,5);
	osSerdes2Write (2,0,101,201);
	osSerdes2Write (2,0,102,201);
	osSerdes2Write (2,0,103,7);
	osSerdes2Write (2,0,104,7);
	osSerdes2Write (2,0,105,24);
	osSerdes2Write (2,0,106,24);
	osSerdes2Write (2,0,107,1);
	osSerdes2Write (2,0,108,1);
	osSerdes2Write (2,0,109,34);
	osSerdes2Write (2,0,110,5);
	osSerdes2Write (3,0,101,201);
	osSerdes2Write (3,0,102,201);
	osSerdes2Write (3,0,103,7);
	osSerdes2Write (3,0,104,7);
	osSerdes2Write (3,0,105,24);
	osSerdes2Write (3,0,106,24);
	osSerdes2Write (3,0,107,1);
	osSerdes2Write (3,0,108,1);
	osSerdes2Write (3,0,109,34);
	osSerdes2Write (3,0,110,5);
	osSerdes2Write (4,0,101,170);
	osSerdes2Write (4,0,102,0);
	osSerdes2Write (4,0,103,69);
	osSerdes2Write (4,0,104,201);
	osSerdes2Write (4,0,105,201);
	osSerdes2Write (4,0,106,7);
	osSerdes2Write (4,0,107,7);
	osSerdes2Write (4,0,108,24);
	osSerdes2Write (4,0,109,24);
	osSerdes2Write (4,0,110,5);
	osSerdes2Write (4,0,111,5);
	osSerdes2Write (4,0,112,16);
	osSerdes2Write (4,0,113,0);
	osSerdes2Write (4,0,114,16);
	osSerdes2Write (4,0,115,0);
	osSerdes2Write (4,0,116,255);
	osSerdes2Write (4,0,117,207);
	osSerdes2Write (4,0,118,247);
	osSerdes2Write (4,0,119,225);
	osSerdes2Write (4,0,120,245);
	osSerdes2Write (4,0,121,253);
	osSerdes2Write (4,0,122,253);
	osSerdes2Write (4,0,123,255);
	osSerdes2Write (4,0,124,255);
	osSerdes2Write (4,0,125,255);
	osSerdes2Write (4,0,126,255);
	osSerdes2Write (4,0,127,227);
	osSerdes2Write (4,0,128,231);
	osSerdes2Write (4,0,129,219);
	osSerdes2Write (4,0,130,245);
	osSerdes2Write (4,0,131,253);
	osSerdes2Write (4,0,132,253);
	osSerdes2Write (4,0,133,245);
	osSerdes2Write (4,0,134,245);
	osSerdes2Write (4,0,135,255);
	osSerdes2Write (4,0,136,255);
	osSerdes2Write (4,0,137,227);
	osSerdes2Write (4,0,138,231);
	osSerdes2Write (4,0,139,219);
	osSerdes2Write (4,0,140,245);
	osSerdes2Write (4,0,141,253);
	osSerdes2Write (4,0,142,253);
	osSerdes2Write (4,0,143,245);
	osSerdes2Write (4,0,144,245);
	osSerdes2Write (4,0,145,255);
	osSerdes2Write (4,0,146,255);
	osSerdes2Write (4,0,147,255);
	osSerdes2Write (4,0,148,245);
	osSerdes2Write (4,0,149,63);
	osSerdes2Write (4,0,150,0);
	osSerdes2Write (4,0,151,50);
	osSerdes2Write (4,0,152,0);
	osSerdes2Write (4,0,153,2);
	osSerdes2Write (4,0,154,1);
	osSerdes2Write (4,0,155,5);
	osSerdes2Write (4,0,156,5);
	osSerdes2Write (4,0,157,4);
	osSerdes2Write (4,0,158,0);
	osSerdes2Write (4,0,159,0);
	osSerdes2Write (4,0,160,8);
	osSerdes2Write (4,0,161,4);
	osSerdes2Write (4,0,162,0);
	osSerdes2Write (4,0,163,0);
	osSerdes2Write (4,0,164,4);
	osSerdes2Write (0,0,7,0);
	osSerdes2Write (1,0,7,0);
	osSerdes2Write (2,0,7,0);
	osSerdes2Write (3,0,7,0);
	osSerdes2Write (4,0,13,16);
	osSerdes2Write (4,0,48,0);
	osSerdes2Write (4,0,49,0);
	osSerdes2Write (4,0,54,0);
	osSerdes2Write (4,0,55,176);
	osSerdes2Write (4,0,93,2);
	osSerdes2Write (4,0,165,2);
	osSerdes2Write (0,0,41,6);
	osSerdes2Write (1,0,41,6);
	osSerdes2Write (2,0,41,6);
	osSerdes2Write (3,0,41,6);
	osSerdes2Write (4,0,354,3);
	osSerdes2Write (4,0,355,58);
	osSerdes2Write (4,0,356,9);
	osSerdes2Write (4,0,357,3);
	osSerdes2Write (4,0,358,62);
	osSerdes2Write (4,0,359,12);
	osSerdes2Write (0,0,701,0);
	osSerdes2Write (1,0,701,0);
	osSerdes2Write (2,0,701,0);
	osSerdes2Write (3,0,701,0);
	/* add something */

	writel (0x3099032,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x5099032,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x30E9033,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x50E9033,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x31D9034,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x51D9034,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x3219035,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x5219035,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x3089036,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x5089036,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x30B9037,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x50B9037,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x35A9038,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x55A9038,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x3649039,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x5649039,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x301903A,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x501903A,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x3A3902E,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x5A3902E,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x3539030,HI_IOCONTROL1_REG_BASE_ADDR+0x48);
	writel (0x5539030,HI_IOCONTROL1_REG_BASE_ADDR+0x48);

	// 6.   Overwrite 由0x4c改成0xdc
	osSerdes2Write (0x0,0x0,0x55,0xdc);
	osSerdes2Write (0x0,0x0,0x56,0xe6);
	osSerdes2Write (0x0,0x0,0x57,0x1f);
	osSerdes2Write (0x0,0x0,0xe6,0xfe);

	osSerdes2Write (0x1,0x0,0x55,0xdc);
	osSerdes2Write (0x1,0x0,0x56,0xe6);
	osSerdes2Write (0x1,0x0,0x57,0x1f);
	osSerdes2Write (0x1,0x0,0xe6,0xfe);

	osSerdes2Write (0x2,0x0,0x55,0xdc);
	osSerdes2Write (0x2,0x0,0x56,0xe6);
	osSerdes2Write (0x2,0x0,0x57,0x1f);
	osSerdes2Write (0x2,0x0,0xe6,0xfe);

	osSerdes2Write (0x3,0x0,0x55,0xdc);
	osSerdes2Write (0x3,0x0,0x56,0xe6);
	osSerdes2Write (0x3,0x0,0x57,0x1f);
	osSerdes2Write (0x3,0x0,0xe6,0xfe);

	osSerdes2Write (0x4,0x0,0x5f,0xc3);

	// 7.   Set to IDDQ state

	osSerdes2Write (0x0,0x0,0x3,0x1);
	osSerdes2Write (0x0,0x0,0x4,0x1);
	osSerdes2Write (0x1,0x0,0x3,0x1);
	osSerdes2Write (0x1,0x0,0x4,0x1);
	osSerdes2Write (0x2,0x0,0x3,0x1);
	osSerdes2Write (0x2,0x0,0x4,0x1);
	osSerdes2Write (0x3,0x0,0x3,0x1);
	osSerdes2Write (0x3,0x0,0x4,0x1);
	osSerdes2Write (0x4,0x0,0x3,0x1);

	// 8.   Release Hard Reset

	osSerdes2Write (0x0,0x0,0x2,0x3);
	osSerdes2Write (0x1,0x0,0x2,0x3);
	osSerdes2Write (0x2,0x0,0x2,0x3);
	osSerdes2Write (0x3,0x0,0x2,0x3);
	osSerdes2Write (0x4,0x0,0x2,0x3);

	// 9(1). PRBS pattern

	osSerdes2Write (0x4,0x0,0x50,0x1);

	// 10.	RXEQ setting

	osSerdes2Write (0x0,0x0,0x18,0x0);
	osSerdes2Write (0x0,0x0,0x19,0x0);
	osSerdes2Write (0x0,0x0,0x1a,0x8);
	osSerdes2Write (0x0,0x0,0x1b,0x78);
	osSerdes2Write (0x0,0x0,0x1c,0x0);

	osSerdes2Write (0x1,0x0,0x18,0x0);
	osSerdes2Write (0x1,0x0,0x19,0x0);
	osSerdes2Write (0x1,0x0,0x1a,0x8);
	osSerdes2Write (0x1,0x0,0x1b,0x78);
	osSerdes2Write (0x1,0x0,0x1c,0x0);

	osSerdes2Write (0x2,0x0,0x18,0x0);
	osSerdes2Write (0x2,0x0,0x19,0x0);
	osSerdes2Write (0x2,0x0,0x1a,0x8);
	osSerdes2Write (0x2,0x0,0x1b,0x78);
	osSerdes2Write (0x2,0x0,0x1c,0x0);

	osSerdes2Write (0x3,0x0,0x18,0x0);
	osSerdes2Write (0x3,0x0,0x19,0x0);
	osSerdes2Write (0x3,0x0,0x1a,0x8);
	osSerdes2Write (0x3,0x0,0x1b,0x78);
	osSerdes2Write (0x3,0x0,0x1c,0x0);

	osSerdes2Write (0x4,0x0,0x53,0x2);

	// 11.	TXIODRIVER setting

	osSerdes2Write (0x0,0x0,0x15,0xdf);
	osSerdes2Write (0x0,0x0,0x16,0x0);
	osSerdes2Write (0x0,0x0,0x17,0x0);
	osSerdes2Write (0x1,0x0,0x15,0xdf);
	osSerdes2Write (0x1,0x0,0x16,0x0);
	osSerdes2Write (0x1,0x0,0x17,0x0);
	osSerdes2Write (0x2,0x0,0x15,0xdf);
	osSerdes2Write (0x2,0x0,0x16,0x0);
	osSerdes2Write (0x2,0x0,0x17,0x0);
	osSerdes2Write (0x3,0x0,0x15,0xdf);
	osSerdes2Write (0x3,0x0,0x16,0x0);
	osSerdes2Write (0x3,0x0,0x17,0x0);

	/*由于3G问题注释*/
	#if 0
	// 12.	Change Data Width to 10 bits

	osSerdes2Write (0x0,0x0,0x5,0x11);
	osSerdes2Write (0x1,0x0,0x5,0x11);
	osSerdes2Write (0x2,0x0,0x5,0x11);
	osSerdes2Write (0x3,0x0,0x5,0x11);
	// 13.	Change DIVRATE
	osSerdes2Write (0x0,0x0,0x6,0x11);
	osSerdes2Write (0x1,0x0,0x6,0x11);
	osSerdes2Write (0x2,0x0,0x6,0x11);
	osSerdes2Write (0x3,0x0,0x6,0x11);
	#endif

	// 15.	Enable PCS TX
	osSerdes2Write (0x4,0x0,0x8,0x54);


	// 16.	Overwrite Enable for Lock2Ref
	osSerdes2Write (0x0,0x0,0x27,0x2);
	osSerdes2Write (0x1,0x0,0x27,0x2);
	osSerdes2Write (0x2,0x0,0x27,0x2);
	osSerdes2Write (0x3,0x0,0x27,0x2);

	// 17.	Power Up to P0

	osSerdes2Write (0x0,0x0,0x3,0x10);
	osSerdes2Write (0x0,0x0,0x4,0x10);

	osSerdes2Write (0x1,0x0,0x3,0x10);
	osSerdes2Write (0x1,0x0,0x4,0x10);

	osSerdes2Write (0x2,0x0,0x3,0x10);
	osSerdes2Write (0x2,0x0,0x4,0x10);

	osSerdes2Write (0x3,0x0,0x3,0x10);
	osSerdes2Write (0x3,0x0,0x4,0x10);

	osSerdes2Write (0x4,0x0,0x3,0x10);
}

static void hi_vsemi_init(void __iomem *mmio)
{
	hi_sata_init_config_all();

	msleep(20);
	hi_sata_init_ok();

	writel(0x1,HI_IOCONTROL1_REG_BASE_ADDR+8);
	writel(0x30000,HI_SYS_CTL_REG_BASE_ADDR+0x604);

	writel(0x3,HI_SYS_CTL_REG_BASE_ADDR+0x310);
	writel(0x0,HI_SYS_CTL_REG_BASE_ADDR+0x314);

	writel(0x1111,HI_IOCONTROL1_REG_BASE_ADDR+0);

#if 0
	int i, n_ports = 4;
	unsigned int tmp = 0xa400000;

	printk("----------------------------------------\n");
	for (i = 0; i < n_ports; i++)
		writel(tmp, (mmio + 0x100 + i*0x80 + HI_SATA_PORT_PHYCTL));
#endif
}
