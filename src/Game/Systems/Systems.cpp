/// | ------------------------------------ |
#include "Systems.hpp"
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
/// | ------------------------------------ |
#include "Engine/Utils/Log.hpp"
/// | ------------------------------------ |
#include <string>
/// | ------------------------------------ |

namespace APP
{
  constexpr uint8_t MaxLevel = 255;
  constexpr int StandardThreshold = 1000;
  // Se define un valor maximo para la experiencia total y evitar el overflow
  constexpr uint32_t MaxXp = (StandardThreshold * (MaxLevel * (MaxLevel+1))/2);

  // Se declara la funcion estatica ya que solo se utilizara dentro de este archivo
  static uint32_t System_thresholdXp(uint8_t level)
  {
    return uint32_t(level * StandardThreshold);
  }

  void System_AumentLevel(ENG::Object& who, uint32_t xp)
  {
    auto& stats = who.GetStats();

    // Se asegura que la experiencia no supere el valor maximo permitido
    if(xp > MaxXp)  xp = MaxXp;
    // Se asegura que el nivel minimo sea 1
    if(stats.m_level == 0) stats.m_level = 1;
    stats.m_xp += xp;
    while(stats.m_xp >= System_thresholdXp(stats.m_level))
    {
      // Se verifica que el nivel no pase del maximo permitido
      if(stats.m_level == MaxLevel) break;
      stats.m_xp -= System_thresholdXp(stats.m_level);
      stats.m_level++;
      std::string texto = ("Se aumento el nivel del jugador a: " + std::to_string(stats.m_level) + " Actualmente tiene " + std::to_string(stats.m_xp) + " de experiencia");
      LOG_INFO(" | << " + texto);
    }
    // Se asegura que si el nivel es el maximo la xp sea el maximo
    if(stats.m_level == MaxLevel)
    {
      stats.m_xp = System_thresholdXp(MaxLevel);
    }
  }

  void System_PlayerMovement(ENG::Object& o, float dt)
  {
    auto& pPos = o.GetTransform();
    pPos.m_direction.Normalize();

    pPos.m_position += pPos.m_direction * pPos.m_velocity * dt;
    pPos.m_direction = {0,0};
  }
}
