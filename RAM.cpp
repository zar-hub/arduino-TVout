#include "RAM.h"

RAMClass RAM;

////////////////////METHODS////////////////
RAMClass::RAMClass()
{
    SPI.begin();
    attachedSlaves = 0;
}

void RAMClass::add(uint8_t SS, uint8_t MODE){
  if(attachedSlaves == (SUPPORTED_SLAVES - 1))
    return;
  
  // sets SS as output and gets the ram to sleep
  DDR_RAM |= _BV(SS);
  PORT_RAM |= _BV(SS);
  writeSr(SS, MODE);

  // save slave data
  slaveInfo[attachedSlaves].SS = SS;
  slaveInfo[attachedSlaves].HOLD = NONE;
  slaveInfo[attachedSlaves].MODE = MODE;
  attachedSlaves++;
}
void RAMClass::add(uint8_t SS, uint8_t HOLD, uint8_t MODE){
  if(attachedSlaves == (SUPPORTED_SLAVES - 1))
    return;
  
  // sets pins as output and gets the ram to sleep
  DDR_RAM |= _BV(SS);
  PORT_RAM |= _BV(SS); 
  DDR_RAM |= _BV(HOLD);
  PORT_RAM |= _BV(HOLD);
  writeSr(SS, MODE);

  // save slave data
  slaveInfo[attachedSlaves].SS = SS;
  slaveInfo[attachedSlaves].HOLD = HOLD;
  slaveInfo[attachedSlaves].MODE = MODE;
  attachedSlaves++;
}




// transmition funtions
 uint8_t RAMClass::readSr(uint8_t SS){
    uint8_t sreg;
    // pull SS low to start transmittion
    PORT_RAM &= ~_BV(SS);

    //__asm__ __volatile__ ("delayTransistor\n"); 
    SPI.transfer(RDSR);
    sreg = SPI.transfer(NONE);

    // signal end transmittion by pulling ss low
    PORT_RAM |= _BV(SS);
    return sreg;
}

void RAMClass::writeSr(uint8_t SS, uint8_t MODE){
    // pull SS low to start transmittion
    PORT_RAM &= ~_BV(SS);

    //__asm__ __volatile__ ("delayTransistor\n"); 
    SPI.transfer(WRSR);
    SPI.transfer(MODE);

    // signal end transmittion by pulling ss low
    PORT_RAM |= _BV(SS);
}
