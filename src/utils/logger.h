#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "config/config.h"

class Logger
{
    public:
        static void begin(long speed = 9600)
        {
        #if defined(HAVE_USB_SERIAL)
            Serial.begin(speed);
            while(!Serial && millis() < 4000);
        #endif
        }

        static void log(const String& message)
        {
        #if defined(ENABLE_LOGGING) && defined(HAVE_USB_SERIAL)
            Serial.println(message);
        #endif
        }

        static void flush()
        {
        #if defined(ENABLE_LOGGING) && defined(HAVE_USB_SERIAL)
            Serial.flush();
        #endif
        }

        static int available()
        {
        #if defined(HAVE_USB_SERIAL)
            return Serial.available();
        #else
            return 0;
        #endif
        }

        static int read()
        {
        #if defined(HAVE_USB_SERIAL)
            return Serial.read();
        #else
            return -1;
        #endif
    }

};

#endif
