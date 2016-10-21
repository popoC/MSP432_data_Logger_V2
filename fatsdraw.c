/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *  2016..10 21
 */
/* *  ======== fatsdraw.c ======== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
/* XDCtools Header files */
#include <xdc/std.h>
//#include <xdc/runtime/System.h>
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>
#include <ti/drivers/UART.h>

#include <ti/mw/fatfs/ff.h>
/* Example/Board Header files */
#include "Board.h"
#include "msp.h"
#include "driverlib.h"

#include "hp_time_po.h"

#include "logger.h"

#define WriteSD_TASKSTACKSIZE    2048
#define Uart_TASKSTACKSIZE       3076


//------------------------------------------------------------------
int Logger_status = 0;
//----   0   開機上電後            閃紅燈   右 -- LED1
//----   1    GPS訊號對時完成  閃綠燈   右 -- LED2

//------ 0與1的時候一樣會輸出即時訊號  可檢查波形----需30分內完成----

//----   2   啟動紀錄模式         閃紅燈   左 -- LED0


//----   3   停止紀錄與GPS校時完成  全亮

//--------    異常出現 閃藍燈 ---LED3  --- 次數為出現異常的點
int error_code = 0;
//--------
//--------






Task_Struct WriteSD_taskStruct;
Task_Struct Uart_taskStruct;

Char WriteSD_taskStack[WriteSD_TASKSTACKSIZE];
Char Uart_taskStack[Uart_TASKSTACKSIZE];


//----UART--------------------------------------------------------
int outPUT_mms = 0;
char printbuffer[160];
unsigned char ahex2bin (unsigned char MSB, unsigned char LSB);

//----------------------------------------------------------------

//----Time Base---------------------
//signed long long NOW_Time = 6064148192292480; //--- 10-6 sec

signed long long NOW_Time = 1451606400000000; // 2016/01/01/ 0:00:00.00

volatile uint32_t curValue;

volatile uint32_t seaScan_PPS_ticks;



volatile int32_t one_sec_ticks;

//-----ADS1281----------------------
unsigned long ads1281_data1 =0;
unsigned long ads1281_data2 =0;
unsigned long ads1281_data3 =0;
unsigned long ads1281_data4 =0;

unsigned long print_ads1281_data[4];
float print_ads1281_value[4];
float ads1281_value = 0;

int get_print = 0;
//-----ADS1281----------------------

/* Drive number used for FatFs */
#define DRIVE_NUM           0
//#define DATA_BUF_SIZE       300*4 // 4CH
#define DATA_BUF_SIZE       1200*3 //3Byte*4CH *100Hz =1200 /=/1sec data,


char  inputfile[30];

hptime_t fatfs_write_Time = 0;
//----fatfs--------------------------
//-  adc 1sec = 100 sample * 3Byte = 300Bytes

FIL src;

int fileSize = 60/10; //--- 60 sec 1 file
int adc_counter = 0;

int start_loggering = 0; //-- start REC
int databuuer_is_Full = 0;

char textarray[DATA_BUF_SIZE];
char data_buffer[DATA_BUF_SIZE];


//-------------- Get Time
int first_GPS_string = 0;
int first_GPS_pps = 0;


hptime_t gps_time = 0;
hptime_t gps_time2 = 0;
hptime_t time_offset_now;
hptime_t time_offset_first;

char GPS_Time_String[]  = "2000/01/01 00:00:00.000000";


hptime_t Get_Logger_Time(){
	hptime_t now_t;
	float time_drift;

	time_drift = (Clock_getTicks() - seaScan_PPS_ticks);
	time_drift =	time_drift	/ one_sec_ticks;
	time_drift = time_drift *1000000;

//	time_drift = Clock_getTicks() - seaScan_PPS_ticks;
//	time_drift = time_drift*100;

	now_t = NOW_Time +(long int)time_drift;			  //  0.1ms  (0-10000)

	//now_t = now_t + time_offset_first;


	return(now_t);
}


//---   FatFs Task
Void task_WriteSD_Fxn(UArg arg0, UArg arg1)
{


    FRESULT fresult;
    SDSPI_Handle sdspiHandle;
    SDSPI_Params sdspiParams;
    unsigned int bytesWritten = 0;       /* Variables to keep track of the file copy progress */

    SDSPI_Params_init(&sdspiParams);	 /* Mount and register the SD Card */
    sdspiHandle = SDSPI_open(Board_SDSPI0, DRIVE_NUM, &sdspiParams);
    if (sdspiHandle == NULL) {
       // System_abort("Error starting the SD card\n");
    }
    else {
       // System_printf("Drive %u is mounted\n", DRIVE_NUM);
    }

    int data_counter = 0;
    int file_sn = 0;

    	while(start_loggering==0){

    		 Task_sleep((UInt)arg0);
    	}

        while(start_loggering){
       		/* Open file for writing */
    	    sprintf(inputfile,"%05ld.vvt",file_sn);

    	    fresult = f_open(&src, inputfile, FA_CREATE_NEW|FA_WRITE);
    	    f_sync(&src);

    	    fatfs_write_Time = Get_Logger_Time();



    	    if (fresult == FR_OK) {
    	    	f_write(&src, &fatfs_write_Time, 8, &bytesWritten);
           		// System_printf("Creating a new file \"%s\"...", inputfile);
    	    	 file_sn++;
           		for(data_counter = 0;data_counter<fileSize;data_counter++){

           			while(!databuuer_is_Full){
           				Task_sleep((UInt)arg0);
           			}



           			GPIO_write(Board_LED2, Board_LED_ON);

           			f_write(&src, textarray, DATA_BUF_SIZE, &bytesWritten);


           			databuuer_is_Full = 0;

           			GPIO_write(Board_LED2, Board_LED_OFF);
           		}

           		f_sync(&src);
           		f_close(&src);
           		//GPIO_write(Board_LED1, Board_LED_ON);
        		/* Reset the internal file pointer */	//		f_lseek(&src, 0);
           	}
           	else {
           	  //	System_printf("Using existing copy of \"%s\"\n", inputfile);
           	}

        }

    SDSPI_close(sdspiHandle);    /* Stopping the SDCard */
   // System_printf("Drive %u unmounted\n", DRIVE_NUM);

    //BIOS_exit(0);
}




int avg_c = 0;
int delay_c = 20;

void hwi_GetADS1281Data_Fxn(unsigned int index)
{
	int i;
    /* Clear the GPIO interrupt and toggle an LED */
	avg_c++;

	if(avg_c==5){   //--  data rate = 500 Hz

		//GPIO_toggle(Board_LED1);

		ads1281_data1 = 0;		ads1281_data2 = 0;		ads1281_data3 = 0;		ads1281_data4 = 0;
		avg_c = 0;

		for(i=0;i<32;i++){
			//-- DRDY->SYNC min 100ns-------
			// -- min = 24/fclk == 0.000006 s
			GPIO_write(Board_ADS1281_SCLK, 1);

			ads1281_data3 = (ads1281_data3<<1) | (GPIO_read(Board_ADS1281_DOUT3));
			ads1281_data4 = (ads1281_data4<<1) | (GPIO_read(Board_ADS1281_DOUT4));
			ads1281_data1 = (ads1281_data1<<1) | (GPIO_read(Board_ADS1281_DOUT1));
			ads1281_data2 = (ads1281_data2<<1) | (GPIO_read(Board_ADS1281_DOUT2));
		//	while(delay_c!=0){				delay_c--;			}

			GPIO_write(Board_ADS1281_SCLK, 0);

		}

		(ads1281_data1 > 0x7fffffff) ? (ads1281_data1 = ads1281_data1 - 0x80000000) :(ads1281_data1 = ads1281_data1 + 0x80000000);
		(ads1281_data2 > 0x7fffffff) ? (ads1281_data2 = ads1281_data2 - 0x80000000) :(ads1281_data2 = ads1281_data2 + 0x80000000);
		(ads1281_data3 > 0x7fffffff) ? (ads1281_data3 = ads1281_data3 - 0x80000000) :(ads1281_data3 = ads1281_data3 + 0x80000000);
		(ads1281_data4 > 0x7fffffff) ? (ads1281_data4 = ads1281_data4 - 0x80000000) :(ads1281_data4 = ads1281_data4 + 0x80000000);


		if(get_print ==0){
			get_print = 1;
		  print_ads1281_data[0] = ads1281_data1;
		  print_ads1281_data[1] = ads1281_data2;
		  print_ads1281_data[2] = ads1281_data3;
		  print_ads1281_data[3] = ads1281_data4;
		}

		//  ads1281_value = ((double)ads1281_data / 0xffffffff)*5 - 2.5;

		data_buffer[adc_counter   ] = (ads1281_data1 >> 24 ) & 0xff ;
		data_buffer[adc_counter+1 ] = (ads1281_data1 >> 16 ) & 0xff ;
		data_buffer[adc_counter+2 ] = (ads1281_data1 >> 8 ) & 0xff ;
		data_buffer[adc_counter+3 ] = (ads1281_data2 >> 24 ) & 0xff ;
		data_buffer[adc_counter+4 ] = (ads1281_data2 >> 16 ) & 0xff ;
		data_buffer[adc_counter+5 ] = (ads1281_data2 >> 8 ) & 0xff ;
		data_buffer[adc_counter+6 ] = (ads1281_data3 >> 24 ) & 0xff ;
		data_buffer[adc_counter+7 ] = (ads1281_data3 >> 16 ) & 0xff ;
		data_buffer[adc_counter+8 ] = (ads1281_data3 >> 8 ) & 0xff ;
		data_buffer[adc_counter+9 ] = (ads1281_data4 >> 24 ) & 0xff ;
		data_buffer[adc_counter+10] = (ads1281_data4 >> 16 ) & 0xff ;
		data_buffer[adc_counter+11] = (ads1281_data4 >> 8 ) & 0xff ;
		adc_counter+=12;

       if(adc_counter==DATA_BUF_SIZE){

    	  //if(databuuer_is_Full==1)GPIO_write(Board_LED0, Board_LED_ON);
    	  memcpy(textarray,data_buffer,DATA_BUF_SIZE);
    	  adc_counter = 0;
    	  databuuer_is_Full = 1;
    	  //GPIO_toggle(Board_LED3);
       }

    //   GPIO_toggle(Board_LED1);
	}
   // ads1281_value = 0;
}



void hwi_GetSEAScanPPS_Fxn(unsigned int index){
	NOW_Time = NOW_Time+1000000;
	curValue = Clock_getTicks();  //-- 0.1ms ?
	one_sec_ticks =   curValue - seaScan_PPS_ticks;


	outPUT_mms = 1;

	seaScan_PPS_ticks = curValue ;


	switch(Logger_status){
			case 0:
				GPIO_toggle(Board_LED1);
			break;
			case 1:
				GPIO_toggle(Board_LED2);
			break;
			case 2:
				GPIO_toggle(Board_LED0);
			break;
			case 3:
				GPIO_write(Board_LED0, Board_LED_ON);
				GPIO_write(Board_LED1, Board_LED_ON);
				GPIO_write(Board_LED2, Board_LED_ON);
				GPIO_write(Board_LED3, Board_LED_ON);
			break;
	}






}

void hwi_GetGPS_PPS_Fxn(unsigned int index){

	gps_time = Get_Logger_Time();
	//gps_time = ms_timestr2hptime(GPS_Time_String);
	//GPIO_toggle(Board_LED0);

	time_offset_now = gps_time - gps_time2;


	if(first_GPS_string==1){

		time_offset_first = gps_time - gps_time2;
		NOW_Time = gps_time2;
		first_GPS_string = 2;  //--  校正完成

	}


}

void task_Uart_Fxn(UArg arg0, UArg arg1){

	char input;
	static char uart_rx_buf[100];
	static uint8_t uart_rx_buf_counter = 0;
    char checksum = 0;
    int ck = 0;

    char Gps_Date[6];    char Gps_Time[6];    char Gps_Staus[3];


    static int GPS_sig = 0;

    UART_Handle uart;
    UART_Params uartParams;
    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uartParams.readTimeout = 10;
    uart = UART_open(Board_UART0, &uartParams);  //-- P1.2 P1.3   Board_UART1 = P3.2 P3.3
   // if (uart == NULL)System_abort("Error opening the UART 0");

    int print_i;


    UART_Handle uart2;
    UART_Params uartParams2;
    UART_Params_init(&uartParams2);
    uartParams2.writeDataMode = UART_DATA_BINARY;
    uartParams2.readDataMode = UART_DATA_BINARY;
    uartParams2.readReturnMode = UART_RETURN_FULL;
    uartParams2.readEcho = UART_ECHO_OFF;
    uartParams2.baudRate = 9600;
    uartParams2.readTimeout = 10;
    uart2 = UART_open(Board_UART1, &uartParams2);  //-- P1.2 P1.3   Board_UART1 = P3.2 P3.3
   // if (uart2 == NULL)System_abort("Error opening the UART 1");




	while(1){
	 Task_sleep((UInt)arg0);
	 if(outPUT_mms==1){
		 outPUT_mms = 0;

		 if(GPS_sig ==1){
			 //sprintf(printbuffer,"GPS = %s , %s ,%s ,%lld, %lld \r\n", Gps_Date,Gps_Time,Gps_Staus,gps_time/100,gps_time2/100);
			 sprintf(printbuffer,"GPS = %s , %s ,%s ,%lld, %lld, %lld \r\n", Gps_Date,Gps_Time,Gps_Staus,gps_time/100,gps_time2/100,time_offset_now);
			 GPS_sig = 0;
		 }
		 else{
			 sprintf(printbuffer,"GPS_sig = 0, err  = %lld \r\n", Get_Logger_Time());
		 }
		 UART_write(uart, &printbuffer, strlen(printbuffer));


		if(get_print==1){
			get_print = 0;
			for(print_i=0;print_i<4;print_i++){
				print_ads1281_value[print_i] = (((double)print_ads1281_data[print_i] / 0xffffffff)*5 - 2.5);//0.5819;
			}
		     sprintf(printbuffer,"%9.7f %9.7f %9.7f %9.7f \r\n", print_ads1281_value[0],print_ads1281_value[1],print_ads1281_value[2],print_ads1281_value[3]);
	 	 //	 sprintf(printbuffer,"%8.5f %lu  \r\n", print_ads1281_value[0],print_ads1281_data[0]);
   // 	     UART_write(uart, &printbuffer, strlen(printbuffer));
		}
	 }




	   if(UART_read(uart2, &input, 1)>0){

		   uart_rx_buf[uart_rx_buf_counter] = input;
		   uart_rx_buf_counter++;

       	if(uart_rx_buf[uart_rx_buf_counter-1] == '\n'){
               if(strncmp("$GPRMC",uart_rx_buf,6)==0){

          //  	   sprintf(printbuffer,"%s",uart_rx_buf);
           // 	   UART_write(uart, &printbuffer, strlen(printbuffer));
               	   //check sum ---
            	   checksum = 0;
                   for(ck=1;ck<uart_rx_buf_counter-5;ck++){
                     checksum ^= uart_rx_buf[ck];
                   }
                   if(checksum == ahex2bin(uart_rx_buf[uart_rx_buf_counter-4],uart_rx_buf[uart_rx_buf_counter-3])){

                	   sscanf(uart_rx_buf,"$GPRMC,%[^,],%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,]",Gps_Time,Gps_Staus,Gps_Date);//--  %[^,] 可用來分割,內的字元

            	   	   if(Gps_Staus[0]=='A'){
            		   	   Gps_Staus[0]='N';
            		   	   GPIO_toggle(Board_LED0);
            		   	GPS_sig = 1;
            		   	//char GPS_Time_String[]  = "2000/01/01 00:00:00.000000";

            		   	GPS_Time_String[2] = Gps_Date[4];
            		   	GPS_Time_String[3] = Gps_Date[5];
            		   	GPS_Time_String[5] = Gps_Date[2];
            		   	GPS_Time_String[6] = Gps_Date[3];
            		   	GPS_Time_String[8] = Gps_Date[0];
            		   	GPS_Time_String[9] = Gps_Date[1];
            		   	GPS_Time_String[11] = Gps_Time[0];
            		   	GPS_Time_String[12] = Gps_Time[1];
            		   	GPS_Time_String[14] = Gps_Time[2];
            		   	GPS_Time_String[15] = Gps_Time[3];
            		   	GPS_Time_String[17] = Gps_Time[4];
            		   	GPS_Time_String[18] = Gps_Time[5];

            		   	gps_time2 = ms_timestr2hptime(GPS_Time_String);

            		   	first_GPS_string = 1;


            	   	   }

                   }
               }
               //--這裡可以增加不同面令間的使用
               //
               memset(uart_rx_buf,0,uart_rx_buf_counter+1);
       		uart_rx_buf_counter = 0;
       	}
       	else if(uart_rx_buf_counter>98){
            memset(uart_rx_buf,0,uart_rx_buf_counter+1);
    		uart_rx_buf_counter = 0;
       	}




	   }

	}
}


/*
 *  ======== main ========
 */
int main(void)
{
  /* Construct BIOS Objects */
  //  Clock_Params clkParams;

    Task_Params WriteSD_taskParams;
    Task_Params Uart_taskParams;


    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initSDSPI();
    Board_initUART();

    GPIO_write(Board_ADS1281_RESET, 0); //- -- ADS1281 SYNC



    /* Construct file copy Task thread */
    Task_Params_init(&WriteSD_taskParams);
    WriteSD_taskParams.stackSize = WriteSD_TASKSTACKSIZE;
    WriteSD_taskParams.stack = &WriteSD_taskStack;
    WriteSD_taskParams.arg0 = 10;
    WriteSD_taskParams.priority = 5;
    Task_construct(&WriteSD_taskStruct, (Task_FuncPtr)task_WriteSD_Fxn, &WriteSD_taskParams, NULL);


    Task_Params_init(&Uart_taskParams);
    Uart_taskParams.stackSize = Uart_TASKSTACKSIZE;
    Uart_taskParams.stack = &Uart_taskStack;
    Uart_taskParams.arg0 = 10;
  //  Uart_taskParams.priority = 1;  //--  biger is first   range 1-15
    Task_construct(&Uart_taskStruct, (Task_FuncPtr)task_Uart_Fxn, &Uart_taskParams, NULL);


    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_OFF); //- RED SD too slow
    GPIO_write(Board_LED2, Board_LED_OFF); //- Green SD card

    //System_printf("Starting the FatSD Raw example\n");



    GPIO_write(Board_ADS1281_RESET, 1);   //- -- ADS1281 SYNC


    GPIO_setCallback(Board_ADS1281_DRDY, hwi_GetADS1281Data_Fxn);
    GPIO_setCallback(Board_SEASCAN_PPS, hwi_GetSEAScanPPS_Fxn);
    GPIO_setCallback(Board_GPS_PPS, hwi_GetGPS_PPS_Fxn);

    /* Enable interrupts */
    GPIO_enableInt(Board_ADS1281_DRDY);
    GPIO_enableInt(Board_SEASCAN_PPS);
    GPIO_enableInt(Board_GPS_PPS);



    /* Start BIOS */
    BIOS_start();

    return (0);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------


unsigned char ahex2bin (unsigned char MSB, unsigned char LSB)
{
    if (MSB > '9') MSB += 9;   // Convert MSB value to a contiguous range (0x30..0x?F)
    if (LSB > '9') LSB += 9;   // Convert LSB value to a contiguous range (0x30..0x?F)
    return (MSB <<4) | (LSB & 0x0F);   // Make a result byte using only low nibbles of MSB and LSB
}
