#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BluetoothUtils.h>
#include <GlassUtils.h>
#include <DisplayUtils.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <TFT_eSPI.h>

namespace Bluetooth
{

BluetoothUtils::BluetoothUtils() : SerialBT()
{
    // define buffer size to hold the bluetooth data
    buffer_size = 128;
    // allocate buffer using unique pointer
    buffer = std::unique_ptr<char>(new char [buffer_size]);
}

BluetoothUtils::~BluetoothUtils()
{
    // call destructor for member subclass
    SerialBT.~BluetoothSerial();
}

void BluetoothUtils::Begin()
{
    // initialize serial bluetooth communication
    SerialBT.begin("Smart Glass V1.0");
}

void BluetoothUtils::WriteToBuffer()
{
    // if there is a message inbound
    if(this->Available())
    {
        // resets old contents
        memset(buffer.get(), ' ', buffer_size);
        // writes the incoming telemetry to buffer
        SerialBT.readBytes(buffer.get(), buffer_size);

        #ifdef GLASS_DEBUG
            for(int i=0; i<buffer_size; i++)
            {
                Serial.print(buffer[i]);
            }
            Serial.print('\n');
        #endif
    }
    else
    {
        // do nothing
    }
}

bool BluetoothUtils::Available()
{
    // return bluetooth availability
    return SerialBT.available();
}

}; // namespace