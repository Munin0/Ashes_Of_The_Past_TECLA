#pragma once
#include "Engine/Component/Component.hpp"

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
    ENG::Vector2 walkingVelocity = {100, 100};
    ENG::Vector2 runningVelocity = {220, 220};
  };
}
