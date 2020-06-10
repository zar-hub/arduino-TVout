#include "SPI.h"

SPIClass SPI;

void SPIClass::begin()
{
  // Set SS to high so a connected chip will be "deselected" by default
  digitalWrite(SS, HIGH);

  // When the SS pin is set as OUTPUT, it can be used as
  // a general purpose output port (it doesn't influence
  // SPI operations).
  pinMode(SS, OUTPUT);

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
  // automatically switches to Slave, so the data direction of
  // the SS pin MUST be kept as OUTPUT.
  SPCR |= _BV(MSTR);
  SPCR |= _BV(SPE);

  // Set direction register for SCK and MOSI pin.
  // MISO pin automatically overrides to INPUT.
  // By doing this AFTER enabling SPI, we avoid accidentally
  // clocking in a single bit since the lines go directly
  // from "input" to SPI control.
  // http://code.google.com/p/arduino/issues/detail?id=888
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);

  // f = fosc / 2
  SPSR |= _BV(SPI2X);
  SPCR &= ~(_BV(SPR0) & _BV(SPR1));
  //SPCR |= (_BV(SPR0) | _BV(SPR1));
}

void SPIClass::end() {
  SPCR &= ~_BV(SPE);
}

void SPIClass::transfer(const void * buf, void * retbuf, uint32_t count) {
  if (count == 0) return;

  const uint8_t *p = (const uint8_t *)buf;
  uint8_t *pret = (uint8_t *)retbuf;
  uint8_t in;

  uint8_t out = p ? *p++ : 0;
  SPDR = out;
  while (--count > 0) {
    if (p) {
      out = *p++;
    }
    while (!(SPSR & _BV(SPIF))) ;
    in = SPDR;
    SPDR = out;
    if (pret)*pret++ = in;
  }
  while (!(SPSR & _BV(SPIF))) ;
  in = SPDR;
  if (pret)*pret = in;
}

static void SPIClass::setClkDivider(uint8_t foscDivider)
{
  if(foscDivider == 2)
    {
      SPCR &= ~(_BV(SPR1) | _BV(SPR0));
      SPSR |= _BV(SPI2X);
    }
  else if(foscDivider == 4)
    {
      SPCR &= ~(_BV(SPR1) | _BV(SPR0));
      SPSR &= ~_BV(SPI2X);
    }
  else if(foscDivider == 16)
    {
      SPCR &= ~_BV(SPR1);
      SPCR |= _BV(SPR0);
      SPSR &= ~_BV(SPI2X);
    }
  else if(foscDivider == 64)
    {
      SPCR &= ~_BV(SPR0);
      SPCR |= _BV(SPR1);
      SPSR &= ~_BV(SPI2X);
    }
  else if(foscDivider == 128)
    {
      SPCR |= (_BV(SPR1) | _BV(SPR0));
      SPSR &= ~_BV(SPI2X);
    }
}
