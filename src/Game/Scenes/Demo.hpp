/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Layer/Scene.hpp"
#include "Engine/Object/ObjectPool.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Render/Batching/RenderEntry.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Game/FactoryEnemies/Factory.hpp"
#include "Game/Systems/NameEntry.hpp"
/// | ------------------------------------ |
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
/// | ------------------------------------ |

namespace APP
{
  class DemoScene : public ENG::Scene
  {
    public:
      DemoScene(const std::string& name);

      void Init(void)               override;
      void Destroy(void)            override;
      bool IsRunning(void)          override; 
      void Inputs(float dt)         override;
      void Update(float dt)         override;
      void UpdateFixed(float dt)    override;
      void Render(ENG::Batcher& b)  override;
    
    private:
      APP::FactoryEnemies factoryEnemies;
      APP::NameEntry      nameEntry;
      ENG::ObjectPool pool;
      std::vector<ENG::RenderEntry> renderQueue;
      std::unique_ptr<ENG::Camera2D> cam;

      uint32_t m_enemiesKilled = 0;
      uint32_t m_score = 0;
      float m_playerHitCooldown = 0.0f;
  };
}
