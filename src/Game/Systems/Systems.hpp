/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Vector2.hpp"
#include <memory>
#include <string>
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] void System_PlayerMovement(ENG::Object& o, float dt);
  void TogglePlayerRunning(ENG::Object& player);
  void TryStartPlayerDash(ENG::Object& player);
  void System_PlayerDash(ENG::Object& player, float dt);

  // Get were is looking the player with the Direction.
  [[maybe_unused]] std::string GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction);

  [[maybe_unused]] std::unique_ptr<ENG::Object> CreateEntity(const ENG::Rectangle& rect, const std::string& entityName, const std::string& entityClass); 

  [[maybe_unused]] ENG::Vector2 GetRandomPosition(const ENG::Rectangle& where);
}
