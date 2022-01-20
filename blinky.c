/*
 * blinky.c -- working with Serial I/O and GPIO
 *
 * Assumes the LED's are connected to AXI_GPIO_0, on channel 1
 *
 * Terminal Settings:
 *  -Baud: 115200
 *  -Data bits: 8
 *  -Parity: no
 *  -Stop bits: 1
 */
#include <stdio.h>							/* printf(), getchar() */
#include "xil_types.h"					/* u32, u16 etc */
#include "platform.h"						/* ZYBOboard interface */
#include <xgpio.h>							/* Xilinx GPIO functions */
#include "xparameters.h"				/* constants used by the hardware */
#include "led.h"
#include "io.h"
#include "gic.h"
#include "ttc.h"

#define OUTPUT 0x0							/* setting GPIO direction to output */
#define CHANNEL1 1							/* channel 1 of the GPIO port */



void callback(u32 led_num){
	led_toggle(led_num);
}

void timer_callback(){
	led_toggle(4);
}


int main() {

   init_platform();							/* initialize the hardware platform */
   led_init();
   if (gic_init() == 0){
	   io_btn_init(callback);
	   io_sw_init(callback);
	   led_init();
	   ttc_init(1, timer_callback);
	   ttc_start();
   }
   size_t BUFF = 80;
   char line[BUFF];




   /*
		* set stdin unbuffered, forcing getchar to return immediately when
		* a character is typed.
		*/
	 setvbuf(stdin,NULL,_IONBF,0);
	 printf("[Hello]\n");

	 void getLine(char *final_string, size_t size){
		 char curr;
		 curr=getchar();
		 printf("%c", curr);
		 int i = 0;

		 while (curr != 13){
			 final_string[i] = curr;
			 i++;
			 curr = getchar();
			 printf("%c", curr);
			 fflush(stdout);
		 }

		 final_string[i] = '\0';
	 }


	do{
		printf(">");
		getLine(line, BUFF);
		printf("\n");
		fflush(stdout);

		if (strcmp(line, "0") == 0 || strcmp(line, "1") == 0 || strcmp(line, "2") == 0 || strcmp(line, "3") == 0){

			printf("[%s]\n\r", line);

			if (strcmp(line, "0") == 0){
				led_toggle(0);
				//printf("[0 on]\n\r");
			}

			if (strcmp(line, "1") == 0){
				led_toggle(1);
				//printf("[1 on]\n\r");
			}
			if (strcmp(line, "2") == 0){
				led_toggle(2);
				//printf("[2 on]\n\r");
			}
			if (strcmp(line, "3") == 0){
				led_toggle(3);
				//printf("[3 on]\n\r");
			}


		}

	}while(strcmp(line, "q") != 0);


	ttc_stop();
	led_set(ALL, false);
	led_set(4, false);


	io_btn_close();
	io_sw_close();
	ttc_close();
	gic_close();
	cleanup_platform();					/* cleanup the hardware platform */
	return 0;
}
