/// | ------------------------------------ |
#include "ITemplateStat.hpp"
/// | ------------------------------------ |
#include <cstdint>
/// | ------------------------------------ |

namespace APP 
{
  class HealtStat : public ITemplateStat<uint8_t> 
  {
    public:
      explicit HealtStat()
        : ITemplateStat<uint8_t>(100, 0, 100, "vida")
      {};
      
      void receiveDamage(uint8_t damage);
      bool isDead() const;
  };

  class StrengthStat : public ITemplateStat<uint8_t>
  {
    public:
      explicit StrengthStat()
        : ITemplateStat<uint8_t>(50, 0, 40, "Fuerza")
      {}
  };

  class DefenseStat : public ITemplateStat<uint8_t>
  {
    public:
      explicit DefenseStat()
        : ITemplateStat<uint8_t>(10, 0, 10, "Defensa")
      {}
  };

  class XPStat : public ITemplateStat<uint32_t>
  {
    public:
      explicit XPStat()
        : ITemplateStat<uint32_t>(0, 0, "Experiencia")
      {}        
  };

  class MeleeAttackDistance : public ITemplateStat<float>
  {
    public:
      explicit MeleeAttackDistance()
        : ITemplateStat<float>(100.f, 0.5f, 100.f, "Distancia de ataque cuerpo a cuerpo")
      {}
  };

  class MeleeAttackRange : public ITemplateStat<float>
  {
    public:
      explicit MeleeAttackRange()
        : ITemplateStat<float>(100, 0, 50, "Rango de ataque cuerpo a cuerpo")
      {}
  };

  class MeleeAttackVelocity : public ITemplateStat<float>
  {
    public: 
      explicit MeleeAttackVelocity()
        : ITemplateStat<float>(1.f, 0.1f, 0.8f, "Velocidad de ataque cuerpo a cuerpo")
      {}
  };
}
