/// | ------------------------------------ |
#include "Demo.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Inputs/Mouse.hpp"
#include "Engine/Render/Batching/RAPIBatch.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Engine/Render/Color/RColor.hpp"
#include "Engine/Render/Render.hpp"
#include "Engine/Layer/Scene.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Object/ObjectPool.hpp"
#include "Engine/Map/TileMap/TileMap.hpp"
#include "Engine/Physics/Collision.hpp"
#include "Engine/Inputs/PollEvent.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Services/Services.hpp"
#include "Engine/Text/Font/Font.hpp"
#include "Engine/Text/TextAPI.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include "Game/FactoryEnemies/Factory.hpp"
#include "Game/Components/Shield.hpp"
#include "Game/Systems/NameEntry.hpp"
#include "Game/Systems/Systems.hpp"
#include "Game/Components/Dash.hpp"
#include "Game/Components/Heal.hpp"
#include "Game/Components/Movement.hpp"
#include "Game/Systems/HUD.hpp"
#include "Game/Components/Stats.hpp"
/// | ------------------------------------ |
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
/// | ------------------------------------ |
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
/// | ------------------------------------|
#define PLAYER 1
/// | ------------------------------------|

namespace APP
{
  DemoScene::DemoScene(const std::string& id)
    : ENG::Scene{}, factoryEnemies{1000, 5.0f, 0.2f}
  {
    sceneID = id;
  }

  void DemoScene::Init()
  {
    if (isInit)
      return;
    
    // ###############################
    // Objects  
    // Configuration of the Object entities 
    // ###############################
    auto idPlayer = pool.Add(std::make_unique<ENG::Object>("Player"));
    auto objPlayer = pool.Get(idPlayer);
    objPlayer->GetStats().m_hp = 200;
    objPlayer->GetStats().m_hp_max = 200;
    objPlayer->GetStats().m_essence = 100;
    objPlayer->GetStats().m_essence_max = 100;
    
    objPlayer->SetPosition(ENG::Vector2{30,50});
    
    objPlayer->AddComponent<ENG::ISprite>("Player", "IddleNE", 0, 1.0f);
    objPlayer->AddComponent<ENG::IAnimator>("Player", "IddleNE", 10.f, 1, 1.0f);
    objPlayer->GetComponent<ENG::IAnimator>()->Play();
    
    objPlayer->AddComponent<ENG::IBoundingBox>(ENG::Vector2{32,32});

    objPlayer->AddComponent<HealtStat>();
    objPlayer->AddComponent<DefenseStat>();
    objPlayer->AddComponent<StrengthStat>();
    objPlayer->AddComponent<XPStat>();
    
    objPlayer->AddComponent<MeleeAttackDistance>();
    objPlayer->AddComponent<MeleeAttackRange>();
    objPlayer->AddComponent<MeleeAttackVelocity>();

    objPlayer->AddComponent<Shield>(1.0f, 5.0f, 100);
    objPlayer->AddComponent<Dash>(5.0f, 2.0f, .5f);
    objPlayer->AddComponent<Heal>(8.0f, 20);

    objPlayer->AddComponent<Movement>(ENG::Vector2{150.f,150.f}, ENG::Vector2{250.f,250.f}, 2.0f, .5f, 5.0f);
    objPlayer->GetTransform().m_direction = {0,0};
    
    objPlayer->GetTransform().m_prev_direction = {0 , 1.f};
    // ###############################
    // Tilemap 
    // Configuration of the Tilemaps
    // ###############################
    // Tile map (Tiled JSON export, atlas resolved by tile "id" == gid - firstgid)
    ENG::Services::Assets().LoadAtlas("Atlas/Isometrico/CaveMap.json", "I_CaveMap");
    for (auto& mapLayer : ENG::TileMap::LoadTiledMap("I_CaveMap", "Map/Scenes/expIsometrico.json"))
    {
      AddTileMap(std::move(mapLayer));
    }

    // ###############################
    // Camera  
    // Configuration of the camera
    // ###############################
    auto r = ENG::Render::Get().GetScreenSize();
    cam = std::make_unique<ENG::Camera2D>(r.x, r.y);
    cam->SetTarget(&objPlayer->GetTransform());
    cam->SetPosition(objPlayer->GetPosition());
    cam->SetZoom(1.5f);
    ENG::Render::Get().GetBatcher().SetCamera2D(cam.get());
   
    // ###############################
    //  audio
    //  loading audios part
    // ###############################
    // SDL_PropertiesID options;
    // options = SDL_CreateProperties();
    // SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    // ENG::Services::Music().LoadMusic("Music/Ambient_MainMenu.mp3", "Ambient_MainMenu", options);
    // ENG::Services::Music().SetVolume("Marine", 10.0f);
    // ENG::Services::Music().PlayMusic("Marine");

    // ###############################
    // Final
    // Final configurations
    // ###############################
    m_enemiesKilled = 0;
    m_score = 0;
    m_playerHitCooldown = 0.0f;
    nameEntry = APP::NameEntry{};

    isInit = true;
    isRunning = true;
    this->renderQueue = pool.Sort();
  }

  void DemoScene::Inputs(float dt)
  {
    auto& pollEvent = ENG::PollEvent::Get();

    /// Con el menu de fin de partida abierto, el control le pertenece a el.
    if(nameEntry.IsActive())
    {
      nameEntry.Input(dt);
      if(nameEntry.WantsExit())
      {
        isRunning = false;
        ENG::Services::Scenes().PedingScene("MainMenu");
      }
      return;
    }

    auto player = pool.Get(PLAYER);
    auto& transform = player->GetTransform();
    auto* animator = player->GetComponent<ENG::IAnimator>();
    auto* movement = player->GetComponent<APP::Movement>();
    auto* dash = player->GetComponent<APP::Dash>();
    auto poolIDs = pool.GetAllIDs();

    if(pollEvent.IsMouseButtonDown(SDL_BUTTON_RIGHT))
    {
      player->GetComponent<Shield>()->ActiveShield();
    }

    ENG::Vector2 inputDir{0.f, 0.f};
    if (pollEvent.IsKeyDown(SDL_SCANCODE_W))
      inputDir.y -= 1.0f;
    if (pollEvent.IsKeyDown(SDL_SCANCODE_S))
      inputDir.y += 1.0f;
    if (pollEvent.IsKeyDown(SDL_SCANCODE_A))
      inputDir.x -= 1.0f;
    if (pollEvent.IsKeyDown(SDL_SCANCODE_D))
      inputDir.x += 1.0f;
    transform.m_direction = inputDir;

    const bool moving = inputDir.x != 0.f || inputDir.y != 0.f;

    if (moving)
    {
      transform.m_prev_direction = transform.m_direction;
    }

    if (pollEvent.IsKeyPress(SDL_SCANCODE_LSHIFT))
    {
      movement->ApplyRun();
    }

    if (pollEvent.IsKeyPress(SDL_SCANCODE_SPACE))
    {
      dash->TryDash(player->GetTransform());
    }

    if (pollEvent.IsKeyPress(SDL_SCANCODE_H))
    {
      player->GetComponent<Heal>()->TryHeal(*player);
    }

    if (dash->GetState() == DashState::Dashing)
    {
      animator->SetAnimation(System_GetAnimationLooking("Walk", dash->GetDirecction()));
      animator->Resume();
      return;
    }

    if (moving)
    {
      if (movement->IsRunning())
      {
        animator->SetAnimation(System_GetAnimationLooking("Run", player->GetTransform().m_direction));
      }
      else
      {
        animator->SetAnimation(System_GetAnimationLooking("Walk", player->GetTransform().m_direction));
      }
      animator->Resume();
    }
    else
    {
      animator->SetAnimation(System_GetAnimationLooking("Iddle", player->GetTransform().m_prev_direction));
      animator->Resume();
    }
    ////////////////////////////////////////


    //////////////////Attacks//////////////////////
    if(pollEvent.IsKeyPress(SDL_SCANCODE_J))
    {
      System_MeleeAttack(*player, poolIDs, pool);
    }

    //////////////////Attacks//////////////////////

    float wheel = pollEvent.GetMouseWheel();
    if (wheel != 0.0f)
    {
      float zoom = cam->GetZoom() + wheel * 0.1f;
      cam->SetZoom(std::clamp(zoom, 1.5f, 2.0f));
    }

  }

  void DemoScene::Update(float dt)
  {
    /// Con el jugador muerto la partida queda congelada: solo importa el
    /// ingreso del nombre y el guardado del puntaje.
    if(nameEntry.IsActive())
      return;

    /// Cada 1000 puntos de puntaje sube un poco la velocidad de spawn.
    factoryEnemies.UpdateSpawnStepFromScore(m_score);

    /// Adding Enemies per frame
    if(factoryEnemies.CanSpawnEnemy())
    {
      pool.Add(factoryEnemies.CreateEnemy(this->cam->GetRectCamera(),"Native"));
      this->renderQueue = pool.Sort();
    }

    /// IDs de objetos muertos a eliminar al terminar el bucle: no se puede
    /// modificar el pool mientras se itera sobre GetAllIDs().
    std::vector<ENG::ObjectID> deadIDs;

    /// Objects Update
    for (auto& o : pool.GetAllIDs())
    {
      auto obj = pool.Get(o);

      if(auto* hs = obj->GetComponent<APP::HealtStat>())
      {
        if(hs->isDead())
        {
          if(o != PLAYER)
          {
            factoryEnemies.DecreaseEnemyCount();

            if(obj->GetName().rfind("Native", 0) == 0)
            {
              ++m_enemiesKilled;
              m_score += 1u + 10;
              if(auto* player = pool.Get(PLAYER))
              {
                System_AumentLevel(*player, 10);
              }
            }
            deadIDs.push_back(o);
          }
          continue;
        }
      }

      if(obj->GetName() == "Player")
      {
        auto d = obj->GetComponent<Dash>();
        auto mv = obj->GetComponent<Movement>();
        auto heal = obj->GetComponent<Heal>();

        d->UpdateDash(obj->GetTransform(), dt);
        mv->ApplyMovement(obj->GetTransform(), dt);
        mv->UpdateMovement(obj->GetTransform(), dt, d->IsDashing());
        heal->UpdateHeal(dt);
      }

      System_AIMovement(*pool.Get(PLAYER),*obj, dt);
      obj->Update(dt);

      for (auto& map : tileMaps)
      {
        if (map.IsCollisionLayer() && ENG::CollisionTileMapIsometric(*pool.Get(PLAYER), map, true))
        {
          break;
        }
      }
    }

    if(!deadIDs.empty())
    {
      for(ENG::ObjectID id : deadIDs)
      {
        pool.Remove(id);
      }
      this->renderQueue = pool.Sort();
    }

    /// Enemigos que tocan al jugador le bajan la vida, con una cadencia para
    /// que no drene cada frame.
    m_playerHitCooldown -= dt;
    if(m_playerHitCooldown <= 0.0f)
    {
      if(auto* player = pool.Get(PLAYER))
      {
        if(System_EnemiesAttackPlayer(*player, pool, 5.f))
        {
          m_playerHitCooldown = 0.5f;
        }
      }
    }

    /// Si el jugador murio, se abre el menu de ingreso de nombre.
    if(auto* player = pool.Get(PLAYER))
    {
      if(auto* hs = player->GetComponent<HealtStat>(); hs && hs->isDead())
        nameEntry.Open(m_score);
    }

    /// Update all Modules
    factoryEnemies.UpdateFactoryEnemies();
    cam->Update(dt);
  }

  void DemoScene::UpdateFixed(float dt)
  {
    (void)dt;
  }

  void DemoScene::Render(ENG::Batcher& b)
  {
    RenderTileMaps(b, LAYER_GROUND, LAYER_PLAYER);      /// Layer back of the player
    for (auto& entry : renderQueue)
    {
      auto* obj = pool.Get(entry.id);
      if(!obj)
        continue;
      auto* hs = obj->GetComponent<HealtStat>();
      if(hs && hs->isDead())
        continue;
      obj->Draw(b);
    }
    RenderTileMaps(b, LAYER_PLAYER, LAYER_MAX);    /// Layer front of the player

    if (auto* player = pool.Get(PLAYER))
      System_DrawMeleeDebug(*player, pool.GetAllIDs(), pool);

    // ENG::Drawer::DrawRectangleOutline(cam->GetRectCamera(),ENG::Color::Blue); 
    // if (auto* player = pool.Get(PLAYER))
    // {
    //   if (auto* box = player->GetComponent<ENG::IBoundingBox>())
    //   {
    //     const auto sz = box->GetSize();
    //     float fw = sz.x * 0.4f, fh = std::max(2.0f, sz.y * 0.12f);
    //     ENG::Drawer::DrawRectangleOutline(
    //         ENG::Vector2{box->GetPosition().x + (sz.x - fw) * 0.5f, box->GetPosition().y + sz.y - fh},
    //         ENG::Vector2{fw, fh}, ENG::Color::Red);
    //
    //     ENG::Drawer::DrawRectangleOutline(
    //         box->GetPosition(),
    //         box->GetSize(), ENG::Color::Yellow);
    //
    //   }
    // }

    const float   hudTextX = 30.f;
    float         hudTextY = 120.f;
    const float   hudLineH = 24.f;
    const ENG::Font* hudFont = ENG::Services::Fonts().GetFont("Ithaca");
    auto drawHudLine = [&](const std::string& str)
    {
      ENG::TextAPI::Get().DrawText(*hudFont, hudTextX, hudTextY, 16.f, str);
      hudTextY += hudLineH;
    };

    drawHudLine("WASD para moverte");
    drawHudLine("Shift para correr");
    drawHudLine("Espacio para Dash");
    drawHudLine("H para curarte");
    drawHudLine("J para golpear enemigos");
    drawHudLine(ENG::TextAPI::Get().FormatText("Enemigos vivos: %u" , factoryEnemies.GetAliveEnemies()));
    drawHudLine(ENG::TextAPI::Get().FormatText("FPS: %.2f" , ENG::Services::Clock().GetFPS()));
    EssenceBar(b, *cam, *pool.Get(PLAYER));
    PlayerHUD(b, *cam, *pool.Get(PLAYER));
    ScoreHUD(b, *cam, m_enemiesKilled, m_score);

    if(nameEntry.IsActive())
      nameEntry.Render(*cam);
  }

  void DemoScene::Destroy()
  {
    pool.Clear();
    renderQueue.clear();
    isInit = false;
    isRunning = false;
    tileMaps.clear();
    factoryEnemies.Clear();
  }

  bool DemoScene::IsRunning()
  {
    return isRunning;
  }
} // namespace APPProperties for the layer of the map
