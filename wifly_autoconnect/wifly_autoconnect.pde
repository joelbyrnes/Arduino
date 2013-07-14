/*
stolen from http://burt.googlecode.com/svn/trunk/Arduino%20Sketchbook/WiFly_Auto_Connect/wifly_autoconnect.pde

 * WiFly Autoconnect Example
 * Copyright (c) 2010 SparkFun Electronics.  All right reserved.
 * Written by Chris Taylor
 *
 * This code was written to demonstrate the WiFly Shield from SparkFun Electronics
 * 
 * This code will initialize and test the SC16IS750 UART-SPI bridge, and automatically
 * connect to a WiFi network using the parameters given in the global variables.
 *
 * http://www.sparkfun.com
 */

#include <string.h>

// SC16IS750 Register definitions
#define THR        0x00 << 3
#define RHR        0x00 << 3
#define IER        0x01 << 3
#define FCR        0x02 << 3
#define IIR        0x02 << 3
#define LCR        0x03 << 3
#define MCR        0x04 << 3
#define LSR        0x05 << 3
#define MSR        0x06 << 3
#define SPR        0x07 << 3
#define TXFIFO     0x08 << 3
#define RXFIFO     0x09 << 3
#define DLAB       0x80 << 3
#define IODIR      0x0A << 3
#define IOSTATE    0x0B << 3
#define IOINTMSK   0x0C << 3
#define IOCTRL     0x0E << 3
#define EFCR       0x0F << 3

#define DLL        0x00 << 3
#define DLM        0x01 << 3
#define EFR        0x02 << 3
#define XON1       0x04 << 3  
#define XON2       0x05 << 3
#define XOFF1      0x06 << 3
#define XOFF2      0x07 << 3

// SPI Pin definitions
#define CS         10
#define MOSI       11
#define MISO       12
#define SCK        13

#define ASSOCIATE_TIMEOUT 5000

// Global variables
char incoming_data; 
char TX_Fifo_Address = THR; 

int i = 0;
int j = 0;
int k = 0;
char clr = 0;
char polling = 0;

// SC16IS750 register values
struct SPI_UART_cfg
{
  char DivL,DivM,DataFormat,Flow;
};

struct SPI_UART_cfg SPI_Uart_config = {
  0x50,0x00,0x03,0x10};

// Wifi parameters
char auth_level[] = "3";
char auth_phrase[] = "examplepassword";
char port_listen[] = "80";
char ssid[] = "SparkFunWireless";

void setup()
{
  // Initialize SPI pins
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK,OUTPUT);
  pinMode(CS,OUTPUT);
  digitalWrite(CS,HIGH); //disable device 

  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
  clr=SPSR;
  clr=SPDR;
  delay(10); 

  Serial.begin(9600);
  Serial.println("\n\r\n\rWiFly Shield Terminal Routine");
  // Initialize and test SC16IS750
  if(SPI_Uart_Init()){ 
    Serial.println("Bridge initialized successfully!"); 
  }
  else{ 
    Serial.println("Could not initialize bridge, locking up.\n\r"); 
    while(1); 
  }
  autoconnect();
}

void loop()
{
  // Terminal routine
  if(SPI_Uart_ReadByte(LSR) & 0x01) // Incoming data
  { 
    polling = 1;
    i = 0;
    while(polling)
    {
      if((SPI_Uart_ReadByte(LSR) & 0x01))
      {
        incoming_data = SPI_Uart_ReadByte(RHR);
        Serial.print(incoming_data,BYTE);
        //Serial.print(incoming_data, HEX);
        //Serial.print(" ");
      }  
      else
      {
        polling = 0;
      }
    }

  }
  else if(Serial.available()) // Outgoing data
  {
    incoming_data = Serial.read();
    select();
    spi_transfer(0x00); // Transmit command
    spi_transfer(incoming_data);
    deselect();
  }

}

void select(void)
{
  digitalWrite(CS,LOW);
}

void deselect(void)
{
  digitalWrite(CS,HIGH);
}

char SPI_Uart_Init(void) // Initialize and test SC16IS750
{
  char data = 0;

  SPI_Uart_WriteByte(LCR,0x80); // 0x80 to program baudrate
  SPI_Uart_WriteByte(DLL,SPI_Uart_config.DivL); //0x50 = 9600 with Xtal = 12.288MHz
  SPI_Uart_WriteByte(DLM,SPI_Uart_config.DivM); 

  SPI_Uart_WriteByte(LCR, 0xBF); // access EFR register
  SPI_Uart_WriteByte(EFR, SPI_Uart_config.Flow); // enable enhanced registers
  SPI_Uart_WriteByte(LCR, SPI_Uart_config.DataFormat); // 8 data bit, 1 stop bit, no parity
  SPI_Uart_WriteByte(FCR, 0x06); // reset TXFIFO, reset RXFIFO, non FIFO mode
  SPI_Uart_WriteByte(FCR, 0x01); // enable FIFO mode

  // Perform read/write test to check if UART is working
  SPI_Uart_WriteByte(SPR,'H');
  data = SPI_Uart_ReadByte(SPR);

  if(data == 'H'){ 
    return 1; 
  }
  else{ 
    return 0; 
  }

}

void SPI_Uart_WriteByte(char address, char data) 
// Write <data> byte to SC16IS750 register <address>
{
  long int length;
  char senddata[2];
  senddata[0] = address;
  senddata[1] = data;

  select();
  length = SPI_Write(senddata, 2);
  deselect();
}

long int SPI_Write(char* srcptr, long int length)
// Write string to SC16IS750
{
  for(long int i = 0; i < length; i++)
  {
    spi_transfer(srcptr[i]);
  }
  return length; 
}

void SPI_Uart_WriteArray(char *data, long int NumBytes)
// Write string to SC16IS750 THR
{
  long int length;
  select();
  length = SPI_Write(&TX_Fifo_Address,1);

  while(NumBytes > 16)
  {
    length = SPI_Write(data,16);
    NumBytes -= 16;
    data += 16;
  }
  length = SPI_Write(data,NumBytes);

  deselect();
}

char SPI_Uart_ReadByte(char address)
// Read byte from SC16IS750 register at <address>
{
  char data;

  address = (address | 0x80);

  select();
  spi_transfer(address);
  data = spi_transfer(0xFF);
  deselect();
  return data;  
}

char autoconnect(void)
{
  // Exit command mode if we haven't already
  SPI_Uart_println("");  
  SPI_Uart_println("exit");
  delay(500);

  // Enter command mode 
  SPI_Uart_print("$$$");
  delay(500);

  // Reboot to get device into known state
  Serial.println("Rebooting");
  SPI_Uart_println("reboot");
  delay(3000);

  // Enter command mode
  Serial.println("Entering command mode.");
  SPI_Uart_print("$$$");
  delay(500);

  // Set authorization level to <auth_level>
  SPI_Uart_print("set w a ");
  SPI_Uart_println(auth_level); 
  delay(500);
  Serial.print("Set wlan to authorization level ");
  Serial.println(auth_level);

  // Set passphrase to <auth_phrase>
  SPI_Uart_print("set w p ");
  SPI_Uart_println(auth_phrase);
  delay(500);
  Serial.print("Set security phrase to ");
  Serial.println(auth_phrase);

  // Set localport to <port_listen>
  SPI_Uart_print("set i l ");
  SPI_Uart_println(port_listen);
  delay(500);
  Serial.print("Set IP localport to ");
  Serial.println(port_listen);

  // Join wifi network <ssid>
  Serial.print("Joining '");
  Serial.print(ssid);
  Serial.println("'\n\r");
  delay(100);
  Flush_RX();
  delay(100);
  SPI_Uart_print("join ");
  SPI_Uart_println(ssid);  
  delay(ASSOCIATE_TIMEOUT);
  Flush_RX();

  // Check if we're associated
  SPI_Uart_println("show c");
  if(Wait_On_Response_Char(13) != '0')
  {
    Serial.print("Failed to associate with '");
    Serial.print(ssid);
    Serial.println("'\n\rRetrying...");
    Flush_RX();
    autoconnect();
  }
  else
  {
    Serial.println("Associated!");
    Flush_RX();
  }
}

void Flush_RX(void)
// Flush characters from SC16IS750
{
  int j = 0;
  while(j < 1000)
  {
    if((SPI_Uart_ReadByte(LSR) & 0x01))
    {
      incoming_data = SPI_Uart_ReadByte(RHR);
    }  
    else
    {
      j++;
    }
  }
}

char Wait_On_Response_Char(char num)
// Wait for incoming char number <num> and return it
{
  i = 1;
  while(1)
  {
    if((SPI_Uart_ReadByte(LSR) & 0x01))
    {
      incoming_data = SPI_Uart_ReadByte(RHR);
      //Serial.print(incoming_data, BYTE);
      if(i == num){ 
        return incoming_data; 
      }
      else{ 
        i++; 
      }
    }  
  }
}

void SPI_Uart_println(char *data)
// Write array to SC16IS750 followed by a carriage return
{
  SPI_Uart_WriteArray(data,strlen(data));
  SPI_Uart_WriteByte(THR, 0x0d);
}

void SPI_Uart_print(char *data)
// Routine to write array to SC16IS750 using strlen instead of hardcoded length
{
  SPI_Uart_WriteArray(data,strlen(data));
}

char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}




