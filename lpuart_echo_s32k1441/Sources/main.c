/*
 * Copyright (c) 2015 - 2016 , Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
/* ###################################################################
**     Filename    : main.c
**     Project     : lpuart_echo_s32k144
**     Processor   : S32K144_100
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-08-04, 12:27, # CodeGen: 1
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "pin_mux.h"
#include "dmaController1.h"
#include "clockMan1.h"
#include "lpuart1.h"

volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "shell.h"
#include "shell_if.h"
/* Welcome message displayed at the console */
#define welcomeMsg "This example is an simple echo using LPUART\r\n\
it will send back any character you send to it.\r\n\
The board will greet you if you send 'Hello Board'\r\
\nNow you can begin typing:\r\n"

/* Error message displayed at the console, in case data is received erroneously */
#define errorMsg "An error occurred! The application will stop!\r\n"

/* Timeout in ms for blocking operations */
#define TIMEOUT     100U

/* Receive buffer size */
#define BUFFER_SIZE 256U

/* Buffer used to receive data from the console */
uint8_t buffer;
uint8_t bufferIdx;

/* UART rx callback for continuous reception, byte by byte */

void rxCallback(void *driverState, uart_event_t event, void *userData)
{
	 if(event == UART_EVENT_RX_FULL)
	 {
	  ShellIf_Gets(&buffer,1);
	  LPUART_DRV_SetRxBuffer(INST_LPUART1, &buffer, 1U);
	 }
	 else
	 {
	 }
}

void ShellIf_CMD_Appstate(void *arg)
{
	myprintf("test");
}
/*!
 \brief The main function for the project.
 \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
 */
int main(void)
{
  /* Write your local variable definition here */
//  status_t status;
  /* Declare a buffer used to store the received data */
//uint32_t bytesRemaining;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
#ifdef PEX_RTOS_INIT
  PEX_RTOS_INIT(); /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
#endif
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */

  /* Initialize and configure clocks
   *     -    see clock manager component for details
   */
  CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                 g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

  /* Initialize pins
   *    -    See PinSettings component for more info
   */
  PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);

  /* Initialize LPUART instance */
  LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
  /* Install the callback for rx events */
  LPUART_DRV_InstallRxCallback(INST_LPUART1, rxCallback, NULL);
  LPUART_DRV_ReceiveData(INST_LPUART1,&buffer,1);
  /* Send a welcome message */
  LPUART_DRV_SendDataBlocking(INST_LPUART1, (uint8_t *)welcomeMsg, strlen(welcomeMsg), TIMEOUT);
  ShellIf_Init();
  /* Infinite loop:
   *     - Receive data from user
   *     - Echo the received data back
   */
  while (1)
  {

      ShellIf_Process();
      ShellIf_Process();
      ShellIf_Process();
      ShellIf_Process();
      ShellIf_Process();
      ShellIf_Process();
      ShellIf_Process();
      ShellIf_Process();
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.1 [05.21]
 **     for the Freescale S32K series of microcontrollers.
 **
 ** ###################################################################
 */

