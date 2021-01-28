/****************************************************************************
* @file
* @author		yangmaolin
* @date			2020.02.29
* @version		0.1
* @brief		
* @copyright	
****************************************************************************/

/****************************************************************************
* Version & History
* 
* 0.1 
* First edition(development).
****************************************************************************/

/**********************************INCLUDE**********************************/
#include "shell.h"
#include "shell_if.h"
#include "getopt.h"
//#include "msg_mgmt.h"
//#include "data_mgmt.h"
//#include "can_mgmt.h"
//#include "system_S32K142.h"
//#include "timer_mgmt.h"
#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP)
//#include "net_mgmt.h"
#include "Cpu.h"
//#include <framework/pkg/Inc/printf.h>
#endif
/***************************************************************************/
#if (DEBUG_ENABLE == TRUE)
/****************************Macrow Definitions*****************************/
#define CMD_BUFFER_LEN							(128U)
#define PUTS_BUFFER_LEN							(8)
#define PARSE_BUFFER_LEN						(100LU)

#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP)
#define WELCOM_STRING							"Application Shell System Start...\r\nVersion:" __DATE__ "-" __TIME__
#else
#define WELCOM_STRING							"Bootloader Shell System Start...\r\nVersion:" __DATE__ "-" __TIME__
#endif
/***************************************************************************/
//#define FALSE 0
//#define TRUE 1
/*****************************Type Definitions******************************/
/***************************************************************************/

BOOL is_frame_cplt;
/****************************Function Declarations**************************/
static void ShellIf_Puts(const char * data , uint16_t len);
static void ShellIf_CMD_ReadMemory(void *args);
static void ShellIf_CMD_GetInfo(void *arg);
static void ShellIf_CMD_GetNVMData(void *arg);
#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP)
static void ShellIf_CMD_NMControl(void *arg);
#endif
//static void ShellIf_PutsBufferSend(void);
static void ShellIf_CMD_DebugDisable(void *arg);
static void ShellIf_CMD_RecvCANmsg(void *arg);
static void ShellIf_CMD_Reset(void *arg);
static void ShellIf_CMD_CANDis(void *arg);
static void ShellIf_CMD_PowerSave(void *arg);
void ShellIf_CMD_Appstate(void *arg);

/***************************************************************************/


/****************************Variable Definitions***************************/
//static CANSendItem_S item = {.id = 0x01,};
static uint8_t can_rx_buffer[CMD_BUFFER_LEN] = {0};
static volatile uint32_t can_rx_buffer_idx;
//static volatile BOOL is_frame_cplt;
static struct shell_input can_shell;
static uint32_t puts_buffer_idx;
//static int parse_buffer[PARSE_BUFFER_LEN];
static BOOL is_debug_enable;
/***************************************************************************/

/*****************************Function Definitions**************************/
BOOL ShellIf_Init(void)
{
	puts_buffer_idx = 0;
	can_rx_buffer_idx = 0;
	//is_frame_cplt = FALSE;
	is_debug_enable = TRUE;
	
	#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP)
	shell_init("APP >", ShellIf_Puts);
	#else
	shell_init("BOOT >", ShellIf_Puts);
	#endif
	shell_input_init(&can_shell, ShellIf_Puts);
	shell_start(&can_shell, WELCOM_STRING);
	
	shell_register_command("memread", ShellIf_CMD_ReadMemory);
	shell_register_command("getinfo", ShellIf_CMD_GetInfo);
	shell_register_command("nvm", ShellIf_CMD_GetNVMData);
	shell_register_command("debugdisable", ShellIf_CMD_DebugDisable);
	shell_register_command("rcan", ShellIf_CMD_RecvCANmsg);
	//shell_register_command("reset", ShellIf_CMD_Reset);
//	shell_register_command("candis", ShellIf_CMD_CANDis);
//	shell_register_command("powersave", ShellIf_CMD_PowerSave);
	shell_register_command("app_state", ShellIf_CMD_Appstate);
	#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP)
	//shell_register_command("nm_control", ShellIf_CMD_NMControl);
	#endif
	return TRUE;
}

void ShellIf_Process(void)
{
//	ShellIf_PutsBufferSend();
	if(is_frame_cplt == TRUE)
	{
		shell_input(&can_shell, (char *)can_rx_buffer, can_rx_buffer_idx);
		can_rx_buffer_idx = 0;
		is_frame_cplt = FALSE;
	}
}

//static void ShellIf_PutsBufferSend(void)
//{
//	if(puts_buffer_idx != 0)
//	{
//		item.len = puts_buffer_idx;
//		puts_buffer_idx = 0;
//		if(is_debug_enable == FALSE) return;
//		CANMgmt_MessageSend(&item);
//	}
//}

static void ShellIf_Puts(const char * data , uint16_t len)
{
	//uint32_t	bytesRemaining;
	//LPUART_DRV_SendData(INST_LPUART1,(uint8_t *) data,len);
	LPUART_DRV_SendDataBlocking(INST_LPUART1,(uint8_t *) data,len,20);
	//while(LPUART_DRV_GetTransmitStatus(INST_LPUART1, &bytesRemaining) != STATUS_SUCCESS);
}

void ShellIf_Gets(uint8_t *data, uint32_t len)
{
	for(uint32_t i = 0; i < len; i++)
	{
		can_rx_buffer[can_rx_buffer_idx++] = data[i];
	}
	is_frame_cplt = TRUE;
}

static void ShellIf_CMD_ReadMemory(void *arg)
{
	int argv[2];
	int argc = cmdline_param((char*)arg, argv, 2);
	
	if(argc == 2)
	{
		uint32_t addr = *(uint32_t *)&argv[0];
		uint32_t len = (uint32_t)argv[1];
		if(len > 512)
		{
			printk("\r\nThe length to read must less than 512 bytes.");
		}
		else
		{
			printk("Memory Read - Address: %08X, Length: %d\n", addr, len);
			for(uint32_t i = 0; i < len; i++)
			{
				printk("%02X ", ((uint8_t *)addr)[i]);
				if((i != 0) && (i % 0x10 == 0))
				{
					printk("\r\n");
				}
			}
		}
	}
	else
	{
		printk("\r\nArguments error, it must like:memread 0x00001000 4.\r\n");
	}
}

static void ShellIf_CMD_DebugDisable(void *arg)
{
	is_debug_enable = FALSE;
}

static void ShellIf_CMD_GetInfo(void *arg)
{
//	#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP)
//	printk("This is Application, Compile time:%s %s\n", __DATE__, __TIME__);
//	#else
//	prink("This is Bootloader, Compile time:%s %s\n", __DATE__, __TIME__);
//	#endif
//
//	#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_BOOT)
//	printk("Voltage = %.3f\n", (double)VolMgmt_VoltageGet());
//	#else
//	printk("Voltage = %.3f\n", (ServiceIf_BatteryVoltageGet() / 1000.0f));
//	#endif
	
	//CommonTimer_GetUsedInfo(&common_timer);

//	MsgMgmt_InfomationGet();
}

static void ShellIf_CMD_GetNVMData(void *arg)
{
//	int opt;
//	int argc;
//	char **argv = (char **)parse_buffer;
//
//	argc = cmdline_strtok((char*)arg, argv, PARSE_BUFFER_LEN);
//	if(argc >= 2)
//	{
//		optind = 1;
//		opt = getopt(argc, argv, "r::c:s:");
//		if(opt != -1)
//		{
//			switch(opt)
//			{
//				case 'r':
//					if(optarg == NULL)
//					{
//						DataMgmt_NVMDataGetAll();
//					}
//					else
//					{
//						int idx;
//						if(sscanf(optarg, "%d", &idx) == 1)
//						{
//							DataMgmt_NVMDataGetIdx((uint8_t)idx);
//						}
//						else
//						{
//							printk("-r Argments error.\r\n");
//						}
//					}
//					break;
//				case 'c':
//					if(optarg != NULL)
//					{
//						int arg_read;
//						if(sscanf(optarg, "%d", &arg_read) == 1)
//						{
//							DataMgmt_NVMClr(arg_read);
//							break;
//						}
//					}
//					printk("-c Argments error.\r\n");
//					break;
//				case 's':
//					if(optarg != NULL)
//					{
//						uint8_t data_read[100];
//						int idx;
//						if(sscanf(optarg, "%d", &idx) == 1)
//						{
//							uint32_t cnt = 0;
//							int scanf_tmp;
//							for(uint32_t i = 2; i < argc; i++)
//							{
//								if(sscanf(argv[i], "%d", &scanf_tmp) == 1)
//								{
//									cnt++;
//									data_read[i] = (uint8_t)scanf_tmp;
//								}
//								else
//								{
//									break;
//								}
//							}
//							if(cnt != 0)
//							{
//								DataMgmt_NVMSet((uint32_t)idx, &data_read[2], cnt);
//								return;
//							}
//						}
//					}
//					printk("-c Argments error.\r\n");
//					break;
//				default:
//					printk("CMD Argments error.\r\n");
//					break;
//			}
//		}
//		else
//		{
//			printk("CMD Argments error.\r\n");
//		}
//	}
}

static void ShellIf_CMD_RecvCANmsg(void *arg)
{
//	CANSendItem_S item;
//
//	int argc = cmdline_param((char *)arg, parse_buffer, PARSE_BUFFER_LEN);
//
//	if((argc >= 1) && (argc <= 9))
//	{
//		item.id = (uint16_t)parse_buffer[0];
//		argc--;
//		item.len = argc;
//		for(uint32_t i = 0; i < argc; i++)
//		{
//			item.data[i] = parse_buffer[i + 1];
//		}
//		CANMgmt_Recv_Callback(item.id, item.data, item.len);
//	}
//	else
//	{
//		printk("CMD Argments error.\r\n");
//	}
}

#if (CONFIG_RUN_TIME_ENVIROMENT == DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP)
static void ShellIf_CMD_NMControl(void *arg)
{
//	char *op[2];
//
//	int argc = cmdline_strtok(arg, op, 2);
//
//	if(argc == 2)
//	{
//		if(strcmp(op[1], "?") == 0)
//		{
//			printk("Args\r\n\trelease,\r\n\trequest\r\n\trxenable\r\n\trxdisable\r\n\ttxenable\r\n\ttxdisable\r\n\trepeatrequest\r\n\tcvbget:\tget the current control vector bit.\r\n\tnmpdustruct");
//		}
//		else if(strcmp(op[1], "release") == 0)
//		{
//			NetMgmt_NetReleased(NM_REQ_TYPE_APP);
//		}
//		else if(strcmp(op[1], "request") == 0)
//		{
//			NetMgmt_NetRequest(NM_REQ_TYPE_APP);
//		}
//		else if(strcmp(op[1], "rxenable") == 0)
//		{
//			NetMgmt_NMMsgEnableRx(TRUE);
//		}
//		else if(strcmp(op[1], "rxdisable") == 0)
//		{
//			NetMgmt_NMMsgEnableRx(FALSE);
//		}
//		else if(strcmp(op[1], "txenable") == 0)
//		{
//			NetMgmt_NMMsgEnableTx(TRUE);
//		}
//		else if(strcmp(op[1], "txdisable") == 0)
//		{
//			NetMgmt_NMMsgEnableTx(FALSE);
//		}
//		else if(strcmp(op[1], "repeatrequest") == 0)
//		{
//			NetMgmt_RepeatMsgRequst();
//		}
//		else if(strcmp(op[1], "cvbget") == 0)
//		{
//			printk("Control Vector Bit: %02X\n", NetMgmt_ControlVectorBitGet());
//		}
//		else if(strcmp(op[1], "nmpdustruct") == 0)
//		{
//			printk("\tecu_addr\r\n\tcontrol_vector_bit\r\n\twakeup_reason\r\n\tcurr_state\r\n\tstay_awake_reason\r\n");
//		}
//		else
//		{
//			printk("Wrong options.");
//		}
//	}
	printk("Wrong options.");
}
#endif

static void ShellIf_CMD_Reset(void *arg)
{
	#if (CONFIG_WDOG_ENBALE == TRUE)
	while(TRUE);
	#else
	SystemSoftwareReset();
	#endif
	/*Wait for watchdog timer timeout and reset.*/
}

static void ShellIf_CMD_CANDis(void *arg)
{
	//CANMgmt_Disable();
	printk("CAN disabled.\r\n");
}

static void ShellIf_CMD_PowerSave(void *arg)
{
	
}


/***************************************************************************/


























#endif
