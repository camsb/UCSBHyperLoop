#include "sdcard.h"

const char sd_file_names[MAX_FILES][32] = {
	"logfile",
	"datafile"
};

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

STATIC FATFS fatFS;	/* File system object */
STATIC FIL fileObj;	/* File object */
STATIC INT buffer[SD_BUFFER_SIZE / 4];		/* Working buffer */
STATIC volatile int32_t sdcWaitExit = 0;
STATIC SDMMC_EVENT_T *event;
STATIC volatile Status  eventResult = ERROR;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* SDMMC card info structure */
SDMMC_CARD_T sdCardInfo;
volatile uint32_t timerCntms = 0; /* Free running milli second timer */

/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* Delay callback for timed SDIF/SDMMC functions */
STATIC void waitMs(uint32_t time)
{
	uint32_t init = timerCntms;

	/* In an RTOS, the thread would sleep allowing other threads to run.
	   For standalone operation, we just spin on a timer */
	while (timerCntms < init + time) {}
}

/**
 * @brief	Sets up the event driven wakeup
 * @param	pEvent : Event information
 * @return	Nothing
 */
STATIC void setupEvWakeup(void *pEvent)
{
#ifdef SDC_DMA_ENABLE
	/* Wait for IRQ - for an RTOS, you would pend on an event here with a IRQ based wakeup. */
	NVIC_ClearPendingIRQ(DMA_IRQn);
#endif
	event = (SDMMC_EVENT_T *)pEvent;
	sdcWaitExit = 0;
	eventResult = ERROR;
#ifdef SDC_DMA_ENABLE
	NVIC_EnableIRQ(DMA_IRQn);
#endif /*SDC_DMA_ENABLE*/
}

/**
 * @brief	A better wait callback for SDMMC driven by the IRQ flag
 * @return	0 on success, or failure condition (Nonzero)
 */
STATIC uint32_t waitEvIRQDriven(void)
{
	/* Wait for event, would be nice to have a timeout, but keep it  simple */
	while (sdcWaitExit == 0) {}
	if (eventResult) {
		return 0;
	}

	return 1;
}

/* Initialize the Timer at 1us */
STATIC void initAppTimer(void)
{
	/* Setup Systick to tick every 1 milliseconds */
	SysTick_Config(SystemCoreClock / 1000);
}

/* Initialize SD/MMC */
STATIC void initAppSDMMC()
{
	memset(&sdCardInfo, 0, sizeof(sdCardInfo));
	sdCardInfo.evsetup_cb = setupEvWakeup;
	sdCardInfo.waitfunc_cb = waitEvIRQDriven;
	sdCardInfo.msdelay_func = waitMs;

	Board_SDC_Init();

	Chip_SDC_Init(LPC_SDC);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Error processing function: stop with dying message
 * @param	rc	: FatFs return value
 * @return	Nothing
 */
void die(FRESULT rc)
{
	DEBUGOUT("Failed with rc=%u.\r\n", rc);
	//	for (;; ) {}
}

/**
 * @brief	System tick interrupt handler
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	timerCntms++;
}

#ifdef SDC_DMA_ENABLE
/**
 * @brief	GPDMA interrupt handler sub-routine
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	eventResult = Chip_GPDMA_Interrupt(LPC_GPDMA, event->DmaChannel);
	sdcWaitExit = 1;
	NVIC_DisableIRQ(DMA_IRQn);
}
#endif /*SDC_DMA_ENABLE*/

/**
 * @brief	SDC interrupt handler sub-routine
 * @return	Nothing
 */
void SDIO_IRQHandler(void)
{
	int32_t Ret;
#ifdef SDC_DMA_ENABLE
	Ret = Chip_SDMMC_IRQHandler(LPC_SDC, NULL,0,NULL,0);
#else
	if(event->Index < event->Size) {
		if(event->Dir) { /* receive */
			Ret = Chip_SDMMC_IRQHandler(LPC_SDC, NULL,0,(uint8_t*)event->Buffer,&event->Index);
		}
		else {
			Ret = Chip_SDMMC_IRQHandler(LPC_SDC, (uint8_t*)event->Buffer,&event->Index,NULL,0);
		}
	}
	else {
		Ret = Chip_SDMMC_IRQHandler(LPC_SDC, NULL,0,NULL,0);
	}
#endif
	if(Ret < 0) {
		eventResult = ERROR;
		sdcWaitExit = 1;
	}
#ifndef SDC_DMA_ENABLE
	else if(Ret == 0) {
		eventResult = SUCCESS;
		sdcWaitExit = 1;
	}
#endif
}

void sdcardInit() {
	initAppSDMMC();

	/* Initialize Repetitive Timer */
	initAppTimer();

	/* Enable SD interrupt */
	NVIC_EnableIRQ(SDC_IRQn);

	f_mount(0, &fatFS);		/* Register volume work area (never fails) */

	initSessionFiles();
}

void writeData(char* filename, const void *buff, int size){
	FRESULT rc;		/* Result code */
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */
	char debugBuf[64];
	UINT bw, i;
	debugstr("Create a new file filename.\r\n");
	rc = f_open(&fileObj, filename, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		die(rc);
	}
	else {

		debugstr("Write a sensor data\r\n");


		rc = f_write(&fileObj, buff, size, &bw);

		if (rc) {
			die(rc);
		}
		else {
			sprintf(debugBuf, "%u bytes written.\r\n", bw);
			debugstr(debugBuf);
		}
		debugstr("Close the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}
	debugstr("Open root directory.\r\n");
	rc = f_opendir(&dir, "");
	if (rc) {
		die(rc);
	}
	else {
		debugstr("Directory listing...\r\n");
		for (;; ) {
			/* Read a directory item */
			rc = f_readdir(&dir, &fno);
			if (rc || !fno.fname[0]) {
				break;					/* Error or end of dir */
			}
			if (fno.fattrib & AM_DIR) {
				sprintf(debugBuf, "   <dir>  %s\r\n", fno.fname);
			}
			else {
				sprintf(debugBuf, "   %8lu  %s\r\n", fno.fsize, fno.fname);
			}
			debugstr(debugBuf);
		}
		if (rc) {
			die(rc);
		}
	}
	debugstr("Test completed.\r\n");
}
void readData(char *filename){
	FRESULT rc;		/* Result code */
	UINT br, i;
	uint8_t *ptr;

	debugstr("Open an existing file.\r\n");

	rc = f_open(&fileObj, filename, FA_READ);
	if (rc) {
		die(rc);
	}
	else {
		for (;; ) {
			/* Read a chunk of file */
			rc = f_read(&fileObj, buffer, sizeof buffer, &br);
			if (rc || !br) {
				break;					/* Error or end of file */
			}
			ptr = (uint8_t *) buffer;
			for (i = 0; i < br; i++) {	/* Type the data */
				DEBUGOUT("%c", ptr[i]);
			}
		}
		if (rc) {
			die(rc);
		}

		debugstr("Close the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}
}

void readTest() {
	FRESULT rc;		/* Result code */
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */
	UINT br, i;
	uint8_t *ptr;
	char debugBuf[64];

	debugstr("\r\nOpen an existing file (message.txt).\r\n");

	rc = f_open(&fileObj, "MESSAGE.TXT", FA_READ);
	if (rc) {
		die(rc);
	}
	else {
		for (;; ) {
			/* Read a chunk of file */
			rc = f_read(&fileObj, buffer, sizeof buffer, &br);
			if (rc || !br) {
				break;					/* Error or end of file */
			}
			ptr = (uint8_t *) buffer;
			for (i = 0; i < br; i++) {	/* Type the data */
				DEBUGOUT("%c", ptr[i]);
			}
		}
		if (rc) {
			die(rc);
		}

		debugstr("\r\nClose the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}
}

void writeTest() {
	FRESULT rc;		/* Result code */
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */
	UINT bw, i;
	uint8_t *ptr;
	char debugBuf[64];

	debugstr("Create a new file (hello.txt).\r\n");
	rc = f_open(&fileObj, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		die(rc);
	}
	else {

		debugstr("Write a text data. (Hello world!)\r\n");

			rc = f_write(&fileObj, "Hello world!\r\n", 14, &bw);
		if (rc) {
			die(rc);
		}
		else {
			sprintf(debugBuf, "%u bytes written.\r\n", bw);
			debugstr(debugBuf);
		}
		debugstr("Close the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}
	debugstr("Open root directory.\r\n");
	rc = f_opendir(&dir, "");
	if (rc) {
		die(rc);
	}
	else {
		debugstr("Directory listing...\r\n");
		for (;; ) {
			/* Read a directory item */
			rc = f_readdir(&dir, &fno);
			if (rc || !fno.fname[0]) {
				break;					/* Error or end of dir */
			}
			if (fno.fattrib & AM_DIR) {
				sprintf(debugBuf, "   <dir>  %s\r\n", fno.fname);
			}
			else {
				sprintf(debugBuf, "   %8lu  %s\r\n", fno.fsize, fno.fname);
			}
			debugstr(debugBuf);
		}
		if (rc) {
			die(rc);
		}
	}
	debugstr("Test completed.\r\n");
}

void initSessionFiles() {
	FRESULT rc;		/* Result code */

	// TODO Move old files to an archive directory before creating new files.

	debugstr("Opening new session files.\n");

	int i;
	for(i=0; i<MAX_FILES; i++) {
		rc = f_open(&sd_files[i], sd_file_names[i], FA_WRITE | FA_CREATE_ALWAYS);
		if(rc) die(rc);
	}
}

void deinitSessionFiles() {
	FRESULT rc;		/* Result code */

	debugstr("Closing session files.\n");

	int i;
	for(i=0; i<MAX_FILES; i++) {
		rc = f_close(&sd_files[i]);
		if(rc) die(rc);
	}
}

#if 0
/**
 * @brief	Main routine for SDMMC example
 * @return	Nothing
 */
int main(void)
{
	FRESULT rc;		/* Result code */
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */
	UINT bw, br, i;
	uint8_t *ptr;
	char debugBuf[64];

	SystemCoreClockUpdate();
	Board_Init();

	initAppSDMMC();

	/* Initialize Repetitive Timer */
	initAppTimer();

	debugstr("\r\nHello NXP Semiconductors\r\nSD Card demo\r\n");

	/* Enable SD interrupt */
	NVIC_EnableIRQ(SDC_IRQn);

	f_mount(0, &fatFS);		/* Register volume work area (never fails) */

	debugstr("\r\nOpen an existing file (message.txt).\r\n");

	rc = f_open(&fileObj, "MESSAGE.TXT", FA_READ);
	if (rc) {
		die(rc);
	}
	else {
		for (;; ) {
			/* Read a chunk of file */
			rc = f_read(&fileObj, buffer, sizeof buffer, &br);
			if (rc || !br) {
				break;					/* Error or end of file */
			}
			ptr = (uint8_t *) buffer;
			for (i = 0; i < br; i++) {	/* Type the data */
				DEBUGOUT("%c", ptr[i]);
			}
		}
		if (rc) {
			die(rc);
		}

		debugstr("\r\nClose the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}

	debugstr("\r\nCreate a new file (hello.txt).\r\n");
	rc = f_open(&fileObj, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) {
		die(rc);
	}
	else {

		debugstr("\r\nWrite a text data. (Hello world!)\r\n");

		rc = f_write(&fileObj, "Hello world!\r\n", 14, &bw);
		if (rc) {
			die(rc);
		}
		else {
			sprintf(debugBuf, "%u bytes written.\r\n", bw);
			debugstr(debugBuf);
		}
		debugstr("\r\nClose the file.\r\n");
		rc = f_close(&fileObj);
		if (rc) {
			die(rc);
		}
	}
	debugstr("\r\nOpen root directory.\r\n");
	rc = f_opendir(&dir, "");
	if (rc) {
		die(rc);
	}
	else {
		debugstr("\r\nDirectory listing...\r\n");
		for (;; ) {
			/* Read a directory item */
			rc = f_readdir(&dir, &fno);
			if (rc || !fno.fname[0]) {
				break;					/* Error or end of dir */
			}
			if (fno.fattrib & AM_DIR) {
				sprintf(debugBuf, "   <dir>  %s\r\n", fno.fname);
			}
			else {
				sprintf(debugBuf, "   %8lu  %s\r\n", fno.fsize, fno.fname);
			}
			debugstr(debugBuf);
		}
		if (rc) {
			die(rc);
		}
	}
	debugstr("\r\nTest completed.\r\n");
	for (;; ) {}
}

#endif	// 0
