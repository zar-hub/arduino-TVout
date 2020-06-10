#include "TvLib.h"

TVClass TV;

void (*line_hook)();
unsigned int line;
volatile uint8_t updateStuff;
//////////////////////////////TVclass methods////////////////////////////////
TVClass::TVClass(){
  //reset line
  line = 0;
  //reset update
  updateStuff = 0;
  //reset render
  setRender(empty);
  //reset frameHook
  setFrameHook(empty);
}

inline void TVClass::line_handler()
{ 
  //line++; //test
  switch(line)
  {
   
    case TOTAL_LINES:
      line = 0;
      vsync_pulse();
      break;
      
    case 5:
    case 314:
      equal_pulse();
      break;

    case 10:
      scan_line();

    //uncomment those to have user friendly controls over render setups
    //is more difficult to mess up smth with those lines uncommented
      break; 

    case PAL_START_RENDER:
       
      line_hook = render_line;
      break;

    case PAL_END_RENDER:
      line_hook = &empty;
      updateStuff = 1;
      break;
  }
}

TVClass& TVClass::start() 
{
  // reset Timer Counter Control Register
  TCCR1A = 0;
  TCCR1B = 0;
 
  DDRB |= _BV(SYN_PIN); // set sync pin as output

  PORT_SYN |= _BV(SYN_PIN); // set sync high

  // fast pwm, TOP = OCR1A, invert output on OCR1B
  TCCR1A = 0b00110011;
  TCCR1B = 0b00011001;
  
  // interrupt on overflow
  TIMSK1 = _BV(TOIE1);

  // draw the first pulse
  vsync_pulse();
  line_hook = &empty;
  
  // activate global interrupts
  sei();
  return *this;
}

TVClass& TVClass::setRender(void (foo)()){
  render_line = foo;
  return *this;
}

TVClass& TVClass::setFrameHook(void (foo)()){
  frameHook = foo;
  return *this;
}

TVClass& TVClass::startUSART(){
  // USART MSPIM
  UBRR0 = 0; // must be 0 for initializing
  DDRD |= _BV(4); // set XCK as output
  //UCSR0A = _BV (TXC0);  // any old transmit now complete
  //MSPIM_PORT |= _BV(MSPIM_PIN);
  UCSR0C = _BV (UMSEL00) | _BV (UMSEL01);  // Master SPI mode
  UCSR0B = _BV (TXEN0);  // transmit enable
  UBRR0 = BAUD_RATE;
}

////////////////////////////////// interrupt service routine
ISR(TIMER1_OVF_vect)
{
  line++; // this instruction needs to be exactly here
  // this MUST NOT BE TOUCHED, it messes with timing
  line_hook();   
  TV.line_handler(); // spostare
}
