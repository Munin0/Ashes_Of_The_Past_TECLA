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
      uint32_t GetTotalEnemies(void) const {return this->m_countEnemies;}
    private:
      int ConvertClassToID(const std::string_view& s);
    private:
      float m_step = 0.0f;
      float m_actualTime = 0.0f;
      float m_maxTimeToSpawn = 0.0f;

      uint32_t m_countEnemies = 0;
      uint32_t m_maxEnemies = 0;
  };
}
