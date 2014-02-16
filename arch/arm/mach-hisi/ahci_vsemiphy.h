#ifndef __HI_ATA_SYS_VSEMI_H
#define __HI_ATA_SYS_VSEMI_H

/* sata使用的系统和IOCONTROL寄存器地址定义 */


#define HI_SATA_PORT_PHYCTL     0x74
#define UINT32 unsigned int
#define INT32 signed int


/* 寄存器 数据结构定义 */
/* serdes2的复位请求寄存器 */
typedef struct tagHI_SC_SERDES_RESET_REQ1_REG_S {
    unsigned int  Sds1SynthtRst:1;            /* Serdes1的 multi hard synth的软复位位 */
    unsigned int  Sds1MltRst:4;               /* Serdes1的multi hard 复位位，每一位代表一个端口*/
    unsigned int  Sds1PipRst:4;               /* Serdes1的PIP line 复位位，每一位代表一个端口 */
    unsigned int  Sds1rst:1;                  /* Serdes1的端口复位位 */
    unsigned int  Sds2SynthtRst:1;            /* Serdes2的 multi hard synth的软复位位 */
    unsigned int  Sds2MltRst:4;               /* Serdes2的multi hard 复位位，每一位代表一个端口*/
    unsigned int  Sds2PipRst:4;               /* Serdes2的PIP line 复位位，每一位代表一个端口 */
    unsigned int  Sds2rst:1;                  /* Serdes2的端口复位位 */
    unsigned int  Reserve:14;                 /* 保留位 */
} HI_SC_SERDES_RESET_REQ1_REG_STRU;

typedef union tagHI_SC_SERDES_RESET_REQ1_REG_U {
    unsigned int uiScSdsRstReq1Reg;
    HI_SC_SERDES_RESET_REQ1_REG_STRU strScSdsRstReq1RegEach;
} HI_SC_SERDES_RESET_REQ1_REG_UNION;

/* SERDES2去复位请求寄存器 */
typedef struct tagHI_SC_SERDES_RESET_DREQ1_REG_S {
    unsigned int  Sds1SynthtRst:1;            /* Serdes1的 multi hard synth的软复位位 */
    unsigned int  Sds1MltRst:4;               /* Serdes1的multi hard 复位位，每一位代表一个端口*/
    unsigned int  Sds1PipRst:4;               /* Serdes1的PIP line 复位位，每一位代表一个端口 */
    unsigned int  Sds1rst:1;                  /* Serdes1的端口复位位 */
    unsigned int  Sds2SynthtRst:1;            /* Serdes2的 multi hard synth的软复位位 */
    unsigned int  Sds2MltRst:4;               /* Serdes2的multi hard 复位位，每一位代表一个端口*/
    unsigned int  Sds2PipRst:4;               /* Serdes2的PIP line 复位位，每一位代表一个端口 */
    unsigned int  Sds2rst:1;                  /* Serdes2的端口复位位 */
    unsigned int  Reserve:14;                 /* 保留位 */
} HI_SC_SERDES_RESET_DREQ1_REG_STRU;

typedef union tagHI_SC_SERDES_RESET_DREQ1_REG_U {
    unsigned int uiScSdsRstDreq1Reg;
    HI_SC_SERDES_RESET_DREQ1_REG_STRU strScSdsRstDreq1RegEach;
} HI_SC_SERDES_RESET_DREQ1_REG_UNION;

/* IOCTRL1_0寄存器定义，配置SERDES工作模式 */
typedef struct tagHI_IOCTRL1_0_REG_S {
    unsigned int  L0Sel:4;                  /* Serdes2的Lane0的功能选择:0x0表示Gmac3;0x1表示SATA；0x2表示sRIO */
    unsigned int  L1Sel:4;                  /* Serdes2的Lane1的功能选择:0x0表示Gmac3;0x1表示SATA；0x2表示sRIO*/
    unsigned int  L2Sel:4;                  /* Serdes2的Lane2的功能选择:0x0表示Gmac3;0x1表示SATA；0x2表示sRIO */
    unsigned int  L3Sel:4;                  /* Serdes2的Lane3的功能选择:0x0表示Gmac3;0x1表示SATA；0x2表示sRIO */
    unsigned int  Reserve:16;               /* 保留位 */
} HI_IOCTRL1_0_REG_STRU;

typedef union tagHI_IOCTRL1_0_REG_U {
    unsigned int uiIoctrl10Reg;
    HI_IOCTRL1_0_REG_STRU struiIoctrl10RegEach;
} HI_IOCTRL1_0_REG_UNION;


/* 寄存器 数据结构定义 */
typedef struct tagHI_IOCTRL1_2_REG_S {
    unsigned int  ClkEn:6;                  /* SATA接收/发送时钟使能:0表示不使能，1表示使能 */
    unsigned int  Reserve:26;               /* 保留位 */
} HI_IOCTRL1_2_REG_STRU;

typedef union tagHI_IOCTRL1_2_REG_U {
    unsigned int uiIoctrl12Reg;
    HI_IOCTRL1_2_REG_STRU struiIoctrl12RegEach;
} HI_IOCTRL1_2_REG_UNION;


/* 寄存器 数据结构定义 */
typedef struct tagHI_IOCTRL0_4_REG_S {
    unsigned int  RxTxChPowerCtl:8;  /* 通道发送/接收disable/power-down控制 */
    unsigned int  RxDatawidthl0:3;   /* Ch0 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
    unsigned int  RxDatawidthl1:3;   /* Ch1 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
    unsigned int  RxDatawidthl2:3;   /* Ch2 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
    unsigned int  RxDatawidthl3:3;   /* Ch3 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
    unsigned int  TxDatawidthl0:3;   /* Ch0 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
    unsigned int  TxDatawidthl1:3;   /* Ch1 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
    unsigned int  TxDatawidthl2:3;   /* Ch2 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
    unsigned int  TxDatawidthl3:3;   /* Ch3 发送数据位宽选择 000:8bit;001:10bit;010:16bit;011:20bit;100:32bit;101:40bit */
} HI_IOCTRL0_4_REG_STRU;

typedef union tagHI_IOCTRL0_4_REG_U {
    unsigned int uiIoctrl04Reg;
    HI_IOCTRL0_4_REG_STRU struiIoctrl04RegEach;
} HI_IOCTRL0_4_REG_UNION;


typedef struct tagHI_IOCTRL0_5_REG_S {
    unsigned int  RxRatel0:3;    /* ch0 PMA接收速率选择 */
    unsigned int  RxRatel1:3;    /* ch1 PMA接收速率选择 */
    unsigned int  RxRatel2:3;    /* ch2 PMA接收速率选择 */
    unsigned int  RxRatel3:3;    /* ch3 PMA接收速率选择 */
    unsigned int  TxRatel0:3;    /* ch0 PMA发送速率选择*/
    unsigned int  TxRatel1:3;    /* ch1 PMA发送速率选择 */
    unsigned int  TxRatel2:3;    /* ch2 PMA发送速率选择 */
    unsigned int  TxRatel3:3;    /* ch3 PMA发送速率选择 */
    unsigned int  Pstatel0:2;    /* ch0 power down状态控制 11: Coma Power State;10:Slumber;01:Doze;00:Wake*/
    unsigned int  Pstatel1:2;    /* ch1 power down状态控制 11: Coma Power State;10:Slumber;01:Doze;00:Wake*/
    unsigned int  Pstatel2:2;    /* ch2 power down状态控制 11: Coma Power State;10:Slumber;01:Doze;00:Wake*/
    unsigned int  Pstatel3:2;    /* ch3 power down状态控制 11: Coma Power State;10:Slumber;01:Doze;00:Wake*/
} HI_IOCTRL0_5_REG_STRU;

typedef union tagHI_IOCTRL0_5_REG_U {
    unsigned int uiIoctrl05Reg;
    HI_IOCTRL0_5_REG_STRU struiIoctrl05RegEach;
} HI_IOCTRL0_5_REG_UNION;

typedef struct tagHI_IOCTRL1_12_REG_S {
    unsigned int  RxLockEn:4;              /* PCS CDR锁定控制使能 */
    unsigned int  RxLock:4;                /* PCS CDR锁定控制   */
    unsigned int  TxampEn:4;               /* 发送摆幅配置使能，每一位代表一个ch*/
    unsigned int  Txampl0:3;               /* CH0发送幅度控制 */
    unsigned int  Txampl1:3;               /* CH1发送幅度控制 */
    unsigned int  Txampl2:3;               /* CH2发送幅度控制 */
    unsigned int  Txampl3:3;               /* CH3发送幅度控制 */
    unsigned int  TxBeacon:4;              /* CH3 Beacon发送使能，每一位代表一个ch；0:禁止;1:使能 */
    unsigned int  TxDtctRxReq:4;           /* 发送接收器检测请求，每一位代表一个通道 */
} HI_IOCTRL1_12_REG_STRU;

typedef union tagHI_IOCTRL1_12_REG_U {
    unsigned int uiIoctrl112Reg;
    HI_IOCTRL1_12_REG_STRU struiIoctrl112RegEach;
} HI_IOCTRL1_12_REG_UNION;

/* 定义 IOCTRL1_18寄存器 */
typedef struct tagHI_IOCTRL1_18_REG_S {
    unsigned int  IctlMemAddr:12;            /* memory地址 */
    unsigned int  IdatPma:1;                 /* memory的PMA */
    unsigned int  IdatPage:3;                /* memory写地址 */
    unsigned int  IdatMemWdat:8;             /* memory写数据 */
    unsigned int  IctlMemWr:1;               /* memory读写标志 */
    unsigned int  IctlMemReq:1;              /* memory请求 */
    unsigned int  ClrOctlMemAck:1;           /* 清除octl_mem_ack响应 */
    unsigned int  Reserve:5;                 /* 保留位 */
} HI_IOCTRL1_18_REG_STRU;

typedef union tagHI_IOCTRL1_18_REG_U {
    unsigned int uiIoctrl118Reg;
    HI_IOCTRL1_18_REG_STRU struiIoctrl118RegEach;
} HI_IOCTRL1_18_REG_UNION;


typedef struct tagHI_SC_USB_RESET_REQ_REG_S {
    unsigned int  Reserve1:16;               /* 保留位 */
    unsigned int  SataSftRst:2;             /* SATA软复位:0x3表示软复位；0x0表示没有软复位 */
    unsigned int  Reserve0:14;               /* 保留位 */
} HI_SC_USB_RESET_REQ_STRU;

typedef union tagHI_SC_USB_RESET_REQ_U {
    unsigned int uiScUsbRstReqReg;
    HI_SC_USB_RESET_REQ_STRU strScUsbRstReqEach;
} HI_SC_USB_RESET_REQ_UNION;

typedef struct tagHI_SC_SATA_CLK_EN_REG_S {
    unsigned int  SataClkEn:2;             /* SATA时钟复位使能位 */
    unsigned int  Reserve0:30;               /* 保留位 */
} HI_SC_SATA_CLK_EN_STRU;

typedef union tagHI_SC_SATA_CLK_EN_U {
    unsigned int uiScSataClkEnReg;
    HI_SC_SATA_CLK_EN_STRU strScSataClkEnRegEach;
} HI_SC_SATA_CLK_EN_UNION;

typedef struct tagHI_SC_SATA_CLK_DIS_REG_S {
    unsigned int  SataClkDis:2;             /* SATA时钟禁止位 */
    unsigned int  Reserve0:30;               /* 保留位 */
} HI_SC_SATA_CLK_DIS_STRU;

typedef union tagHI_SC_SATA_CLK_DIS_U {
    unsigned int uiScSataClkDisReg;
    HI_SC_SATA_CLK_DIS_STRU strScSataClkDisRegEach;
} HI_SC_SATA_CLK_DIS_UNION;

typedef struct tagHI_IOCTRL1_IOSTAT1_0_S {
    unsigned int  ChRxMultiStatus:4;          /* CH 接收通道状态跳转请求响应指示  */
    unsigned int  ChRxMultiRdy:4;             /* CH 接收通道Ready指示   */
    unsigned int  ChTxMultiRdy:4;             /* CH 发送通道Ready指示  */
    unsigned int  ChTxPmaStatus:4;            /* CH 发送通道状态跳转请求响应指示  */
    unsigned int  ChRxSgnlDetc:4;             /* CH 接收数据检测状态指示 */
    unsigned int  ChRxBistDone:4;             /* CH RX BIST完成指示，高有效*/
    unsigned int  ChRxBistLocked:4;           /* CH RX BIST 已锁定指示  */
    unsigned int  ChRxBistOverFlow:4;         /* CH RX BIST error count 上溢指示,每一位代表一个通道 */
} HI_IOCTRL1_IOSTAT1_0_STRU;

typedef union tagHI_IOCTRL1_IOSTAT1_0_U {
    unsigned int uiIoctrl1Iostat10Reg;
    HI_IOCTRL1_IOSTAT1_0_STRU strIoctrl1Iostat10RegEach;
} HI_IOCTRL1_IOSTAT1_0_UNION;


typedef struct tagHI_SC_USB_RESET_DREQ_REG_S {
    unsigned int  Reserve1:16;               /* 保留位 */
    unsigned int  SataDereset:2;             /* SATA 去软复位 */
    unsigned int  Reserve0:14;               /* 保留位 */
} HI_SC_USB_RESET_DREQ_STRU;

typedef union tagHI_SC_USB_RESET_DREQ_U {
    unsigned int uiScUsbRstDreqReg;
    HI_SC_USB_RESET_DREQ_STRU strScUsbRstDreqRegEach;
} HI_SC_USB_RESET_DREQ_UNION;

#endif
