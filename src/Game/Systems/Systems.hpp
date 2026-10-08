/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
/// | ------------------------------------ |
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] void System_PlayerMovement(ENG::Object& o, float dt);
  [[maybe_unused]] void System_AumentLevel(ENG::Object& who, uint32_t xp);
}
