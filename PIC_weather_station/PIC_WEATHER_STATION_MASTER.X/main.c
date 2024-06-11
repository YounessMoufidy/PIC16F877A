#include <xc.h>
#include "config.h"
#define _XTAL_FREQ 4000000 // Fosc
#define I2C_BaudRate 100000 // I2C Baud Rate = 100 Kbps
//----------------------------------------------------------
//-----------------[ Functions' Prototypes ]----------------
void I2C_Master_Init(void);
void I2C_Wait(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Restart(void);
void I2C_ACK(void);
void I2C_NACK(void);
unsigned char I2C_Read(void);
unsigned char I2C_Write(unsigned char Data);
//----------------------------------------------------------
void main()
{
  TRISB = 0x00;
  TRISD = 0x00;
  PORTD = 0x00;
  I2C_Master_Init();
  while(1)
  {
    I2C_Start(); // I2C Start Sequence
    //PORTBbits.RB0=1;
    I2C_Write(0x41); // I2C Slave Device Address + Write
    //PORTDbits.RD1=1;
    PORTB=I2C_Read();
   // PORTDbits.RD2=1;
    I2C_Stop(); // I2C Stop Sequence
    __delay_ms(500);
    RD3 = ~RD3; // Toggle LED Each Time A Byte Is received!
  }
}
//------------[ END OF MAIN ]--------------
//-----------------------------------------
void I2C_Master_Init()
{
  SSPCON = 0x28;
  SSPCON2 = 0x00;
  SSPSTAT = 0x00;
  SSPADD = ((_XTAL_FREQ/4)/I2C_BaudRate) - 1;
  TRISC3 = 1;
  TRISC4 = 1;
}
void I2C_Wait()
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}
void I2C_Start()
{
  //---[ Initiate I2C Start Condition Sequence ]---
  I2C_Wait();
  SSPCON2bits.SEN = 1;
}
void I2C_Stop()
{
  //---[ Initiate I2C Stop Condition Sequence ]---
  I2C_Wait();
  SSPCON2bits.PEN = 1;
}
void I2C_Restart()
{
  //---[ Initiate I2C Restart Condition Sequence ]---
  I2C_Wait();
  SSPCON2bits.RSEN = 1;
}
void I2C_ACK(void)
{
  //---[ Send ACK - For Master Receiver Mode ]---
  I2C_Wait();
  SSPCON2bits.ACKDT = 0; // 0 -> ACK, 1 -> NACK
  SSPCON2bits.ACKEN = 1; // Send ACK Signal!
}
void I2C_NACK(void)
{
  //---[ Send NACK - For Master Receiver Mode ]---
  I2C_Wait();
  SSPCON2bits.ACKDT = 1; // 1 -> NACK, 0 -> ACK
  SSPCON2bits.ACKEN = 1; // Send NACK Signal!
}
unsigned char I2C_Write(unsigned char Data)
{
  //---[ Send Byte, Return The ACK/NACK ]---
  //I2C_Wait();
  SSPBUF = Data;
  //I2C_Wait();
  return ACKSTAT;
}

unsigned char I2C_Read()
{
  unsigned char Data;
  I2C_Wait();
  SSPCON2bits.RCEN = 1;
  I2C_Wait();
  Data = SSPBUF;
  I2C_NACK();
  return Data;
}