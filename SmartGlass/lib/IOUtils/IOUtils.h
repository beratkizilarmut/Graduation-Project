#pragma once
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <SPI.h>

namespace IO
{
    
class IOUtils
{
    public:
        IOUtils();
        ~IOUtils();
        
        void UpdateIO(bool&, bool&);

        uint8_t ignore_cycle;
        uint16_t IOTouch;

};

} //namespace