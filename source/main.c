/*
	Hello World example made by Aurelio Mannara for libctru
	This code was modified for the last time on: 12/12/2014 21:00 UTC+1
*/

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "coremark.h"

int ee_printf(const char *format, ...){
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	printf(buffer);
	return 1;
}

uint32_t Arduino_millis(void){
	return (svcGetSystemTick() / (CPU_TICKS_PER_MSEC));
} 

static void coremark_thread(){
	coremark_main();
}


int main(int argc, char **argv){
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);

	//Move the cursor to row 15 and column 19 and then prints "Hello World!"
	//To move the cursor you have to print "\x1b[r;cH", where r and c are respectively
	//the row and column where you want your cursor to move
	//The top screen has 30 rows and 50 columns
	//The bottom screen has 30 rows and 40 columns
	printf("CoreMark 3DS Port by Brais Solla G.\n");

	bool isNew3DS = false;
	APT_CheckNew3DS(&isNew3DS);

	if(isNew3DS){
		printf("New 3DS. Clock configured at 804 MHz \n");
		osSetSpeedupEnable(true);
	} else {
		printf("Old 3DS. Clock default at 268 MHz\n");
	}

	gfxFlushBuffers();
	gfxSwapBuffers();

	gspWaitForVBlank();

	// Clock calibration (This is OK!)
	// printf("Clock calib: %d\n", (int) (svcGetSystemTick() / (CPU_TICKS_PER_MSEC)));
	// svcSleepThread(1000000ULL * 1000);
	// printf("Clock 1 sec: %d\n", (int) (svcGetSystemTick() / (CPU_TICKS_PER_MSEC)));

	printf("Starting CoreMark for core 0...\n");
	coremark_main();

	gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();

	gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();

	
	int cpu = 0;
	if(isNew3DS){
		cpu = 2;
	} else {
		cpu = 1;
		// Enable thread creation on core 1 (Shared with the OS)
		APT_SetAppCpuTimeLimit(80);
	}

	s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);


	printf("Starting CoreMark for core %d...\n", cpu);
	Thread cmt_handle = threadCreate(coremark_thread, 0, 128 * 1024, prio - 1, cpu, true);
	// threadJoin(cmt_handle, U64_MAX);


	while (aptMainLoop()){
		
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}
