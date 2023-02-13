// TTL to USB : https://www.robolinkmarket.com/usb-rs485-donusturucu-modul
// RS485 to TTL : https://www.robolinkmarket.com/rs485-ttl-seri-donusturucu-karti
// Source: https://github.com/furkanbakkal

//DE and RE pins shorted ->D2
//DI -> TX
//RO -> RX

//A->A  
//B->B

/*Supported functions:
3 : Read holding register
6 : Write single register
15: Write multiple registers
*/

/* LCD Pins
LCD_DB4   PIN_B4
LCD_DB5   PIN_B5
LCD_DB6   PIN_B6
LCD_DB7   PIN_B7
LCD_RS    PIN_B3
LCD_RW    PIN_B2
LCD_E     PIN_B1
*/

#include <main.h>

#define MODBUS_BUS SERIAL
#define MODBUS_TYPE MODBUS_TYPE_MASTER //PIC is master, PC is slave
#define MODBUS_SERIAL_TYPE MODBUS_RTU     //use MODBUS_ASCII for ASCII mode
#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA //Default TX RX pins 
#define MODBUS_SERIAL_RX_ENABLE PIN_D2   // Controls RE pin for RS485
#define MODBUS_SERIAL_BAUD 9600 //baudrate
#define MODBUS_SLAVE_ADDRESS 0x05 //slave adress (default 5)

#define LED PIN_C4 //output pin (if address-reg 0 is "1" this pin will be ON, if address-reg 0 is "0" this pin will be OFF)

#include"modbus.c"
#include "flex_lcd420.c" //using 4x20 LCD

unsigned int16 read_reg[3]; //readed reg values (default 3 reg)
int8 d=0;

void read_all_holding(int16 start, int16 quantity) //read holding registers (start adress, quantity)
{
   output_high(MODBUS_SERIAL_RX_ENABLE);
   
   modbus_read_holding_registers(MODBUS_SLAVE_ADDRESS,start,quantity);
   
   int count=0;
     
   for(int i=1; i < (modbus_rx.len); i=i+2){ 
      
      unsigned int16 val;
      val=make16( modbus_rx.data[i], modbus_rx.data[i+1]);
      read_reg[count]=val;
      count++;
   }
   
   output_low(MODBUS_SERIAL_RX_ENABLE);
}


void write_reg(int16 address, unsigned int16 data) //write one register  (adress,data)
{
   output_high(MODBUS_SERIAL_RX_ENABLE);
   
   modbus_write_single_register(MODBUS_SLAVE_ADDRESS,address,data);
   
   int count=0;
     
   for(int i=1; i < (modbus_rx.len); i=i+2){ 
      
      unsigned int16 val;
      val=make16( modbus_rx.data[i], modbus_rx.data[i+1]);
      read_reg[count]=val;
      count++;
   }
   
   output_low(MODBUS_SERIAL_RX_ENABLE);
}


void write_multiple_reg(int16 start, int16 quantity, unsigned int16 reg_array) //write multiple registers  (start adress, quantity, data as array)
{
   
   output_high(MODBUS_SERIAL_RX_ENABLE);
   
   modbus_write_multiple_registers(MODBUS_SLAVE_ADDRESS,start,quantity,reg_array);
   
   output_low(MODBUS_SERIAL_RX_ENABLE);
   
}


void main()
{

   setup_adc_ports(NO_ANALOGS);
   
   modbus_init();
   lcd_init();
   delay_ms(100);
   
   lcd_gotoxy(1,1);
   printf(lcd_putc,"System Started!");

   
 while(TRUE){

   //unsigned int16 reg_array[2] = {1, 22};
   //write_multiple_reg(1,2,reg_array);
   
   write_reg(1,d);
   read_all_holding(0,3);
   
   int c=1;
   for(int i=0; i<3;i++){ //lcd print
      lcd_gotoxy(1,c+1);
      printf(lcd_putc,"%lu      ",read_reg[i]);
      c++;
   }
   
   if(read_reg[0]==1){ //if adress-reg 0 is 1 LED on
      output_high(LED);
   }
   
   if(read_reg[0]==0){ //if adress-reg 0 is 0 LED off
      output_low(LED);
   }
   
   d++;
   delay_ms(100);
   
   
 }
}

