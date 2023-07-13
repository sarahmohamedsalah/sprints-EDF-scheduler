/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpc21xx.h"
#include "semphr.h"
#include "event_groups.h"
/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
/*****************************************Defintions used in the main **************************/

#define NULL_PTR 	(void *) 0
#define LOGIC_HIGH			1
#define LOGIC_LOW			0
/************** Periodicity of the Tasks************************/


#define UART_TASK_PERIOD				20
#define LOAD1_TASK_PERIOD				10
#define LOAD2_TASK_PERIOD				100

/********************************************************************
**********************Task Handlers***********************************
************************************************************************/
TaskHandle_t Uart_ReceiverHandler = NULL;

TaskHandle_t Load1Handler = NULL;
TaskHandle_t Load2Handler = NULL;

/******************************Queue Handler **************************/
QueueHandle_t xMessageBuffer = NULL;

void Uart_Receiver( void * pvParameters)
{
	char* Received_message = NULL_PTR;
	TickType_t xLastWakeTime;
	 xLastWakeTime = xTaskGetTickCount();

	vTaskSetApplicationTaskTag(NULL,(void *) 5); /*giving Tag to the task to use Trace Hooks */

	for(;;)
	{
		if( xQueueReceive( xMessageBuffer, &( Received_message ), ( TickType_t ) 0 ) == pdPASS )
		{ 
			vSerialPutString((const signed char *)Received_message,20); /* receiving the message to the queue */
		}
		vTaskDelayUntil(&xLastWakeTime,UART_TASK_PERIOD);
	}
}






void Load_1_Simulation( void * pvParameters )
{
	int i;
   TickType_t xLastWakeTime;
	 xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL,(void *) 6);
    for( ;; )
    {
        /* Task code goes here. */
			
			for (i = 0; i < 37313; i++)
			{
				/* for loop to make the excutions time 5ms*/
			}
			
			vTaskDelayUntil(&xLastWakeTime,LOAD1_TASK_PERIOD);
    } 


}


void Load_2_Simulation( void * pvParameters )
{
	int i;
   TickType_t xLastWakeTime;
	 xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL,(void *) 7);
    for( ;; )
    {
        /* Task code goes here. */
			
			
			for ( i = 0; i < 89552; i++)
			{
				/* for loop to make the excutions time 12ms*/
			}
			
				
			
			
			vTaskDelayUntil(&xLastWakeTime,LOAD2_TASK_PERIOD);
    } 


}
void vApplicationTickHook(void)
{
	GPIO_write(PORT_1, PIN0, PIN_IS_HIGH);
	GPIO_write(PORT_1, PIN0, PIN_IS_LOW);
}


void vApplicationIdleHook(void)
{
	vTaskSetApplicationTaskTag(NULL,(void *) 8); /*giving Tag to the task to use Trace Hooks */
}


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/


/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
    /* Create Tasks here */
xTaskPeriodicCreate(
                    Uart_Receiver,       /* Function that implements the task. */
                    "Load_1_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Uart_ReceiverHandler, /* Used to pass out the created task's handle. */
										UART_TASK_PERIOD);     
	
	 xTaskPeriodicCreate(
                    Load_1_Simulation,       /* Function that implements the task. */
                    "Load_1_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Load1Handler, /* Used to pass out the created task's handle. */
										LOAD1_TASK_PERIOD);     /*Used t0 pass the period of the task*/
										
		xTaskPeriodicCreate(
                    Load_2_Simulation,       /* Function that implements the task. */
                    "Load_2_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1 ,/* Priority at which the task is created. */
                    &Load2Handler, /* Used to pass out the created task's handle. */
										LOAD2_TASK_PERIOD); /*Used t0 pass the period of the task*/
			
/*Creating the Queue */						
		xMessageBuffer = xQueueCreate( 3, sizeof( unsigned char[15] ) );

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


