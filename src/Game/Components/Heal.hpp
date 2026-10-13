/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Object/Object.hpp"
/// | ------------------------------------ |
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  enum class HealState
  {
    Ready,
    Cooldown
  };

  class Heal : public ENG::IComponents
  {
    public:
      Heal(float cooldown, uint32_t amount)
        : ENG::IComponents{}, m_cooldown{cooldown}, m_amount{amount}
      {}

      HealState GetState(void) const { return this->m_state;}
      bool IsReady(void) const       { return this->m_state == HealState::Ready;}
      float GetTimer(void) const     { return this->m_timer;}
      float GetCooldown(void) const  { return this->m_cooldown;}
      uint32_t GetAmount(void) const { return this->m_amount;}

      bool TryHeal(ENG::Object& player);
      void UpdateHeal(float dt);

    private:
      HealState m_state = HealState::Ready;
      float     m_timer = 0.0f;
      float     m_cooldown = 5.0f;
      uint32_t  m_amount = 50;
  };
}
