/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
/// | ------------------------------------ |
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
/// | ------------------------------------ |

namespace APP
{
  enum class classEnemy
  {
    TYPE_NATIVE = 0,
    TYPE_MARINE = 1
  };

  class FactoryEnemies
  {
    public:
      FactoryEnemies(uint32_t maxEnemies, float maxTimeToSpawn, float step);
      ~FactoryEnemies() = default;

      std::unique_ptr<ENG::Object> CreateEnemy(const ENG::Rectangle& rect, const std::string& classEnemy);

      void UpdateFactoryEnemies();
      bool CanSpawnEnemy();

      /// @brief Total de enemigos creados (acumulado, usado como tope de spawn).
      uint32_t GetTotalEnemies(void) const {return this->m_countEnemies;}

      /// @brief Enemigos actualmente vivos (se decrementa al morir uno).
      uint32_t GetAliveEnemies(void) const {return this->m_aliveEnemies;}

      /// @brief Decrementa el contador de enemigos vivos. Llamar cuando muere
      ///        un enemigo.
      void DecreaseEnemyCount(void);

      /// @brief Ajusta la velocidad de spawn segun el puntaje del jugador:
      ///        cada ScorePerStep puntos, m_step sube StepIncrement. Asi el
      ///        spawn se acelera de forma gradual por niveles de puntaje.
      void UpdateSpawnStepFromScore(uint32_t score);

      /// @brief Velocidad de acumulacion actual del spawn.
      float GetStep(void) const {return this->m_step;}

      /// @brief Tiempo base de spawn (el que se pasa al constructor).
      float GetBaseTimeToSpawn(void) const {return this->m_maxTimeToSpawn;}

      void Clear(void);
    private:
      int ConvertClassToID(const std::string_view& s);
    private:
      /// @brief Puntaje necesario para subir un nivel de velocidad de spawn.
      static constexpr uint32_t ScorePerStep = 500;

      /// @brief Cuanto aumenta m_step por cada nivel de puntaje.
      static constexpr float StepIncrement = 0.1f;

      float m_step = 0.0f;
      float m_baseStep = 0.0f;
      float m_actualTime = 0.0f;
      float m_maxTimeToSpawn = 0.0f;

      uint32_t m_countEnemies = 0;
      uint32_t m_aliveEnemies = 0;
      uint32_t m_maxEnemies = 0;
  };
}
