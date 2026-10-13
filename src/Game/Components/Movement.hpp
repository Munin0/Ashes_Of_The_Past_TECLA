/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |

namespace APP
{
  enum class MovementState
  {
    Walking,
    Running,
    Cooldown
  };

  class Movement : public ENG::IComponents
  {
    public:
      explicit Movement(const ENG::Vector2& walkinV, const ENG::Vector2& runningV, float cooldown, float step, float duration)
        : ENG::IComponents{}, m_walkingV{walkinV}, m_runningV{runningV}, m_cooldown{cooldown}, m_step{step}, m_duration{duration}
      {}

      void ApplyRun(void);
      void ApplyMovement(ENG::ITransform& tranform, float dt);
      void UpdateMovement(ENG::ITransform& transform, float dt, bool entityDashing);

      bool IsRunning(void) const { return this->m_state == MovementState::Running;}

      MovementState GetState(void) const { return this->m_state;}
      float GetTimer(void) const         { return this->m_timer;}
      float GetDuration(void) const      { return this->m_duration;}
      float GetCooldown(void) const      { return this->m_cooldown;}
    private:
      MovementState   m_state = MovementState::Walking;
      float           m_timer = 0.0f;
      float           m_cooldown = 0.0f;
      float           m_duration = 0.0f;
      float           m_step = 0.0f;
      ENG::Vector2    m_walkingV;
      ENG::Vector2    m_runningV;

  };
}
