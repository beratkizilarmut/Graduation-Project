#pragma once
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BluetoothUtils.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <TFT_eSPI.h>

namespace Display
{

class DisplayUtils
{
    public:
        DisplayUtils();
        ~DisplayUtils();

        void Begin();
        void Refresh(bool&, bool&);
        void ParseBT(char*, size_t, bool&);
        void UpdateTime(bool&);

        enum class display_state:uint8_t { main_screen = 0, last_notification = 1, status_screen = 2, test = 3};
        display_state current_state;

        uint8_t hour;
        uint8_t minute;
        uint8_t second;

        uint8_t time_x;
        uint8_t time_y;

        uint8_t date_x;
        uint8_t date_y;

        uint8_t sender_x;
        uint8_t sender_y;

        uint8_t message_x;
        uint8_t message_y;

        uint8_t offset_x;
        uint8_t offset_y;
        
        std::unique_ptr<char> time_buffer;
        size_t time_buffer_size;
        std::unique_ptr<char> sender_buffer;
        size_t sender_buffer_size;
        std::unique_ptr<char> message_buffer;
        size_t message_buffer_size;
        std::unique_ptr<char> date_buffer;
        size_t date_buffer_size;

        size_t show_message_counter;
        bool time_flag;
        bool sender_flag;
        bool message_flag;
        bool date_flag;
        bool show_message_flag;

    private:
        TFT_eSPI tft;
};

} //namespace