/*
 * File:   main.c
 * Author: pc
 *
 * Created on 26 mai 2024, 09:20
 */


#include <xc.h>
#include "config.h"
#define _XTAL_FREQ 4000000 // Fosc


uint16_t AN0RES=0;
float Temperature, Voltage;
char* TempSTR[25];

void ADC_Init(void);
void I2C_Slave_Init(uint8_t );
uint16_t ADC_Read(uint8_t ANC);


void main(void) {
    ADC_Init();
    I2C_Slave_Init(0x40);//initialisation de le slave par une addresse de 0x31
    PORTB=0;
    while(1){
    //read the ADC
    AN0RES = ADC_Read(0); // Read Analog Channel 0
    // Calculate The Temperature
    Voltage = (float)(AN0RES * 0.0048828);
    Temperature = Voltage / 0.01;   
    //__delay_ms(10);
    }




}
////////////////////////////////////////////////////////////
/*******************************************/
/********************************************/
//--------[ ADC Routines ]---------
void ADC_Init()
{
  ADCON0 = 0x41; // Turn ADC ON, Select AN0 Channel, ADC Clock = Fosc/8
  ADCON1 = 0x80; // All 8 Channels Are Analog, Result is "Right-Justified"
  // ADC Clock = Fosc/8
}
uint16_t ADC_Read(uint8_t ANC)
{
  if(ANC<0 || ANC>7) // Check Channel Number Validity
  { return 0;}
  ADCON0 &= 0x11000101; // Clear The Channel Selection Bits
  ADCON0 |= ANC<<3; // Select The Required Channel (ANC)
  // Wait The Aquisition Time
  __delay_us(30); // The Minimum Tacq = 20us, So That should be enough
  GO_DONE = 1; // Start A/D Conversion
  while(ADCON0bits.GO_DONE); // Polling GO_DONE Bit
  // Provides Delay Until Conversion Is Complete
  return ((ADRESH << 8) + ADRESL); // Return The Right-Justified 10-Bit Result
}
void I2C_Slave_Init(uint8_t Address)
{
  //---[ Configures The I2C In Slave Mode]---
  SSPADD = Address;  // Set I2C Device Address
  SSPSTAT = 0x80; // Disable Slew Rate Control (Standard Mode)
  SSPCON = 0x36; // Select & Enable I2C (Slave Mode)
  SSPCON2 = 0x01; // Enable Clock Stretching
  TRISCbits.TRISC3 = 1;     // Set As Input - SDA
  TRISCbits.TRISC4 = 1;     // Set As Input - SCL
  PIR1bits.SSPIF = 0;      // Enbable Interrupts
  PIE1bits.SSPIE = 1;
  INTCONbits.PEIE = 1;
  INTCONbits.GIE = 1;
}

  void __interrupt() ISR(void)
{
  if(SSPIF)  
  {
    if(!(SSPSTATbits.D_nA) && (SSPSTATbits.R_nW))
    {
        PORTBbits.RB0=1;
      char Dummy = SSPBUF;/*Clear the buffer*/
      SSPBUF = (uint8_t)Temperature ;
      SSPCONbits.CKP = 1;
      while(SSPSTATbits.BF);
    }  
    SSPCONbits.CKP = 1;
    PIR1bits.SSPIF = 0;
  }
}





