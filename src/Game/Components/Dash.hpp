/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Utils/Vector2.hpp"
#include "Engine/Component/Component.hpp"
/// | ------------------------------------ |

namespace APP
{
  enum class DashState 
  {
    Ready,
    Dashing,
    Cooldown
  };

  class Dash : public ENG::IComponents
  {
    public:
      Dash(float cooldown, float speedMult, float duration)
        : ENG::IComponents{}, m_cooldown{cooldown}, m_speedMultiplier{speedMult}, m_duration{duration}
      { }

      DashState GetState(void) const        { return this->m_state;}
      bool IsDashing(void) const            { return this->m_state == DashState::Dashing;}
      ENG::Vector2 GetDirecction() const    { return this->m_direction;}

      void UpdateDash(ENG::ITransform& transform, float dt);
      void TryDash(ENG::ITransform& transform);

    private:
      DashState m_state = DashState::Ready;
      ENG::Vector2 m_direction = {0, 0};
      ENG::Vector2 m_normalVelocity = {0, 0};
      float m_timer = 0.0f;
      float m_duration = 0.5f;
      float m_cooldown = 5.0f;
      float m_speedMultiplier = 2.0f;
  };
}
