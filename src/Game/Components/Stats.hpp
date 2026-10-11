#include "ITemplateStat.hpp"

namespace APP 
{
    class HealtStat : public ITemplateStat<uint8_t> 
    {
        public:
            explicit HealtStat() : 
                ITemplateStat<uint8_t>(100, 0, 100, "vida")
            {};

    };

    class StrengthStat : public ITemplateStat<uint8_t>
    {
        public:
            explicit StrengthStat() : 
                ITemplateStat<uint8_t>(20, 0, 20, "Fuerza")
            {};
    };

    class DefenseStat : public ITemplateStat<uint8_t>
    {
        public:
            explicit DefenseStat() :
                ITemplateStat<uint8_t>(10, 0, 10, "Defensa")
            {}
    };

    class XPStat : public ITemplateStat<uint32_t>
    {
        public:
            explicit XPStat() :
                ITemplateStat<uint32_t>(0, 0, "Experiencia")
            {}        
    };
}