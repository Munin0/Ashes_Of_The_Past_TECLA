/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Vector2.hpp"
#include <memory>
#include <string>
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] void System_PlayerMovement(ENG::Object& o, float dt);
  
  [[maybe_unused]] void TogglePlayerRunning(ENG::Object& player);
  [[maybe_unused]] void TryStartPlayerDash(ENG::Object& player);
  [[maybe_unused]] void System_PlayerDash(ENG::Object& player, float dt);
  
  [[maybe_unused]] void System_AumentLevel(ENG::Object& who, uint32_t xp);

  [[maybe_unused]] void System_AIMovement(ENG::Object& target, ENG::Object& source, float dt);

  [[maybe_unused]] std::string System_GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction);

  [[maybe_unused]] std::unique_ptr<ENG::Object> System_CreateEntity(const ENG::Rectangle& rect, const std::string& entityName, const std::string& entityClass); 

  [[maybe_unused]] ENG::Vector2 System_GetRandomPosition(const ENG::Rectangle& where);
}
