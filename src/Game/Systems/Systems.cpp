/// | ------------------------------------ |
#include "Systems.hpp"
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/Vector2.hpp"
#include <string>
/// | ------------------------------------ |

namespace APP
{
  void System_PlayerMovement(ENG::Object& o, float dt)
  {
    auto& pPos = o.GetTransform();
    pPos.m_direction.Normalize();

    auto delta = pPos.m_direction * pPos.m_velocity * dt;
    delta.y *= 0.5f;
    pPos.m_position += delta;
    pPos.m_direction = {0,0};
  }

  std::string GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction)
  {
    std::string animation = prefix;
    if(direction.x > 0 && direction.y == 0)           // East
      animation.push_back('E');
    else if(direction.x < 0 && direction.y == 0)      // West
      animation.push_back('W');

    if(direction.y < 0)                               // North
    {
      animation.push_back('N');
      if(direction.x > 0)
        animation.push_back('E');
      if(direction.x < 0)
        animation.push_back('W');
    }
    if(direction.y > 0)                               // South 
    {
      animation.push_back('S');
      if(direction.x > 0)
        animation.push_back('E');
      if(direction.x < 0)
        animation.push_back('W');
    }
    return animation;
  }
}
