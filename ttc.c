/*
 * ttc.h
 *
 * NOTE: The TTC hardware must be enabled (Timer 0 on the processing system) before it can be used!!
 *
 */

#include <stdio.h>
#include "xttcps.h"
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */
#include "xttcps.h"
#include "gic.h"

static XTtcPs timerPort;

static void (*saved_timer_callback)();

static void ttc_handler(void *dev){
	XTtcPs *timer = (XTtcPs*)dev; // ask if this is necessary
	saved_timer_callback();
	XTtcPs_ClearInterruptStatus(timer, XTTCPS_IXR_INTERVAL_MASK);
}


/*
 * ttc_init -- initialize the ttc freqency and callback
 */
void ttc_init(u32 freq, void (*ttc_callback)(void)){

	XTtcPs_Config *config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_DEVICE_ID);
	s32 started = XTtcPs_CfgInitialize(&timerPort, config, config->BaseAddress);

	saved_timer_callback = ttc_callback;

	if (started==0){
	   XInterval inter;
	   u8 prescale;
	   // how many times per second
	   XTtcPs_CalcIntervalFromFreq(&timerPort, freq, &inter, &prescale);
	   XTtcPs_SetPrescaler(&timerPort, prescale);
	   XTtcPs_SetInterval(&timerPort, inter);
	   if (XTtcPs_SetOptions(&timerPort, XTTCPS_OPTION_INTERVAL_MODE)==0){
		   if (gic_connect(XPAR_XTTCPS_0_INTR, ttc_handler, &timerPort)){
			   XTtcPs_DisableInterrupts(&timerPort, XTTCPS_IXR_INTERVAL_MASK);
		   }
	   }

	}
}


/*
 * ttc_start -- start the ttc
 */
void ttc_start(void){
	XTtcPs_EnableInterrupts(&timerPort, XTTCPS_IXR_INTERVAL_MASK);
	XTtcPs_Start(&timerPort);
}

/*
 * ttc_stop -- stop the ttc
 */
void ttc_stop(void){
	XTtcPs_Stop(&timerPort);
	XTtcPs_DisableInterrupts(&timerPort, XTTCPS_IXR_INTERVAL_MASK);
}

/*
 * ttc_close -- close down the ttc
 */
void ttc_close(void){
	gic_disconnect(XPAR_XTTCPS_0_INTR);
}
