#pragma once
#include "Engine/Component/Component.hpp"

namespace APP
{
  struct Shield : public ENG::IComponents
  {
    enum class State
    {
      Ready,
      Active,
      Cooldown
    };

    State state = State::Ready;
    float timer = 0.0f;
    float cooldown = 5.0f;
    float minimumBlockDot = 0.5f; //bloqueo de 120 grados (60 grados cada lado), -1 contrario, 0 perpendicular y mas 1 es perfecto
    uint8_t hp = 100;
    uint8_t MaxHp = 100;
  };
}
