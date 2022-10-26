#include <stdlib.h>
#include <string.h>
#include "LoraApp.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "GPS.h"
#include "HardwareInit.h"
#include "fatfs.h"

char        LoRaRxBuff[LRBuffSize];
uint8_t     LoRaTxBuff[LoRaTxBufferSize];
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef dma2;
ModuleStatus LoRaStatus = Normal;
LoRaModuleConf modulecfg;
extern SDStatus sdstat;
volatile uint8_t data_length;
extern GPS_t GPS;
LoraRx loraRx;


void LORA_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	//Stop this DMA transmission
    HAL_UART_DMAStop(huart);         
   
    //Calculate the length of the received data
    data_length  = LRBuffSize - __HAL_DMA_GET_COUNTER(&dma2); 

    if(LoRaStatus == Normal){
        LoRaRxBuff[data_length+1] = 0x00;
      //  HAL_UART_Transmit(&huart1,(uint8_t*) LoRaRxBuff,(uint16_t)data_length,0x700);  
        DecodeLoraFrame();
        SaveLoraFrame();
    }
    if(LoRaStatus == Program){
        modulecfg.addr = (uint16_t) ( (LoRaRxBuff[1] << 8) + LoRaRxBuff[2]);
        modulecfg.conf1 = LoRaRxBuff[3];
        modulecfg.chan = LoRaRxBuff[4];
        modulecfg.conf2 = LoRaRxBuff[5];
        myprintf("Adr = %d,Ch %d \n\r", modulecfg.addr, modulecfg.chan);
        LoRaStatus = Normal;
    }
    
    __LoRaLED_Toogle();
    data_length = 0;
    //Restart to start DMA transmission of LRBuffSize bytes of data at a time
    HAL_UART_Receive_DMA(huart, (uint8_t*)LoRaRxBuff, LRBuffSize);                    
}

uint8_t genByte(void){

    uint8_t res;
        res = rand()%255;
    return res;
}

void genPayload(void){

    uint8_t length;
    txbufferLoRa = NULL;
    length = rand()%112;
   // txbufferLoRa = (uint8_t*) malloc(length);
    if(txbufferLoRa != NULL){
        for(uint8_t i=2; i<length;i++)
            txbufferLoRa[i] = genByte();
    }
}

void sendFrameLoRa(void){

    static uint16_t counter;
    uint8_t tmp;
    genPayload();
    tmp = (uint8_t) counter >> 8;
    txbufferLoRa[0] = tmp;
    tmp = (uint8_t) counter;
    txbufferLoRa[1] = tmp;
    if(txbufferLoRa != NULL){
        HAL_UART_Transmit(&huart3, txbufferLoRa, sizeof(txbufferLoRa), 0x700);     
        counter++;
 //       free(txbufferLoRa);
    }
}
void  LoRa_Init(void){
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE); 
    HAL_UART_Receive_DMA(&huart3, (uint8_t*)LoRaRxBuff, (uint16_t)LRBuffSize); 
}

void GetLoRaConfig(void){

uint8_t cmd[] = {0xc1, 0xc1, 0xc1};
    M0_HIGH();
    M1_HIGH();
    LoRaStatus = Program;
    HAL_Delay(1);
    HAL_UART_Transmit(&huart3, cmd, 3 , 0x700);     
    HAL_Delay(1);
    M0_LOW();
    M1_LOW();
}

void SetLoRaConfig(void){

uint8_t conf[] = {0xc0, SRCADDR >> 8, SRCADDR, 0x1a, CHANNEL, 0xc4};
uint8_t cmd[] = {0xc1, 0xc1, 0xc1};

myprintf("Program begins\n\r");
M0_HIGH();
M1_HIGH();
LoRaStatus = Program;
HAL_Delay(10);
HAL_UART_Transmit(&huart3, conf, 6, 0x700);
HAL_Delay(10);
HAL_UART_Transmit(&huart3, cmd, 3, 0x700);
HAL_Delay(10);
//HAL_UART_Transmit(&huart3, cmd, 3, 0x700);
HAL_Delay(10);
M0_LOW();
M1_LOW();
myprintf("Program end\n\r");
}

void send_string(char* s){
	HAL_UART_Transmit(&huart1, (uint8_t*)s, strlen(s), 1000);
}

void MakeLoraFrame(void){

uint16_t dstaddr = DSTADDR;
uint16_t srcaddr = SRCADDR;
uint8_t  chan  = CHANNEL;
static uint16_t index;
uint8_t buffind = 2;
uint8_t sbuff[58];
uint8_t len;
LoRaTxBuff[0] =  (uint8_t) (dstaddr >> 8);
LoRaTxBuff[1] =  (uint8_t) (dstaddr);

memcpy(&LoRaTxBuff[buffind], &chan, 1);
buffind +=1; 
memcpy(&LoRaTxBuff[buffind], &srcaddr, 2);
buffind +=2; 
memcpy(&LoRaTxBuff[buffind], &dstaddr, 2);
buffind +=2; 
memcpy(&LoRaTxBuff[buffind], &index, 2);
buffind +=2; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.UTC_Year, 1);
buffind +=1; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.UTC_Mon, 1);
buffind +=1; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.UTC_Day, 1);
buffind +=1; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.UTC_Hour, 1);
buffind +=1; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.UTC_Min, 1);
buffind +=1; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.UTC_Sec, 1);
buffind +=1; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.Lat, 2);
buffind +=2; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.LatDec, 4);
buffind +=4; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.Lon, 2);
buffind +=2; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.LonDec, 4);
buffind +=4; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.MSL_Altitude, 4);
buffind +=4; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.MSL_AltitudeDecimal, 4);
buffind +=4; 
memcpy(&LoRaTxBuff[buffind], &GPS.GPGGA.SatellitesUsed, 1);
HAL_UART_Transmit(&huart3,(uint8_t*) LoRaTxBuff,LoRaTxBufferSize,0x700);       
index++; 

len = sprintf(sbuff,"ddss,%d,%d:%d:%d,%d.%d,%d.%d,%d.%d,%d\n\r", index, GPS.GPGGA.UTC_Hour, GPS.GPGGA.UTC_Min, GPS.GPGGA.UTC_Sec, GPS.GPGGA.Lat, GPS.GPGGA.LatDec, GPS.GPGGA.Lon, GPS.GPGGA.LonDec, GPS.GPGGA.MSL_Altitude, GPS.GPGGA.MSL_AltitudeDecimal, GPS.GPGGA.SatellitesUsed );
sbuff[0] = 0x00;
sbuff[1] = DSTADDR;
sbuff[2] = 0x00;
sbuff[3] = SRCADDR;
//HAL_UART_Transmit(&huart3,(uint8_t*) sbuff,len,0x700);       
}

void DecodeLoraFrame(void){

uint8_t buffind = 0;
 
memcpy(&loraRx.src,&LoRaRxBuff[buffind], 2);
buffind +=2;
memcpy(&loraRx.dst,&LoRaRxBuff[buffind], 2);
buffind +=2;
memcpy(&loraRx.index,&LoRaRxBuff[buffind], 2);
buffind +=2;
memcpy(&loraRx.UTC_Year,&LoRaRxBuff[buffind], 1);
buffind +=1;
memcpy(&loraRx.UTC_Mon,&LoRaRxBuff[buffind], 1);
buffind +=1;
memcpy(&loraRx.UTC_Day,&LoRaRxBuff[buffind], 1);
buffind +=1;
memcpy(&loraRx.UTC_Hour,&LoRaRxBuff[buffind], 1);
buffind +=1;
memcpy(&loraRx.UTC_Min,&LoRaRxBuff[buffind], 1);
buffind +=1;
memcpy(&loraRx.UTC_Sec,&LoRaRxBuff[buffind], 1);
buffind +=1;
memcpy(&loraRx.Latitude,&LoRaRxBuff[buffind], 2);
buffind +=2;
memcpy(&loraRx.LatitudeDecimal,&LoRaRxBuff[buffind], 4);
buffind +=4;
memcpy(&loraRx.Longitude,&LoRaRxBuff[buffind], 2);
buffind +=2;
memcpy(&loraRx.LongitudeDecimal,&LoRaRxBuff[buffind], 4);
buffind +=4;
memcpy(&loraRx.MSL_Altitude,&LoRaRxBuff[buffind], 4);
buffind +=4;
memcpy(&loraRx.MSL_AltitudeDecimal,&LoRaRxBuff[buffind], 4);
buffind +=4;
memcpy(&loraRx.SatellitesUsed,&LoRaRxBuff[buffind], 1);

memset(LoRaRxBuff,0, LRBuffSize);

}
void SaveLoraFrame(void){

static FIL fil; 		
FRESULT fres; 		
UINT bytesWrote,len;
static uint8_t cnt;
static uint8_t postfix;
char filename[30];   
int8_t out[256];

sprintf(filename,"LoRa-%d.txt", postfix);
cnt = (cnt + 1) % 100;
if (cnt == 0)
 	postfix++;
sprintf(out,"%d,%d,%d,20%d-%d-%dT%d:%d:%d,%d.%d,%d.%d,%d.%d,%d\n", loraRx.src,  loraRx.dst, loraRx.index,  loraRx.UTC_Year,  loraRx.UTC_Mon, loraRx.UTC_Day, loraRx.UTC_Hour,   loraRx.UTC_Min,  loraRx.UTC_Sec,  loraRx.Latitude,  loraRx.LatitudeDecimal,  loraRx.Longitude,  loraRx.LongitudeDecimal,  loraRx.MSL_Altitude,  loraRx.MSL_AltitudeDecimal,  loraRx.SatellitesUsed ); 
myprintf("LoRa: %s", out);  
if(out != NULL){
     len = strlen(out);
    out[len] = 0x00;
    if ((sdstat != 0) && (loraRx.SatellitesUsed)){
        __disable_irq();
        fres = f_open(&fil, filename, FA_WRITE | FA_OPEN_ALWAYS );
        if(fres == FR_OK) {
            if(DEBUG)
                myprintf("I was able to open file for writing\r\n");
        } 
        else {
            if(DEBUG)
                myprintf("f_open error (%i)\r\n", fres);
        }
        
        fres = f_lseek(&fil, f_size(&fil)) ;
        if(fres != FR_OK) {
            if(DEBUG)
                myprintf("Fseek failed!\r\n");
        } 
        
        fres = f_write(&fil, out, len, &bytesWrote);
        //f_printf(&fil, "%s", out);
        if(fres == FR_OK) {
            if(DEBUG)
                myprintf("Wrote %i bytes to file \r\n", bytesWrote);
        } 
        else {
            if(DEBUG)
                myprintf("f_write error (%i)\r\n");
        }
        f_close(&fil);
        __enable_irq();
        }
    }		
}