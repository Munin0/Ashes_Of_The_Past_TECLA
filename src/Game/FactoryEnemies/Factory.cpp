/// | ------------------------------------ |
#include "Factory.hpp"
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Component/Component.hpp"
#include "Engine/Utils/Log.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include "Game/Systems/Systems.hpp"
#include "Game/Components/Stats.hpp"
/// | ------------------------------------ |
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
/// | ------------------------------------ |

namespace APP
{
  FactoryEnemies::FactoryEnemies(uint32_t maxEnemies, float maxTimeToSpawn, float step)
    : m_step{step}, m_baseStep{step},
      m_maxTimeToSpawn{maxTimeToSpawn}, m_maxEnemies{maxEnemies}
  { }
  
 std::unique_ptr<ENG::Object> FactoryEnemies::CreateEnemy(const ENG::Rectangle& rect, const std::string& classEnemy)
 {
    auto entity = std::make_unique<ENG::Object>(classEnemy);
    /// Example Enemy43
    entity->SetName(entity->GetName() + std::to_string(entity->GetCountObj())); 
    entity->GetTransform().m_position = System_GetRandomPosition(rect);
    entity->GetTransform().m_velocity = {100,100};

    if(classEnemy == "Native")
    {
      entity->SetClass(classEnemy);
      entity->AddComponent<ENG::ISprite>("Player", "IddleS", 0, 1.0f);
      entity->AddComponent<ENG::IAnimator>("Player", "IddleS", 10.f, 1, 1.0f);
      entity->GetComponent<ENG::IAnimator>()->Play();
    }
    entity->AddComponent<HealtStat>();
    entity->AddComponent<XPStat>();
    entity->AddComponent<ENG::IBoundingBox>(ENG::Vector2{32,32});
    // Give AI
    
    m_countEnemies++;
    m_aliveEnemies++;
    return std::move(entity);
  }

  void FactoryEnemies::DecreaseEnemyCount(void)
  {
    if(m_aliveEnemies > 0)
      --m_aliveEnemies;
  }

  void FactoryEnemies::UpdateSpawnStepFromScore(uint32_t score)
  {
    /// Por cada ScorePerStep puntos sube un nivel de velocidad.
    const uint32_t levels = score / ScorePerStep;
    m_step = m_baseStep + StepIncrement * static_cast<float>(levels);
  }

 void FactoryEnemies::UpdateFactoryEnemies()
 {
   if(m_actualTime < m_maxTimeToSpawn)
   {
     m_actualTime += m_step;
   }
   else
   {
    m_actualTime = 0.0f;
   }
 }

 bool FactoryEnemies::CanSpawnEnemy()
 {
   bool canByTime = false;
   if(this->m_actualTime > this->m_maxTimeToSpawn)
     canByTime = true;
    return m_countEnemies < m_maxEnemies && canByTime;
 }

 ////! Private Method
 int FactoryEnemies::ConvertClassToID(const std::string_view& s)
 {
   if(s == "Native")
     return 1;
   else if(s == "Marine")
     return 2;

   return 0;
 }


 void FactoryEnemies::Clear()
 {
    this->m_countEnemies = 0;
    this->m_aliveEnemies = 0;
 }
}
