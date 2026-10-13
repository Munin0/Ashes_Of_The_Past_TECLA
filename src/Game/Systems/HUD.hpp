#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Game/Systems/HighScores.hpp"
/// | ------------------------------------ |
#include <cstdint>
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] void EssenceBar(ENG::Batcher& batcher, const ENG::Camera2D& cam, ENG::Object& who);

  /**
   * @brief Draws the player counter panel (health, experience, damage,
   *        strength, dash and run timers) fixed to the top-left of the screen.
   */
  [[maybe_unused]] void PlayerHUD(ENG::Batcher& batcher, const ENG::Camera2D& cam, ENG::Object& who);

  /**
   * @brief Draws the score panel (enemies killed and score) fixed to the
   *        top-right of the screen.
   */
  [[maybe_unused]] void ScoreHUD(ENG::Batcher& batcher, const ENG::Camera2D& cam, uint32_t enemiesKilled, uint32_t score);

  /**
   * @brief Dibuja la tabla de las mejores puntuaciones pegada al borde
   *        superior derecho de la pantalla (coordenadas de pantalla).
   */
  [[maybe_unused]] void DrawHighScoresPanel(const HighScores& scores);
}
