#ifndef EVENETS_H
#define EVENETS_H

#include "port_id.h"

enum class MusicalEventType
{
    PITCH_BEND,
    CONTROL_CHANGE
};

struct MusicalEvent
{
    PortId id;
    MusicalEventType type;
    uint8_t channel;

    // Control change events uses data1 as CC number and data2 as 7-bit value
    uint8_t data1;
    uint8_t data2;
    // Pitch bend events uses value as 14-bit value (0-16383)
    uint16_t value;
};

#endif