#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BluetoothUtils.h>
#include <DisplayUtils.h>
#include <GlassUtils.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <TFT_eSPI.h>

GlassUtils Glass;

void setup()
{
  Serial.begin(115200);
  
  Glass.Bluetooth.Begin();
  Glass.StartBluetoothWriteToBufferTask();
  
  Glass.Display.Begin();
  Glass.StartDisplayTask();
  Glass.StartDisplayUpdateTimeTask();
  Glass.StartIOUpdateIOTask();
}

void loop()
{
  
}