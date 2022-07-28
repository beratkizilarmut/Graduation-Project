#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BluetoothUtils.h>
#include <GlassUtils.h>
#include <DisplayUtils.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <TFT_eSPI.h>

using namespace Bluetooth;

GlassUtils::GlassUtils()
{
  update_flag = true;
  button_flag = false;
}

GlassUtils::~GlassUtils()
{
  
}

// BLUETOOTH TASKS

// WRITE TO BUFFER
void GlassUtils::BluetoothWriteToBufferTask()
{
  for(;;)
  {
    if(Bluetooth.Available())
    {
      // sets update flag if the bluetooth transmission is recieved
      Bluetooth.WriteToBuffer();
      // parse the bluetooth transmission
      Display.ParseBT(Bluetooth.buffer.get(), Bluetooth.buffer_size, this->update_flag);
    }
    else
    {
      /*
      for(size_t ii=0; ii<Bluetooth.buffer_size; ++ii)
      {
        Serial.write(Bluetooth.buffer[ii]);
      }
      */
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}

void GlassUtils::StartBluetoothWriteToBufferTaskImpl(void* _this)
{
  ((GlassUtils*)_this)->BluetoothWriteToBufferTask();
}

void GlassUtils::StartBluetoothWriteToBufferTask()
{
  xTaskCreate
  (
    this->StartBluetoothWriteToBufferTaskImpl, //func ptr
    "StartBTBuffer", //task name
    8192, //stack size
    (void*) this, //task parameters
    2, //task priority
    NULL //task handle
  );
}

// DISPLAY TASKS

void GlassUtils::DisplayTask()
{
  for(;;)
  {
    Display.Refresh(this->update_flag, this->button_flag);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void GlassUtils::StartDisplayTaskImpl(void* _this)
{
  ((GlassUtils*)_this)->DisplayTask();
}

void GlassUtils::StartDisplayTask()
{
  xTaskCreate
  (
    this->StartDisplayTaskImpl, //func ptr
    "StartDPTest", //task name
    65536, //stack size
    (void*) this, //task parameters
    1, //task priority
    NULL //task handle
  );
}

// TIME

void GlassUtils::DisplayUpdateTimeTask()
{
  for(;;)
  {
    Display.UpdateTime(this->update_flag);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void GlassUtils::StartDisplayUpdateTimeTaskImpl(void* _this)
{
  ((GlassUtils*)_this)->DisplayUpdateTimeTask();
}

void GlassUtils::StartDisplayUpdateTimeTask()
{
  xTaskCreate
  (
    this->StartDisplayUpdateTimeTaskImpl, //func ptr
    "StartTimeTest", //task name
    1024, //stack size
    (void*) this, //task parameters
    1, //task priority
    NULL //task handle
  );
}

// IO

void GlassUtils::IOUpdateIOTask()
{
  for(;;)
  {
    IO.UpdateIO(this->update_flag, this->button_flag);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void GlassUtils::StartIOUpdateIOTaskImpl(void* _this)
{
  ((GlassUtils*)_this)->IOUpdateIOTask();
}

void GlassUtils::StartIOUpdateIOTask()
{
  xTaskCreate
  (
    this->StartIOUpdateIOTaskImpl, //func ptr
    "StartUpdateIO", //task name
    1024, //stack size
    (void*) this, //task parameters
    2, //task priority
    NULL //task handle
  );
}