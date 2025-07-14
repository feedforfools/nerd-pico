#ifndef INTERFACES_H
#define INTERFACES_H

#include "types/events.h"
#include <vector>

class IMusicalEventSink
{
    public:
        virtual ~IMusicalEventSink() = default;
        virtual void onMusicalEvent(const MusicalEvent& event) = 0;
        virtual PortId getPortId() const = 0;
};

class IMusicalEventSource
{
    public:
        virtual ~IMusicalEventSource() = default;

        void registerListener(IMusicalEventSink* sink)
        {
            listeners.push_back(sink);
        }

        virtual PortId getPortId() const = 0;
    
    protected:
        std::vector<IMusicalEventSink*> listeners;
        
        void fireEvent(const MusicalEvent& event)
        {
            for (auto* listener : listeners)
            {
                listener->onMusicalEvent(event);
            }
        }
};

#endif