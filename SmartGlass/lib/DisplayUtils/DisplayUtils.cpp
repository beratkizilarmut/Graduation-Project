#include <Arduino.h>
#include <BluetoothSerial.h>
#include <BluetoothUtils.h>
#include <GlassUtils.h>
#include <DisplayUtils.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <FS.h>
#include <SPIFFS.h>

#define GLASS_DEBUG

namespace Display
{

DisplayUtils::DisplayUtils() : tft()
{
    // initial state for the display
    current_state = display_state::main_screen;
    // initial values for the display
    hour = 0;
    minute = 0;
    second = 0;

    time_x = 0;
    time_y = 0;

    date_x = 60;
    date_y = 0;

    sender_x = 0;
    sender_y = 10;

    message_x = 0;
    message_y = 20;

    offset_x = 0;
    offset_y = 0;

    time_buffer_size = 6;
    time_buffer = std::unique_ptr<char>(new char[time_buffer_size]);
    sprintf(time_buffer.get(), "%02d:%02d", hour, minute);
    time_buffer.get()[time_buffer_size-1] = '\0';

    sender_buffer_size = 64;
    sender_buffer = std::unique_ptr<char>(new char[sender_buffer_size]);
    message_buffer_size = 128;
    message_buffer = std::unique_ptr<char>(new char[message_buffer_size]);
    sprintf(sender_buffer.get(),"Error");
    sprintf(message_buffer.get(),"Last Message Not Found");

    date_buffer_size = 8;
    date_buffer = std::unique_ptr<char>(new char[date_buffer_size]);

    show_message_counter = 0;
    time_flag = false;
    sender_flag = false;
    message_flag = false;
    show_message_flag = false;
    date_flag = false;
}

DisplayUtils::~DisplayUtils()
{
    tft.~TFT_eSPI();
}

void DisplayUtils::Begin()
{
    tft.init();
    tft.setRotation(3);
}

void DisplayUtils::ParseBT(char* buffer, size_t buffer_size, bool& update_flag)
{
    switch(buffer[0])
    {
        case 'T':
            // if the packet tag is Time (-1 because first is reserved for 'T' tag)
            this->hour = (((uint8_t) buffer[1] - 48) * 10) + (((uint8_t) buffer[2] - 48) * 1);
            this->minute = (((uint8_t) buffer[3] - 48) * 10) + (((uint8_t) buffer[4] - 48) * 1);
            // parse values into a c string
            sprintf(time_buffer.get(), "%02d:%02d", hour, minute);
            time_buffer.get()[time_buffer_size-1] = '\0';
            time_flag = true;
            break;

        case 'S':
            // if the packet tag is Sender (-1 because first is reserved for 'S' tag)
            memset(sender_buffer.get(), ' ', sender_buffer_size);
            strncpy(sender_buffer.get(), (buffer+1), sender_buffer_size);
            sender_buffer.get()[sender_buffer_size-1] = '\0';
            sender_flag = true;
            break;

        case 'M':
            // if the packet tag is Message (-1 because first is reserved for 'M' tag)
            memset(message_buffer.get(), ' ', message_buffer_size);
            strncpy(message_buffer.get(), (buffer+1), message_buffer_size-1);
            message_buffer.get()[message_buffer_size-1] = '\0';
            message_flag = true;
            break;

        case 'D':
            // if the packet tag is Date (-1 because first is reserved for 'D' tag)
            memset(date_buffer.get(), ' ', date_buffer_size);
            strncpy(date_buffer.get(), (buffer+1), date_buffer_size-1);
            date_buffer.get()[date_buffer_size-1] = '\0';
            date_flag = true;
            break;
        
        case 'X':
            {
            // if the packet tag is X offset (-1 because first is reserved for 'X' tag)
            uint8_t end_index = 0;
            for(int ii=0; ii<buffer_size; ++ii)
            {
                if(buffer[ii] == ' ')
                {
                    end_index = ii;
                    break;
                }
            }

            #ifdef GLASS_DEBUG
                Serial.printf("end index: %d\n", end_index);
            #endif

            switch (end_index)
            {
                case 2:
                    this->offset_x = (((uint8_t) buffer[1] - 48) * 1);
                    break;
                
                case 3:
                    this->offset_x = (((uint8_t) buffer[1] - 48) * 10) + (((uint8_t) buffer[2] - 48) * 1);
                    break;

                case 4:
                    this->offset_x = (((uint8_t) buffer[1] - 48) * 100) + (((uint8_t) buffer[2] - 48) * 10) + (((uint8_t) buffer[3] - 48) * 1);
                    break;

                default:
                    // failed to assign to offset values
                    break;
            }

            #ifdef GLASS_DEBUG
                Serial.printf("offset_x: %d\n", offset_x);
            #endif

            break;
            }

        case 'Y':
            {
            // if the packet tag is X offset (-1 because first is reserved for 'X' tag)
            uint8_t end_index = 0;
            for(int ii=0; ii<buffer_size; ++ii)
            {
                if(buffer[ii] == ' ')
                {
                    end_index = ii;
                    break;
                }
            }

            #ifdef GLASS_DEBUG
                Serial.printf("end index: %d\n", end_index);
            #endif

            switch (end_index)
            {
                case 2:
                    this->offset_y = (((uint8_t) buffer[1] - 48) * 1);
                    break;
                
                case 3:
                    this->offset_y = (((uint8_t) buffer[1] - 48) * 10) + (((uint8_t) buffer[2] - 48) * 1);
                    break;

                case 4:
                    this->offset_y = (((uint8_t) buffer[1] - 48) * 100) + (((uint8_t) buffer[2] - 48) * 10) + (((uint8_t) buffer[3] - 48) * 1);
                    break;

                default:
                    // failed to assign to offset values
                    break;
            }

            #ifdef GLASS_DEBUG
                Serial.printf("offset_y: %d\n", offset_y);
            #endif

            break;
            }

        default:
            // tag not known
            break;
    }

    if(sender_flag && message_flag)
    {
        show_message_flag = true;
        show_message_counter = 10;
    }

    #ifdef GLASS_DEBUG
        Serial.print(buffer[0]);
        Serial.print(buffer[1]);
        Serial.print(buffer[2]);
        Serial.print(buffer[3]);
        Serial.print(buffer[4]);
        Serial.print('\n');
    #endif

    #ifdef GLASS_DEBUG
        // update flag for screen refresh
        Serial.print("flag set\n");
    #endif

    // set update flag
    update_flag = true;
}

void DisplayUtils::UpdateTime(bool& update_flag)
{
    if(time_flag)
    {
        ++second;
        if(second == 60)
        {
            second = 0;
            ++minute;
            #ifdef GLASS_DEBUG
                Serial.write("time refresh\n");
            #endif
            sprintf(time_buffer.get(), "%02d:%02d", hour, minute);
            time_buffer.get()[time_buffer_size-1] = '\0';
            // if change is  visible return update flag as true
            update_flag = true;
        }
        if(minute == 60)
        {
            minute = 0;
            ++hour;
            #ifdef GLASS_DEBUG
                Serial.write("time refresh\n");
            #endif
            sprintf(time_buffer.get(), "%02d:%02d", hour, minute);
            time_buffer.get()[time_buffer_size-1] = '\0';
            // if change is  visible return update flag as true
            update_flag = true;
        }
        if(hour == 24)
        {
            hour = 0;
            #ifdef GLASS_DEBUG
                Serial.write("time refresh\n");
            #endif
            sprintf(time_buffer.get(), "%02d:%02d", hour, minute);
            time_buffer.get()[time_buffer_size-1] = '\0';
            // if change is  visible return update flag as true
            update_flag = true;
        }
    }

    // message counter update
    if(sender_flag && message_flag && (show_message_counter == 0))
    {
        sender_flag = false;
        message_flag = false;
        show_message_flag = false;
    }
    else
    {
        show_message_counter--;
    }
}

void DisplayUtils::Refresh(bool& update_flag, bool& button_flag)
{
    // check update flag
    if(update_flag)
    {
        #ifdef GLASS_DEBUG
            Serial.write("screen refresh\n");
        #endif

        if(button_flag)
        {
            switch(current_state)
            {
                case display_state::main_screen:
                    current_state = display_state::last_notification;
                    break;
                
                case display_state::last_notification:
                    current_state = display_state::status_screen;
                    break;

                case display_state::status_screen:
                    current_state = display_state::test;
                    break;
                
                default:
                    current_state = display_state::main_screen;
                    break;
            }
            button_flag = false;
        }

        switch(current_state)
        {
            case display_state::main_screen:
                tft.fillScreen(TFT_BLACK);
                if(time_flag)
                {
                    tft.setTextSize(2);
                    tft.drawString(time_buffer.get(), offset_x + time_x, offset_y + time_y);
                }
                if(date_flag)
                {
                    tft.setTextSize(1);
                    tft.drawString(date_buffer.get(), offset_x + date_x, offset_y + date_y);
                }
                if(show_message_flag)
                {
                    tft.setTextSize(1);
                    tft.drawString("New Message", offset_x + time_x, offset_y + time_y + 17);
                    tft.drawString("From: ", offset_x + time_x, offset_y + time_y + 27);
                    tft.drawString(sender_buffer.get(), offset_x + sender_x + 30, offset_y + sender_y + 17);
                    tft.drawString(message_buffer.get(), offset_x + message_x, offset_y + message_y + 17);
                }
                break;

            case display_state::last_notification:
                tft.fillScreen(TFT_BLACK);
                tft.setTextSize(1);
                if(time_flag)
                {
                    tft.drawString(time_buffer.get(), offset_x + time_x, offset_y + time_y);
                }
                tft.drawString("Last Message", offset_x + time_x , offset_y + time_y + 10);
                tft.drawString("From: ", offset_x + time_x, offset_y + time_y + 20);
                tft.drawString(sender_buffer.get(), offset_x + sender_x + 30, offset_y + sender_y + 10);
                tft.drawString(message_buffer.get(), offset_x + message_x, offset_y + message_y + 10);
                break;

            case display_state::status_screen:
                tft.fillScreen(TFT_BLACK);
                tft.setTextSize(1);
                if(time_flag)
                {
                    tft.drawString(time_buffer.get(), offset_x + time_x, offset_y + time_y);
                }
                tft.drawString("System Status", offset_x, offset_y + time_y + 10);
                tft.drawString("BT On", offset_x, time_y + offset_y + 20);
                break;
            
            case display_state::test:
                tft.fillScreen(TFT_BLACK);
                tft.setTextSize(1);
                tft.drawString("a1a2a3a4a5a6a7a8a9a1a2a3a4", 0 ,0);
                tft.drawString("b1b2b3b4b5b6b7b8b9b1b2b3b4", 0 ,15);
                tft.drawString("c1c2c3c4c5c6c7c8c9c1c2c3c4", 0 ,30);
                tft.drawString("d1d2d3d4d5d6d7d8d9d1d2d3d4", 0 ,45);
                tft.drawString("e1e2e3e4e5e6e7e8e9e1e2e3e4", 0 ,60);
                tft.drawString("f1f2f3f4f5f6f7f8f9f1f2f3f4", 0 ,75);
                break;
        }
        // clear update flag after refresh
        update_flag = false;
        #ifdef GLASS_DEBUG
            Serial.write("flag cleared\n");
        #endif
    }
    else
    {
        // do nothing
    }
}

}