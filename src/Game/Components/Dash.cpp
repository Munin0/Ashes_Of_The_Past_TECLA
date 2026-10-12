/// | ------------------------------------ |
#include "Dash.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
/// | ------------------------------------ |

namespace APP
{
  void Dash::UpdateDash(ENG::ITransform& transform, float dt)
  {
    switch (this->m_state)
    {
      case DashState::Ready:
        break;

      case DashState::Dashing:
        transform.m_velocity = this->m_normalVelocity * this->m_speedMultiplier;
        transform.m_direction = this->m_direction;
        this->m_timer += dt;

        if (this->m_timer >= this->m_duration)
        {
          transform.m_velocity = this->m_normalVelocity;
          this->m_timer = 0.0f;
          this->m_state = DashState::Cooldown;
        }
        break;

      case DashState::Cooldown:
        this->m_timer += dt;
        if (this->m_timer >= this->m_cooldown)
        {
          this->m_timer = 0.0f;
          this->m_state = DashState::Ready;
        }
        break;
    }
  }
  
  void Dash::TryDash(ENG::ITransform& transform)
  {
    if (transform.m_direction.x == 0.0f && transform.m_direction.y == 0.0f)
      return;
    if(m_state == DashState::Dashing || m_state == DashState::Cooldown)
      return;

    this->m_direction = transform.m_direction;
    this->m_direction.Normalize();
    this->m_normalVelocity = transform.m_velocity;
    this->m_timer = 0.0f;
    this->m_state = DashState::Dashing;
  }

}
