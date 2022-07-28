#pragma once
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BluetoothUtils.h>
#include <DisplayUtils.h>
#include <IOUtils.h>
#include <Adafruit_SSD1306.h>

using namespace Bluetooth;
using namespace Display;
using namespace IO;

class GlassUtils
{
    public:
        GlassUtils();
        ~GlassUtils();

        void StartBluetoothWriteToBufferTask();
        static void StartBluetoothWriteToBufferTaskImpl(void* _this);
        void BluetoothWriteToBufferTask();

        void StartDisplayTask();
        static void StartDisplayTaskImpl(void* _this);
        void DisplayTask();

        void StartDisplayUpdateTimeTask();
        static void StartDisplayUpdateTimeTaskImpl(void* _this);
        void DisplayUpdateTimeTask();

        void StartIOUpdateIOTask();
        static void StartIOUpdateIOTaskImpl(void* _this);
        void IOUpdateIOTask();

        bool update_flag;
        bool button_flag;

        BluetoothUtils Bluetooth;
        DisplayUtils Display;
        IOUtils IO;
};