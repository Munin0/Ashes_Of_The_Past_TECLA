/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Utils/Vector2.hpp"
#include "Engine/Component/Component.hpp"
/// | ------------------------------------ |

namespace APP
{
  struct Dash : public ENG::IComponents
  {
    enum class State
    {
      Ready,
      Dashing,
      Cooldown
    };

    State state = State::Ready;
    ENG::Vector2 direction = {0, 0};
    ENG::Vector2 normalVelocity = {0, 0};
    float timer = 0.0f;
    float duration = 0.5f;
    float cooldown = 5.0f;
    float speedMultiplier = 2.8f;
  };
}
