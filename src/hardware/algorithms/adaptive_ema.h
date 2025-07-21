#ifndef ADAPTIVE_EMA_H
#define ADAPTIVE_EMA_H

#include <cstdint>
#include <type_traits>

/**
 * @brief   Adaptive Exponential Moving Average (EMA) filter.
 *          Applies strong filtering when the input is stable/centered and
 *          light filtering during active movement, with seamless transitions.
 */
template <uint8_t K_STRONG, uint8_t K_LIGHT, class uint_t = uint16_t>
class AdaptiveEMA
{
public:
    AdaptiveEMA() : lastFilterWasStrong(true) {}

    uint_t operator()(uint_t input, bool useStrongFiltering)
    {
        if (useStrongFiltering)
        {
            // If we just switched from light to strong filtering...
            if (!lastFilterWasStrong)
            {
                // ...initialize the strong filter's state from the light one to ensure a smooth transition
                strongState = lightState << (K_STRONG - K_LIGHT);
            }
            lastFilterWasStrong = true;
            
            // Update and return the output of the strong filter ONLY
            strongState += input - (strongState >> K_STRONG);
            return (strongState + (uint_t{1} << (K_STRONG - 1))) >> K_STRONG;
        }
        else // use light filtering
        {
            // If we just switched from strong to light filtering...
            if (lastFilterWasStrong)
            {
                // ...initialize the light filter's state from the strong one
                lightState = strongState >> (K_STRONG - K_LIGHT);
            }
            lastFilterWasStrong = false;
            
            // Update and return the output of the light filter ONLY
            lightState += input - (lightState >> K_LIGHT);
            return (lightState + (uint_t{1} << (K_LIGHT - 1))) >> K_LIGHT;
        }
    }

    void reset(uint_t initialValue = 0)
    {
        strongState = initialValue << K_STRONG;
        lightState = initialValue << K_LIGHT;
        lastFilterWasStrong = true;
    }

private:
    uint_t strongState = 0; // State for strong filtering
    uint_t lightState = 0;  // State for light filtering
    bool lastFilterWasStrong; // Tracks the last used filter to detect switches

    static_assert(std::is_unsigned<uint_t>::value, "The uint_t type should be an unsigned integer type");
    static_assert(K_STRONG > 0 && K_LIGHT > 0, "K_STRONG and K_LIGHT should be greater than 0");
    static_assert(K_STRONG >= K_LIGHT, "K_STRONG should be >= K_LIGHT for stronger center filtering");
};

#endif