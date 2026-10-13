#include "Stats.hpp"

namespace APP
{
    void HealtStat::receiveDamage(uint8_t damage){
        const uint8_t current = getCurrent();
        setCurrent(damage >= current ? getMin() : static_cast<uint8_t>(current - damage));
    }
}