/*
 nRF24L01 Interface

 created 14 November 2013
 by Jon Nethercott
 *
 * Modded for 18F252
 */

#include <xc.h>
#include <GenericTypeDefs.h>
#include "utils.h"
#include "spi.h"
#include "nRF24L01.h"

//Pins
#define triscsn TRISC2
#define trisce TRISC1
#define csnPin PORTCbits.RC2
#define cePin PORTCbits.RC1
//#define irqPin

//Data
// Configuración de dirección, canal, velocidad y potencia del módulo wireless
// van en nRF24L01.h (faltan veloc. y potencia)
//
//BYTE RXTX_ADDR[3] = { 0xB6, 0x24, 0xA6 };  //Randomly chosen address
//#define RF_CHANNEL  0x05

BOOL rfCardPresent = FALSE;

//Local Helper Function Prototypes
void FlushTXRX();
void WriteRegister(BYTE reg, BYTE val);
void WriteAddress(BYTE reg, BYTE num, BYTE* addr);
BYTE ReadRegister(BYTE reg);
BYTE ReadStatus();
void WriteCommand(BYTE command);
void WritePayload(BYTE num, BYTE* data);
void ReadPayload(BYTE num, BYTE* data);


void nRF_Setup()
{
  // start the SPI library:
  SPI_init();

  // initalize the  CSN and CE pins:
  triscsn = 0;
  trisce = 0;

  csnPin = 1;
  cePin = 0;

  WriteRegister(NRF_CONFIG, 0x00001010);    // 1 BYTE CRC, POWER UP, PTX
  WriteRegister(EN_AA, 0b00000000);         // Se deshabilita el auto ack de todos los pipes
  WriteRegister(EN_RXADDR, 0b00000001);     // Se habilita sólo pipe0
  WriteRegister(SETUP_AW, 0x01);            // 3 BYTE address
  WriteRegister(SETUP_RETR, 0x00);          // Retransmit disabled
  WriteRegister(RF_CH, RF_CHANNEL);         // Configuramos canal
  WriteRegister(RF_SETUP, 0b00000110);      // 1Mbps, 0dBm
  WriteRegister(RX_PW_P0, 1);               // RX payload = 1 BYTE

  WriteAddress(RX_ADDR_P0, 3, RXTX_ADDR);
  WriteAddress(TX_ADDR, 3, RXTX_ADDR);

  FlushTXRX();

  if ((ReadRegister(NRF_CONFIG) & 0x08) != 0)
      rfCardPresent = TRUE;
}

void RXMode()
{
  WriteRegister(NRF_CONFIG, 0x0B);         //1 BYTE CRC, POWER UP, PRX
  cePin = 1;
  //According to the datasheet we shouldn't bring CSN low within Tpece2csn
  //after setting ce high. Can't see why (or when that would happen though)
  //so comment out the next line.
  //__delay_us(4);    //Tpece2csn
}

void TXMode()
{
  cePin = 0;
  WriteRegister(NRF_CONFIG, 0x0A);         //1 BYTE CRC, POWER UP, PTX
}

void PowerDown()
{
  cePin = 0;
  WriteRegister(NRF_CONFIG, 0);
}

BYTE RXChar()
{
  BYTE data;
  ReadPayload(1, &data);
  //Clear status bit
  WriteRegister(NRF_STATUS, 0x40);
  return data;
}

void TXChar(BYTE ch)
{
  WritePayload(1, &ch);

  if (rfCardPresent)
  {
      //Wait for char to be sent
      BYTE stat;
      do
      {
          stat = ReadStatus();
      } while ((stat & 0x20) == 0);
  }

  //Clear status bit
  WriteRegister(NRF_STATUS, 0x20);
}

BOOL ReadDataAvailable()
{
  BYTE stat = ReadStatus();
  return (stat & 0x40) != 0;
}

void FlushTXRX()
{
  WriteRegister(NRF_STATUS, 0x70);    //Clear: data RX ready, data sent TX, Max TX retransmits
  WriteCommand(FLUSH_RX);
  WriteCommand(FLUSH_TX);
}

// *************** Helper Methods ***************

void WriteRegister(BYTE reg, BYTE val)
{
  csnPin = 0;
  SPI_transfer(W_REG | reg);
  SPI_transfer(val);
  csnPin = 1;
}

//Address is 3-5 bytes, LSB first
void WriteAddress(BYTE reg, BYTE num, BYTE* addr)
{
  csnPin = 0;
  SPI_transfer(W_REG | reg);
  for (BYTE i=0; i<num; i++)
    SPI_transfer(addr[i]);
  csnPin = 1;
}

BYTE ReadRegister(BYTE reg)
{
  csnPin = 0;
  SPI_transfer(R_REG | reg);
  BYTE val = SPI_transfer(0x00);
  csnPin = 1;
  return val;
}

BYTE ReadStatus()
{
  csnPin = 0;
  BYTE val = SPI_transfer(R_STATUS);
  csnPin = 1;
  return val;
}

void WriteCommand(BYTE command)
{
  csnPin = 0;
  SPI_transfer(command);
  csnPin = 1;
}

void WritePayload(BYTE num, BYTE* data)
{
  csnPin = 0;
  SPI_transfer(TX_PAYLOAD);
  for (BYTE i=0; i<num; i++)
    SPI_transfer(data[i]);
  csnPin = 1;

  cePin = 1;
  __delay_us(12);   //Thce (10us) + a bit (2us)
  cePin = 0;
}

void ReadPayload(BYTE num, BYTE* data)
{
  csnPin = 0;
  SPI_transfer(RX_PAYLOAD);
  for (BYTE i=0; i<num; i++)
    data[i] = SPI_transfer(0);
  csnPin = 1;
}


