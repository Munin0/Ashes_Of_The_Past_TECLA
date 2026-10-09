/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |

namespace APP
{
  struct Movement : public ENG::IComponents
  {
    enum class State
    {
      Walking,
      Running
    };

    State state = State::Walking;
    float timer = 0.0f;
    float step = 0.0f;
    ENG::Vector2 walkingVelocity = {100, 100};
    ENG::Vector2 runningVelocity = {220, 220};
  };
}
