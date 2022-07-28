#include <Arduino.h>
#include <BluetoothSerial.h>
#include <SPI.h>
#include <IOUtils.h>

#define GLASS_DEBUG

namespace IO
{

IOUtils::IOUtils()
{
    ignore_cycle = 0;
    IOTouch = 0;
}

IOUtils::~IOUtils()
{
    
}

void IOUtils::UpdateIO(bool& update_flag, bool& button_flag)
{
    if(ignore_cycle != 0)
    {
        --ignore_cycle;
    }
    
    IOTouch = touchRead(T7);

    #ifdef GLASS_DEBUG
        Serial.println(IOTouch);
    #endif

    if(IOTouch < 15 && ignore_cycle == 0)
    {
        button_flag = true;
        update_flag = true;
        ignore_cycle = 5; // 200ms delay per cycle, 1000 ms per check
    }
}

}