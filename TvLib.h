#ifndef TVLIB_H_INCLUDED
#define TVLIB_H_INCLUDED

#include <avr/interrupt.h>
#include <avr/io.h>

// GPIO
#define SYN_PIN 2 // pin 10, timer 1
#define PORT_VID PORTD
#define PORT_SYN PORTB 

// PAL
#define PAL_32 511
#define PAL_64 1023
#define PAL_OUTPUT_START 177

#define PAL_HSYNC 74
#define PAL_VSYNC_START 433
#define PAL_EQUAL_START 36

#define PAL_LINES 312
#define SYNC_LINES 8
#define TOTAL_LINES PAL_LINES + SYNC_LINES
// can be used to center the image, default val:30 minVal:10
#define PAL_START_RENDER 30
#define PAL_END_DRAW PAL_START_RENDER - 1
#define PAL_END_RENDER 310

// PIXEL TIMING
#define BAUD_RATE 0
#define ACTIVE_VIDEO_CYCLES PAL_SCREEN_X / 8

// SCREEN not updated
#define PAL_SCREEN_Y PAL_END_RENDER - PAL_START_RENDER //+1 maybe
#define PAL_SCREEN_X 384

// SCREEN UPDATED
#define BYTE_PER_LINE 64

extern unsigned int line;
extern volatile uint8_t updateStuff;
////////////////////// TIMING FUNCTION ///////////////////
static void inline wait_until(uint8_t time) {
  __asm__ __volatile__ (
      "subi %[time], 10\n"
      "sub  %[time], %[tcnt1l]\n\t"
    "100:\n\t"
      "subi %[time], 3\n\t"
      "brcc 100b\n\t"
      "subi %[time], 0-3\n\t"
      "breq 101f\n\t"
      "dec  %[time]\n\t"
      "breq 102f\n\t"
      "rjmp 102f\n"
    "101:\n\t"
      "nop\n" 
    "102:\n"
    :
    : [time] "a" (time),
    [tcnt1l] "a" (TCNT1L)
  );
}

//////////////////LINE TYPES//////////////////////////////
// this function uses USART (TX pin) to produce output
inline void USART_render_line()
{
  wait_until(PAL_OUTPUT_START);
  UCSR0B = _BV (TXEN0);
  UCSR0A = _BV(TXC0); // clear flag
  
  //send data
  for(int i = 0; i < ACTIVE_VIDEO_CYCLES; i++)
  {
    while ( !( UCSR0A & (1 << UDRE0)) ); // wait for new data
      UDR0 = 0b11000000;
  }
  
  UCSR0B = 0;
}

// all these funtions are used to create the standard signal, they are not actual render lines.
// they are the "background signal"
inline void vsync_pulse()
{
  OCR1A = PAL_32;
  OCR1B = PAL_VSYNC_START;
}

inline void equal_pulse()
{
  OCR1A = PAL_32;
  OCR1B = PAL_EQUAL_START;
}

inline void scan_line()
{
  OCR1A = PAL_64;
  OCR1B = PAL_HSYNC;
}

inline void empty()
{
  return;
}
//////////////////END LINE TYPES//////////////////////////////

//////////////////////////THE CLASS/////////////////////////
class TVClass{
    void (*render_line)();
    void (*frameHook)();
    
  public:
    TVClass();
    TVClass& start();
    TVClass& setRender(void (foo)());
    TVClass& setFrameHook(void (foo)());
    inline void line_handler();
    TVClass& startUSART();
  
};

////////////////VISIBILITY//////////////////
extern void (*line_hook)();
extern TVClass TV;

#endif
