/// | ------------------------------------ |
#include "Game/Components/Shield.hpp"
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  void Shield::ActiveShield(void)
  {
    switch (this->m_state)
    {
      case ShieldState::Ready:
          this->m_state = ShieldState::Active;
        break;

      case ShieldState::Active:
        break;

      case ShieldState::Cooldown:
        this->m_timer += this->m_step;

        if (this->m_timer >= this->m_cooldown)
        {
          this->m_hp = this->m_maxHp;
          this->m_timer = 0.0f;
          this->m_state = ShieldState::Ready;
        }
        break;
    }
  }

  void Shield::UpdateShield(ENG::Object& player, uint8_t damage, const ENG::Vector2& directionToSource)
  {
    bool r = false;
    if(this->m_state == ShieldState::Active)
      r = this->TryBlockShield(damage,directionToSource);
    if(r)
    {
      this->ApplyDamageShield(damage);
      return;
    }

    if(damage >= player.GetStats().m_hp){
      player.GetStats().m_hp = 0;
    } else{
      player.GetStats().m_hp -= damage;
    }
  }

  //// Private funtions
  
  bool Shield::TryBlockShield(uint8_t damage, const ENG::Vector2& directionToSource)
  {
    auto incomingDirection = directionToSource;
    if (incomingDirection.Length() <= 0.0001f) // evitar normalizacion de vectores nulo.s
      return false;

    incomingDirection.Normalize();

    const float alignment = this->m_direction.x * incomingDirection.x + this->m_direction.y * incomingDirection.y;

    if (alignment < this->m_minimumBlockDot)
      return false;

    return true;
  }

  void Shield::ApplyDamageShield(uint8_t dmg)
  {
    if(this->m_state != ShieldState::Active)
      return;
    if(dmg >= this->m_hp)
    {
      this->m_hp = 0;
      this->m_timer = 0.0f;
      this->m_state = ShieldState::Cooldown;
    }
    else
    {
      this->m_hp -= dmg;
    }
  }

}
