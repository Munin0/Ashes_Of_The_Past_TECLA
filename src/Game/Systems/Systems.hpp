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
#include <vector>
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] void System_Aim(ENG::Object& entity, const ENG::Vector2& targetPosition);
  
  [[maybe_unused]] void System_PlayerTrabuco(ENG::Object& player);
  
  [[maybe_unused]] void System_AumentLevel(ENG::Object& who, uint32_t xp);

  [[maybe_unused]] void System_AIMovement(ENG::Object& target, ENG::Object& source, float dt);

  [[maybe_unused]] std::string System_GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction);

  [[maybe_unused]] ENG::Vector2 System_GetRandomPosition(const ENG::Rectangle& where);

  // Devuelve cuanta esencia se sumo de verdad
  // si el maximo de esencia es 0 el objeto no tiene esencia y no se suma nada
  [[maybe_unused]] uint32_t System_AddEssence(ENG::Object& who, uint32_t essence);

  // Devuelve un true si se pudo usar la esencia y false si no se pudo
  [[maybe_unused]] bool System_RemoveEssence(ENG::Object& who, uint32_t essence);

  // Devuelve true si hay suficiente esencia para usarse y false si no es asi
  [[maybe_unused]] bool System_CheckEssence(ENG::Object& who, uint32_t essence);
  //Attack System
  [[maybe_unused]] bool System_MeleeAttack(ENG::Object& player, std::vector<uint32_t> allObjectsIds, ENG::ObjectPool& pool);

  /// @brief Aplica @p damage a @p player por cada enemigo que lo toque
  ///        (solape de bounding boxes). El dano pasa por el Shield si lo
  ///        tiene. Devuelve true si al menos un enemigo ataco.
  [[maybe_unused]] bool System_EnemiesAttackPlayer(ENG::Object& player, ENG::ObjectPool& pool, uint8_t damage);
  
  [[maybe_unused]] void System_DrawMeleeDebug(ENG::Object& player, std::vector<uint32_t> allObjectsIds, ENG::ObjectPool& pool);

  [[maybe_unused]] bool System_GetMeleeAttackTriangle(ENG::Object& player, ENG::Triangle& attackTriangle);

}
