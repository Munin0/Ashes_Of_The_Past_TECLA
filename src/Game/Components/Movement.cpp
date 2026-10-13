/// | ------------------------------------ |
#include "Movement.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |

namespace APP
{
  void Movement::ApplyMovement(ENG::ITransform& transform, float dt)
  {
    if(transform.m_direction == ENG::Vector2{0.f, 0.f})
      return;
    transform.m_direction.Normalize();

    auto delta = transform.m_direction * transform.m_velocity * dt;
    delta.y *= 0.5f;
    transform.m_prev_position = transform.m_position;
    transform.m_position += delta;
    transform.m_prev_direction = transform.m_direction;
    transform.m_direction = {0,0};
  }

  void Movement::ApplyRun(void)
  {
    if(this->m_state == MovementState::Running || this->m_state == MovementState::Cooldown)
      return;
    this->m_state = MovementState::Running;
  }

  void Movement::UpdateMovement(ENG::ITransform& transform, float dt, bool entityDashing) 
  {
    if (entityDashing)
      return;

    switch (this->m_state)
    {
      case MovementState::Walking:
        transform.m_velocity = this->m_walkingV;
        break;
      
      case MovementState::Running:
        transform.m_velocity = this->m_runningV;
        this->m_timer += dt;
        if (this->m_timer >= this->m_duration)
        {
          this->m_timer = 0.0f;
          this->m_state = MovementState::Cooldown;
          transform.m_velocity = this->m_walkingV;
        }
        break;

      case MovementState::Cooldown:
        this->m_timer += dt;
        if (this->m_timer >= this->m_cooldown)
        {
          this->m_timer = 0.0f;
          this->m_state = MovementState::Walking;
        }
        break;
    }
  }



}
