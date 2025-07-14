#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

// TODO: implement a way to disable logging in production builds
class Logger
{
    public:
        static void log(const String& message)
        {
            Serial.println(message);
        }
};

#endif
