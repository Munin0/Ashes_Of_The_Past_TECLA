/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/Vector2.hpp"
#include <string>
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] void System_PlayerMovement(ENG::Object& o, float dt);

  // Get were is looking the player with the Direction.
  [[maybe_unused]] std::string GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction);
}
