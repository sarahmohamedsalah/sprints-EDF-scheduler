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
#include "lpc21xx.h"
#include "semphr.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

#define BUTTON_1_MONITOR_TASK_DELAY 50
#define BUTTON_2_MONITOR_TASK_DELAY 50
#define PERIODIC_TASK_DELAY         100
#define UART_RECEIVER_TASK_DELAY    20
#define LOAD_1_SIMULATION_DELAY     10
#define LOAD_2_SIMULATION_DELAY     100
#define TICKS_TO_WAIT               10
#define USR_STRING_LEN              15 
#define LOAD_1                      (37313)
#define LOAD_2                      (89552)

#define QUEUE_SIZE                  10
#define QUEUE_MEMBER_SIZE           15
#define APP_STATS_BUFFER_SIZE				240


TaskHandle_t Button_1_Monitor_TASK_TaskHandler  = NULL;
TaskHandle_t Button_2_Monitor_TASK_TaskHandler  = NULL;
TaskHandle_t Uart_Receiver_TaskHandler          = NULL;
TaskHandle_t Periodic_Transmitter_TaskHandler   = NULL;
TaskHandle_t Load_1_Simulation_TaskHandler      = NULL;
TaskHandle_t Load_2_Simulation_TaskHandler      = NULL;

/* Queue handler */
QueueHandle_t xQueue = NULL;

/* Button 1 states */
pinState_t g_Button_1_previousState = PIN_IS_HIGH;
pinState_t g_Button_1_currentState  = PIN_IS_HIGH;

pinState_t g_Button_2_previousState = PIN_IS_HIGH;
pinState_t g_Button_2_currentState  = PIN_IS_HIGH;

const uint8_t Btn_1_Msg_1[ USR_STRING_LEN ] = "PB 1 Pressed.\n";
const uint8_t Btn_1_Msg_2[ USR_STRING_LEN ] = "PB 1 Release.\n";
const uint8_t Btn_2_Msg_1[ USR_STRING_LEN ] = "PB 2 Pressed.\n";
const uint8_t Btn_2_Msg_2[ USR_STRING_LEN ] = "PB 2 Release.\n";
const uint8_t Msg_3[ USR_STRING_LEN ] = "Periodic str.\n";


unsigned int g_u32_button_1_in_time = 0, 							g_u32_button_1_out_time = 0,							g_u32_button_1_total_time;
unsigned int g_u32_button_2_in_time = 0, 							g_u32_button_2_out_time = 0,							g_u32_button_2_total_time;
unsigned int g_u32_load_1_in_time = 0, 								g_u32_load_1_out_time = 0,								g_u32_load_1_total_time;
unsigned int g_u32_load_2_in_time = 0, 								g_u32_load_2_out_time = 0,								g_u32_load_2_total_time;
unsigned int g_u32_periodic_transmitter_in_time = 0, 	g_u32_periodic_transmitter_out_time = 0,	g_u32_periodic_transmitter_total_time;
unsigned int g_u32_uart_receiver_in_time = 0, 				g_u32_uart_receiver_out_time = 0, 				g_u32_uart_receiver_total_time;
unsigned int g_u32_system_time;
unsigned int g_u32_cpu_load;

unsigned char g_arr_u8_runtime_stats_buff [APP_STATS_BUFFER_SIZE];
/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* ---------------------------- Task implementation starts here. ---------------------------- */

void Button_1_Monitor_Task (void *pvParameters) {
  TickType_t currentTick = 0;
  currentTick = xTaskGetTickCount();
  
  vTaskSetApplicationTaskTag(NULL, (void *) PIN2);
  
  for ( ;; ) 
	{
    g_Button_1_currentState = GPIO_read(PORT_0, PIN0);
    
		/* If push button is pressed, take the semaphore if avialable. */
    if (g_Button_1_previousState == PIN_IS_LOW && g_Button_1_currentState == PIN_IS_HIGH) 
		{
      g_Button_1_previousState = PIN_IS_HIGH;
      /* Send the notificaton message to the queue. */
      xQueueSend(xQueue, (void *)&Btn_1_Msg_1, ( TickType_t ) TICKS_TO_WAIT);
    } 
		else if (g_Button_1_previousState == PIN_IS_HIGH && g_Button_1_currentState == PIN_IS_LOW) 
		{
      g_Button_1_previousState = PIN_IS_LOW;
      /* Send the notification message to the queue. */
      xQueueSend(xQueue, (void *)&Btn_1_Msg_2, ( TickType_t ) TICKS_TO_WAIT);
    
    } 
		else 
		{
      /* Do nothing. */
    }
    vTaskDelayUntil(&currentTick, BUTTON_1_MONITOR_TASK_DELAY);
		GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
  }
}

void Button_2_Monitor_Task (void *pvParameters) {
  TickType_t currentTick = 0;
  currentTick = xTaskGetTickCount();
  
  vTaskSetApplicationTaskTag(NULL, (void *) PIN3);
  
  for ( ;; ) 
	{
    g_Button_2_currentState = GPIO_read(PORT_0, PIN1);
    
    /* If push button is pressed, take the semaphore if avialable. */
    if (g_Button_2_previousState == PIN_IS_LOW && g_Button_2_currentState == PIN_IS_HIGH)
		{
      g_Button_2_previousState = PIN_IS_HIGH;
      /* Send the notificaton message to the queue. */
      xQueueSend(xQueue, (void *)&Btn_2_Msg_1, ( TickType_t ) TICKS_TO_WAIT);
    
    } 
		else if (g_Button_2_previousState == PIN_IS_HIGH && g_Button_2_currentState == PIN_IS_LOW) 
		{
      g_Button_2_previousState = PIN_IS_LOW;
      /* Send the notificaton message to the queue. */
      xQueueSend(xQueue, (void *)&Btn_2_Msg_2, ( TickType_t ) TICKS_TO_WAIT);
    
    } 
		else 
		{    
      /* Do nothing. */
    }
    vTaskDelayUntil(&currentTick, BUTTON_2_MONITOR_TASK_DELAY);
		GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
  }
}
  
void Periodic_Transmitter_Task (void *pvParameters) {
  TickType_t currentTick = 0;
  currentTick = xTaskGetTickCount();
  
  vTaskSetApplicationTaskTag(NULL, (void *) PIN5);
  
  for ( ;; ) 
	{
    xQueueSend(xQueue, (void *)&Msg_3, ( TickType_t ) TICKS_TO_WAIT);
    vTaskDelayUntil(&currentTick, PERIODIC_TASK_DELAY);
		GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
  }
} 

void Uart_Receiver_Task (void *pvParameters) {
  TickType_t currentTick = 0;
  uint8_t xRxedString[ USR_STRING_LEN ];
  currentTick = xTaskGetTickCount();
  
  vTaskSetApplicationTaskTag(NULL, (void *) PIN6);
  
  for ( ;; ) 
	{
    //if( xQueue != NULL ) 
		//{
    //  if( xQueueReceive( xQueue, &( xRxedString ), ( TickType_t ) TICKS_TO_WAIT ) == pdPASS ) {
    //    /* xRxedStructure now contains a copy of xMessage. */
    //    
    //    /* Write on the terminal */
    //    while ( vSerialPutString( (const signed char *) xRxedString, USR_STRING_LEN) == pdFALSE );
    //  }
    //}
		
		vTaskGetRunTimeStats(g_arr_u8_runtime_stats_buff);

    xSerialPutChar('\n');
    
		vSerialPutString(g_arr_u8_runtime_stats_buff,APP_STATS_BUFFER_SIZE);
    //xSerialPutChar('n');

    vTaskDelayUntil(&currentTick, UART_RECEIVER_TASK_DELAY);
		
		GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
  }
}

void Load_1_Simulation( void *pvParameters ) {
  int count;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL, (void *) PIN7);
  
  for( ;; ) {	
    for ( count = 0; count < LOAD_1; count++ ) {
      /* for loop to make the excutions time 5ms*/
    }
    vTaskDelayUntil(&xLastWakeTime, LOAD_1_SIMULATION_DELAY);
		GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
  } 
}

void Load_2_Simulation( void * pvParameters ) {
  int count;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL,(void *) PIN8);
    for( ;; ) {
      for ( count = 0; count < LOAD_2; count++) {
        /* for loop to make the excutions time 12ms*/
      }
      vTaskDelayUntil(&xLastWakeTime, LOAD_2_SIMULATION_DELAY);
			GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
    } 
}

/* ---------------------------- Task implementation ends here. ---------------------------- */

void vApplicationTickHook(void) {
  GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);
  GPIO_write(PORT_0, PIN4, PIN_IS_LOW);
}

void vApplicationIdleHook()
{
	static char tagInit = 0;
	if( tagInit == 0 )
	{
		GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
		vTaskSetApplicationTaskTag(NULL, (void *) PIN9);
		tagInit = 1;
	}
}


/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
  
  /* Initialize UART */
  xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);
  
  /* Create a queue capable of containing 10 15 bits values. */
  xQueue = xQueueCreate( QUEUE_SIZE, QUEUE_MEMBER_SIZE * sizeof( uint8_t ) );
	
   /* ------------------------------------------ Create Tasks here ------------------------------------------*/
  
  /* Create Task 1 (Button 1 Monitor) */
//  xTaskPeriodicCreate(
//  Button_1_Monitor_Task,
//	"Button 1",
//	configMINIMAL_STACK_SIZE,
//	NULL,
//	1,
//	&Button_1_Monitor_TASK_TaskHandler,
//	BUTTON_1_MONITOR_TASK_DELAY);
//  
//  /* Create Task 2 (Button 2 Monitor) */
//  xTaskPeriodicCreate(
//	Button_2_Monitor_Task,
//	"Button 2",
//	configMINIMAL_STACK_SIZE,
//	NULL,
//	1,
//	&Button_2_Monitor_TASK_TaskHandler,
//	BUTTON_2_MONITOR_TASK_DELAY);
//
//  /* Create the periodic task */
//  xTaskPeriodicCreate(
//	Periodic_Transmitter_Task,
//	"Periodic",
//	configMINIMAL_STACK_SIZE,
//	NULL,
//	1,
//	&Periodic_Transmitter_TaskHandler,
//	PERIODIC_TASK_DELAY);
//
//  /* Create the consumer task (UART recieve) */
//  xTaskPeriodicCreate(
//	Uart_Receiver_Task,
//	"Uart",
//	configMINIMAL_STACK_SIZE,
//	NULL,
//	1,
//	&Uart_Receiver_TaskHandler, 
//	UART_RECEIVER_TASK_DELAY);
//  
//  /* Create the load simulation task (Load 1 simulation) */
//  xTaskPeriodicCreate(
//	Load_1_Simulation,
//	"Load 1",
//	configMINIMAL_STACK_SIZE,
//	NULL,
//	1,
//	&Load_1_Simulation_TaskHandler,
//	LOAD_1_SIMULATION_DELAY);
//
//  /* Create the load simulation task (Load 2 simulation) */
//  xTaskPeriodicCreate(
//	Load_2_Simulation,
//	"Load 2",
//	configMINIMAL_STACK_SIZE,
//	NULL,
//	1,
//	&Load_2_Simulation_TaskHandler,
//	LOAD_2_SIMULATION_DELAY);  



/*---------------------------------*/
 xTaskCreate(
  Button_1_Monitor_Task,
	"Button 1",
	configMINIMAL_STACK_SIZE,
	NULL,
	1,
	&Button_1_Monitor_TASK_TaskHandler);
  
  /* Create Task 2 (Button 2 Monitor) */
  xTaskCreate(
	Button_2_Monitor_Task,
	"Button 2",
	configMINIMAL_STACK_SIZE,
	NULL,
	1,
	&Button_2_Monitor_TASK_TaskHandler);

  /* Create the periodic task */
  xTaskCreate(
	Periodic_Transmitter_Task,
	"Periodic",
	configMINIMAL_STACK_SIZE,
	NULL,
	1,
	&Periodic_Transmitter_TaskHandler);

  /* Create the consumer task (UART recieve) */
  xTaskCreate(
	Uart_Receiver_Task,
	"UART",
	configMINIMAL_STACK_SIZE,
	NULL,
	1,
	&Uart_Receiver_TaskHandler);
  
  /* Create the load simulation task (Load 1 simulation) */
  xTaskCreate(
	Load_1_Simulation,
	"Load 1",
	configMINIMAL_STACK_SIZE,
	NULL,
	1,
	&Load_1_Simulation_TaskHandler);

  /* Create the load simulation task (Load 2 simulation) */
  xTaskCreate(
	Load_2_Simulation,
	"Load 2",
	configMINIMAL_STACK_SIZE,
	NULL,
	1,
	&Load_2_Simulation_TaskHandler);  

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


