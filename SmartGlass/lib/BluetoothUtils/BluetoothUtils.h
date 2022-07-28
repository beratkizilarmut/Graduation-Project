#pragma once
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <DisplayUtils.h>
#include <Adafruit_SSD1306.h>

namespace Bluetooth
{

class BluetoothUtils
{
    public:
        BluetoothUtils();
        ~BluetoothUtils();

        void Begin();
        void WriteToBuffer();
        bool Available();

        size_t buffer_size;
        std::unique_ptr<char> buffer;

    private:
        BluetoothSerial SerialBT;
};

} //namespace