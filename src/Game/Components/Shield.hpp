/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  enum class ShieldState
  {
    Ready,
    Active,
    Cooldown
  };

  class Shield : public ENG::IComponents
  {

    public:
    Shield(float step, float cooldown, uint8_t maxHP)
      : ENG::IComponents{},m_step{step}, m_state{ShieldState::Ready}, m_cooldown{cooldown}, m_maxHp{maxHP}, m_hp{maxHP}
    {}
    ~Shield() override {}

    void SetState(const ShieldState& state)            { this->m_state = state;}
    void SetCooldown(float newColdown)                { this->m_cooldown = newColdown;}
    void SetMaxHP(uint8_t newMaxHP)                   { this->m_maxHp = newMaxHP <= 255 ? newMaxHP : m_maxHp;}
    void SetDirecction(const ENG::Vector2& direction) { this->m_direction = direction;}

    ShieldState GetState(void) const        { return this->m_state;}
    ENG::Vector2 GetDirecction(void) const  { return this->m_direction;}
    uint8_t GetActualHP(void) const         { return this->m_hp;}

    void ActiveShield(void);
    void UpdateShield(ENG::Object& player, uint8_t damage, const ENG::Vector2& directionToSource);
    
    void ResetShield(void)
    {
      this->m_hp = this->m_maxHp;
      this->m_timer = 0.0f;
    }
    
    private:
    void ApplyDamageShield(uint8_t dmg);
    bool TryBlockShield(uint8_t damage, const ENG::Vector2& directionToSource);

    private:
      ShieldState   m_state;
      float         m_timer = 0.0f;
      float         m_step = 0.0f;
      float         m_cooldown = 5.0f;
      float         m_minimumBlockDot = 0.5f; //bloqueo de 120 grados (60 grados cada lado), -1 contrario, 0 perpendicular y mas 1 es perfecto
      ENG::Vector2 m_direction;
      uint8_t       m_hp;
      uint8_t       m_maxHp;
  };
}
