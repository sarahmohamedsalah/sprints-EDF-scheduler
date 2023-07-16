/*
 * FreeRTOS Kernel V10.3.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc21xx.h>
#include "GPIO.h"

extern unsigned int g_u32_button_1_in_time, 								g_u32_button_1_out_time,							g_u32_button_1_total_time;
extern unsigned int g_u32_button_2_in_time, 								g_u32_button_2_out_time,							g_u32_button_2_total_time;
extern unsigned int g_u32_load_1_in_time , 								g_u32_load_1_out_time,								g_u32_load_1_total_time;
extern unsigned int g_u32_load_2_in_time , 								g_u32_load_2_out_time,								g_u32_load_2_total_time;
extern unsigned int g_u32_periodic_transmitter_in_time, 		g_u32_periodic_transmitter_out_time,	g_u32_periodic_transmitter_total_time;
extern unsigned int g_u32_uart_receiver_in_time, 					g_u32_uart_receiver_out_time, 				g_u32_uart_receiver_total_time;
extern unsigned int g_u32_system_time;
extern unsigned int g_u32_cpu_load;

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		    1
#define configUSE_IDLE_HOOK					1
#define configUSE_TICK_HOOK			    1
#define configCPU_CLOCK_HZ			    ( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			    ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		    ( 4 )
#define configMINIMAL_STACK_SIZE	    ( ( unsigned short ) 200 )
#define configTOTAL_HEAP_SIZE		    ( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN		    ( 8 )
#define configUSE_TRACE_FACILITY	    1
#define configUSE_16_BIT_TICKS		    0
#define configIDLE_SHOULD_YIELD		    1

#define configUSE_MUTEXES               	1
#define configUSE_APPLICATION_TASK_TAG  	1
#define configUSE_EDF_SCHEDULER         	1
#define configUSE_TIME_SLICING          	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		    0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

#define edfConfigIDLE_PERIOD            105 /* SYSTEM TASKS' PERIODS MUST NOT EXCEED THIS VALUE */

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay						1

//#define traceTASK_SWITCHED_IN() GPIO_write(PORT_0, (int)pxCurrentTCB->pxTaskTag, PIN_IS_HIGH)
//#define traceTASK_SWITCHED_OUT() GPIO_write(PORT_0, (int)pxCurrentTCB->pxTaskTag, PIN_IS_LOW)
#define traceTASK_SWITCHED_IN()			do{																										\
		GPIO_write(PORT_0, (int)pxCurrentTCB->pxTaskTag, PIN_IS_HIGH);												\
		if((int)pxCurrentTCB -> pxTaskTag == PIN2)																						\
		{																																											\
			g_u32_button_1_in_time = T1TC;																											\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN3)																				\
		{																																											\
			g_u32_button_2_in_time = T1TC;																											\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN5)																				\
		{																																											\
			g_u32_periodic_transmitter_in_time = T1TC;																					\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN7)																				\
		{																																											\
			g_u32_load_1_in_time = T1TC;																												\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN8)																				\
		{																																											\
			g_u32_load_2_in_time = T1TC;																												\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN6)																				\
		{																																											\
			g_u32_uart_receiver_in_time = T1TC;																									\
		}																																											\
	}while(0)


#define traceTASK_SWITCHED_OUT()		do{																										\
		GPIO_write(PORT_0, (int)pxCurrentTCB->pxTaskTag, PIN_IS_LOW);													\
		if((int)pxCurrentTCB -> pxTaskTag == PIN2)																						\
		{																																											\
			g_u32_button_1_out_time = T1TC;																											\
			g_u32_button_1_total_time += (g_u32_button_1_out_time - g_u32_button_1_in_time);		\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN3)																				\
		{																																											\
			g_u32_button_2_out_time = T1TC;																											\
			g_u32_button_2_total_time += (g_u32_button_2_out_time - g_u32_button_2_in_time);		\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN5)																				\
		{																																											\
					g_u32_periodic_transmitter_out_time = T1TC;																			\
					g_u32_periodic_transmitter_total_time += (g_u32_periodic_transmitter_out_time - g_u32_periodic_transmitter_in_time);\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN7)																				\
		{																																											\
					g_u32_load_1_out_time = T1TC;																										\
					g_u32_load_1_total_time += (g_u32_load_1_out_time - g_u32_load_1_in_time);			\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN8)																				\
		{																																											\
					g_u32_load_2_out_time = T1TC;																										\
					g_u32_load_2_total_time += (g_u32_load_2_out_time - g_u32_load_2_in_time);			\
		}																																											\
		else if((int)pxCurrentTCB -> pxTaskTag == PIN6)																				\
		{																																											\
					g_u32_uart_receiver_out_time = T1TC;																						\
					g_u32_uart_receiver_total_time += (g_u32_uart_receiver_out_time - g_u32_uart_receiver_in_time);\
		}																																											\
		g_u32_system_time = T1TC;																															\
		g_u32_cpu_load = ((g_u32_button_1_total_time + g_u32_button_2_total_time + g_u32_periodic_transmitter_total_time + g_u32_load_1_total_time + g_u32_load_2_total_time+ g_u32_uart_receiver_total_time)/ (float)g_u32_system_time) * 100;\
	}while(0)

/* Optional Task: System Statistics */
/* configure run-time stats */
#define configUSE_STATS_FORMATTING_FUNCTIONS					1
#define configGENERATE_RUN_TIME_STATS							1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()					(T1TC)

#endif /* FREERTOS_CONFIG_H */
