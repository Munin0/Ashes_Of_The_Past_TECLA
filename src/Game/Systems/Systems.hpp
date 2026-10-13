/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Object/ObjectPool.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include <string>
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] bool System_CollisionRectTriangle(const ENG::Rectangle& rect, const ENG::Triangle& triangle);
  
  [[maybe_unused]] void System_Aim(ENG::Object& entity, const ENG::Vector2& targetPosition);
  
  [[maybe_unused]] void System_PlayerTrabuco(ENG::Object& player);
  
  [[maybe_unused]] void System_AumentLevel(ENG::Object& who, uint32_t xp);

  [[maybe_unused]] void System_AIMovement(ENG::Object& target, ENG::Object& source, float dt);

  [[maybe_unused]] std::string System_GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction);

  [[maybe_unused]] ENG::Vector2 System_GetRandomPosition(const ENG::Rectangle& where);

  //Attack System
  [[maybe_unused]] bool System_MeleeAttack(ENG::Object& player, std::vector<uint32_t> allObjectsIds, ENG::ObjectPool& pool);
  
  [[maybe_unused]] void System_DrawMeleeDebug(ENG::Object& player, std::vector<uint32_t> allObjectsIds, ENG::ObjectPool& pool);
}
