#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <xgpio.h>		  	/* axi gpio */
#include <xgpiops.h>		/* processor gpio */
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */
#include "platform.h"
#include "led.h"
/* led states */
#define LED_ON true
#define LED_OFF false

#define ALL 0xFFFFFFFF		/* A value designating ALL leds */

#define OUTPUT 0x0							/* setting GPIO direction to output */
#define CHANNEL1 1							/* channel 1 of the GPIO port */

static XGpio port;
static XGpio port_6;
static XGpioPs portPs;

static bool led_4_status = false;

/*
 * Initialize the led module
 */
void led_init(void){									/* GPIO port connected to the leds */
	init_platform();							/* initialize the hardware platform */

    XGpio_Initialize(&port, XPAR_AXI_GPIO_0_DEVICE_ID);	/* initialize device AXI_GPIO_0 */
    XGpio_SetDataDirection(&port, CHANNEL1, OUTPUT);	    /* set tristate buffer to output */

    XGpio_Initialize(&port_6, XPAR_AXI_GPIO_1_DEVICE_ID);	/* initialize device AXI_GPIO_0 */
    XGpio_SetDataDirection(&port_6, CHANNEL1, OUTPUT);	    /* set tristate buffer to output */


    //XPAR_XGPIOPS_0_DEVICE_ID
    //XPAR_AXI_GPIO_0_DEVICE_ID
    XGpioPs_Config *ps_config = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&portPs, ps_config, ps_config->BaseAddr);

    XGpioPs_SetDirectionPin(&portPs, 7, 1); // 1 is for output, 0 for input
    XGpioPs_SetOutputEnablePin(&portPs, 7, 1); // enables output

}

/*
 * Set <led> to one of {LED_ON,LED_OFF,...}
 *
 * <led> is either ALL or a number >= 0
 * Does nothing if <led> is invalid
 */
void led_set(u32 led, bool tostate){

	u32 current_reg = XGpio_DiscreteRead(&port, CHANNEL1);
	u32 new_reg;

	if (led == ALL){
		if (tostate){
			current_reg = 0xF;
		}else{
			current_reg = 0x0;
		}
	}

	if (led == 0){
		new_reg = 0x1;
	}

	if (led == 1){
		new_reg = 0x2;
	}

	if (led == 2){
		new_reg = 0x4;
	}


	if (led == 3){
		new_reg = 0x8;
	}

	if (led_get(led) == LED_OFF && tostate == LED_OFF){
		new_reg = 0x0;
	}

	if (tostate){
		current_reg = current_reg | new_reg;
	}else{
		current_reg = (current_reg ^ new_reg);
	}

	if (led == 4){
		if (tostate == LED_ON){
			XGpioPs_WritePin(&portPs, 7, 0x1);
		}else{
			XGpioPs_WritePin(&portPs, 7, 0x0);
		}
		led_4_status = !(led_4_status);
		return;
	}

	if (led == 6){
		current_reg = 0x33333336;
		if (tostate){
			XGpio_DiscreteWrite(&port_6, CHANNEL1, current_reg);
			return;
		}else{
			XGpio_DiscreteWrite(&port_6, CHANNEL1, 0x0);
			return;
		}
	}

	XGpio_DiscreteWrite(&port, CHANNEL1, current_reg);
}

/*
 * Get the status of <led>
 *
 * <led> is a number >= 0
 * returns {LED_ON,LED_OFF,...}; LED_OFF if <led> is invalid
 */
bool led_get(u32 led){

	u32 current_reg = XGpio_DiscreteRead(&port, CHANNEL1);
	u32 new_reg;


	if (led == 0){
		new_reg = 0x1;

		current_reg = current_reg & new_reg;

		if (current_reg != 0x0){
			return LED_ON;
		}else{
			return LED_OFF;
		}

	}
	if (led == 1){
		new_reg = 0x2;

		current_reg = current_reg & new_reg;

		if (current_reg != 0x0){
			return LED_ON;
		}else{
			return LED_OFF;
		}

	}

	if (led == 2){
		new_reg = 0x4;

		current_reg = current_reg & new_reg;

		if (current_reg != 0x0){
			return LED_ON;
		}else{
			return LED_OFF;
		}

	}

	if (led == 3){
		new_reg = 0x8;

		current_reg = current_reg & new_reg;

		if (current_reg != 0x0){
			return LED_ON;
		}else{
			return LED_OFF;
		}

	}

	if (led == 4){
		if (led_4_status == LED_OFF){
			return LED_OFF;
		}else{
			return LED_ON;
		}

	}

	return LED_OFF;

}

/*
 * Toggle <led>
 *
 * <led> is a value >= 0
 * Does nothing if <led> is invalid
 */
void led_toggle(u32 led){

	if (led_get(led) == LED_OFF){
		led_set(led, LED_ON);
		printf("[%ld on]\n\r", led);
	}else{
		led_set(led, LED_OFF);
		printf("[%ld off]\n\r", led);
	}

}


//int main(){
//
//	led_init();

////    led_set(ALL, LED_ON);
////    led_set(ALL, LED_OFF);
////
////    led_set(0, LED_ON);
////    led_set(1, LED_ON);
////    led_set(2, LED_ON);
////    led_set(3, LED_ON);
////
////    led_set(0, LED_OFF);
////    led_set(1, LED_OFF);
////    led_set(2, LED_OFF);
////    led_set(3, LED_OFF);
//
//
////    led_set(ALL, LED_ON);
////
////    led_set(2, LED_OFF);
////    led_set(1, LED_OFF);
////
////
////    printf("%d\n\r", led_get(0));
////    printf("%d\n\r", led_get(1));
////    printf("%d\n\r", led_get(2));
////    printf("%d\n\r", led_get(3));
//
//    led_set(ALL, LED_ON);
////
////    led_toggle(0);
////    led_toggle(1);
////    led_toggle(2);
////    led_toggle(3);
////
////    led_toggle(3);
////    led_toggle(2);
////    led_toggle(1);
////    led_toggle(0);
//
//    led_set(2, LED_ON);
//    led_set(2, LED_OFF);
//    led_set(2, LED_OFF);

//	led_set(4, LED_ON);
//	led_set(6, LED_ON);
//	printf("Done\n\r");
//    cleanup_platform();
//	return 0;
//}
