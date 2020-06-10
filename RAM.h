#ifndef RAM_H_INCLUDED
#define RAM_H_INCLUDED

// include SPI lib
#ifndef SPIMINIMAL_H_INCLUDED
#include "SPI.h"
#endif
extern SPIClass SPI;

// LIBRARY STUFF
#define NONE 0x00
#define SUPPORTED_SLAVES 3

// GPIO
#define DDR_RAM DDRD
#define PORT_RAM PORTD

// 23k256 operations
#define READ 0x03
#define WRITE 0x02
#define RDSR 0x05
#define WRSR 0x01

// 23k256 sreg masks
#define BYTE_MODE 0x00
#define PAGE_MODE 0x80
#define SEQ_MODE 0x40

// RAM CLASS and related structs
struct SlaveStruct{
  uint8_t SS, HOLD, MODE;  
};

class RAMClass {
  public:
    SlaveStruct slaveInfo[SUPPORTED_SLAVES];
    SlaveStruct dummy = {-1, -1, -1};
    uint8_t attachedSlaves; // number of slaves connected to arduino
    
    RAMClass();
    
    void add(uint8_t SS, uint8_t MODE);
    void add(uint8_t SS, uint8_t HOLD, uint8_t MODE);

    // slave info request functions
    inline SlaveStruct& getSlaveInfo(uint8_t slave){
      if(slave < SUPPORTED_SLAVES)
        return slaveInfo[slave];
      return dummy;
    }
    
    // transmition funtions
    uint8_t readSr(uint8_t SS);
    void writeSr(uint8_t SS, uint8_t MODE);
    inline static uint8_t read(uint8_t SS, uint16_t addr){
      uint8_t read_data;
      // pull SS low to start transmittion
      PORT_RAM &= ~_BV(SS);

      //__asm__ __volatile__ ("delayTransistor\n"); 
      SPI.transfer(READ);
      SPI.transfer16(addr);
      read_data = SPI.transfer(NONE);
      
      // signal end transmittion by pulling ss low
      PORT_RAM |= _BV(SS);
      return read_data;
    }
    inline static void write(uint8_t SS, uint8_t data, uint16_t addr){
      // this inline funtion seems to work only if declared inside the class
      // pull SS low to start transmittion
      PORT_RAM &= ~_BV(SS);
      
      //__asm__ __volatile__ ("delayTransistor\n"); 
      SPI.transfer(WRITE);
      SPI.transfer16(addr);
      SPI.transfer(data);

      // signal end transmittion by pulling ss low
      PORT_RAM |= _BV(SS);
  }
  inline static void write(uint8_t SS, void* buffer, uint32_t size, uint16_t addr){
    // pull SS low to start transmittion
    PORT_RAM &= ~_BV(SS);
    
    //__asm__ __volatile__ ("delayTransistor\n");
    SPI.transfer(WRITE);
    SPI.transfer16(addr);
    SPI.transfer(buffer, size);
    
    // signal end transmittion by pulling ss low
    PORT_RAM |= _BV(SS);
  }
  inline static void read(uint8_t SS, uint8_t* buffer, uint32_t size, uint16_t addr)
  {
    // pull SS low to start transmittion
    PORT_RAM &= ~_BV(SS);
    
    //__asm__ __volatile__ ("delayTransistor\n");
    SPI.transfer(READ);
    SPI.transfer16(addr);
    SPI.transfer(buffer, size);
    
    // signal end transmittion by pulling ss low
    PORT_RAM |= _BV(SS);
  }
};

extern RAMClass RAM;
#endif
