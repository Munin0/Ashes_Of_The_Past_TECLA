/// | ------------------------------------ |
#include "Heal.hpp"
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
/// | ------------------------------------ |
#include "Game/Components/Stats.hpp"
/// | ------------------------------------ |
#include <algorithm>
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  bool Heal::TryHeal(ENG::Object& player)
  {
    if(this->m_state == HealState::Cooldown)
      return false;

    auto* health = player.GetComponent<HealtStat>();
    if(!health || health->isDead())
      return false;

    const uint8_t current = health->getCurrent();
    const uint8_t max     = health->getMax();

    if(current >= max)
      return false;

    const uint8_t healed = static_cast<uint8_t>(
        std::min<uint32_t>(this->m_amount, static_cast<uint32_t>(max - current)));
    health->setCurrent(static_cast<uint8_t>(current + healed));

    this->m_timer = 0.0f;
    this->m_state = HealState::Cooldown;
    return true;
  }

  void Heal::UpdateHeal(float dt)
  {
    if(this->m_state != HealState::Cooldown)
      return;

    this->m_timer += dt;
    if(this->m_timer >= this->m_cooldown)
    {
      this->m_timer = 0.0f;
      this->m_state = HealState::Ready;
    }
  }
}
