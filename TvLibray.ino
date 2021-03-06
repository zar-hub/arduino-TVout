// the first 2 bytes of every row are off screen, design choice to implement screen shake
// PAL_OUTPUT_START can be used fos x axis shake
// 51 bytes every row

#include "RAM.h"
#include "TvLib.h"
#include "spec//asm_macros.h"


#define beginTestClock TCCR0B |=  0b00000001; // DO NOT TOUCH
#define endTestClock TCCR0B &= 0b11111000;
#define SS_PORT PORTD
#define s0 SS_PORT &= 0b11111100;  SS_PORT |= 0b00000000;
#define snone SS_PORT |= 0b00000011;
  
void test()
{
  
  if(line == PAL_START_RENDER + 1)
  {
    //start pixel clock
    snone
    wait_until(177);
    DDRD |= _BV(6);
    beginTestClock
  }
  if(line == PAL_END_RENDER)
  {
    
    //endPixelClock
    DDRD &= ~_BV(6);      
    // signal end transmittion by pulling ss low
    PORT_RAM |= _BV(7);
    s0
  }
}


void test01()
{
  uint8_t buff[14];
  wait_until(PAL_OUTPUT_START);
  RAM.read(7, buff, 14, 50);
}


void setup() 
{
  TV.start(); // must be at the beginning
  
  DDRD |= (_BV(0) | _BV(1));
  
  s0
 
  // reset register
  // DO NOT TOUCH THIS STUPID MORON!
  // THIS ISN'T FOR LINE TESTING BITCH!
  TCCR0A = 0b01000011;
  TCCR0B = 0b00001000;
  OCR0A = 0;
  //Serial.begin(9600);
  
  RAM.add(7, BYTE_MODE);

  RAM.add(5, BYTE_MODE);
  uint8_t data;

  for(unsigned int j = 0; j < 320; j++)
  {
   
    for(unsigned int i = 0; i < 64; i++)
        RAM.write(7, 0, 64*j + i);
  }


  //BORDERS
  //xDim is in byte, yDim is in lines
  // RECT, (left, top, width, height)
  //MAXBORDERS (1,0, 50 ,290)
  //VIEWPORT (2,1, 49 ,280)
  //ABSMAX(1,0,<60,notlimited)

  // the first line must be blank.
  // can be drawn only if the first bit is 0
  for(unsigned int j = 1; j < 300; j++)
  {
    switch(j % 8)
    {
      case 0:
      case 1:
      case 2:
      case 3:
        data = 0b01110000;
        break;
   
      case 4:
      case 5:
      case 6:
      case 7:
        data = 0b00001111;
        break;
    }
    for(unsigned int i = 0; i < 50; i++)
        RAM.write(7, data, 64*j + i);
  }
    
    
 // Serial.end()
  RAM.writeSr(7, SEQ_MODE);
  TV.setRender(test);
  
   
  
}

void loop() 
{
  if(updateStuff)
  {    // select slave by pulling SS low
  PORT_RAM &= ~_BV(7);
  __asm__ __volatile__ ("delay16\n"); 
  SPDR = READ;
  asm volatile("nop");
  while (!(SPSR & _BV(SPIF)));
  // give the addr MSB first
  SPDR = (uint8_t)(0);
  asm volatile("nop");
  while (!(SPSR & _BV(SPIF)));
   SPDR = (uint8_t)(0);
  asm volatile("nop");
  while (!(SPSR & _BV(SPIF)));
    updateStuff = 0;
  }
}
