/// | ------------------------------------ |
#include "Demo.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Render/Batching/RAPIBatch.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Engine/Render/Color/RColor.hpp"
#include "Engine/Render/Render.hpp"
/// | ------------------------------------ |
#include "Engine/Layer/Scene.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Object/ObjectPool.hpp"
#include "Engine/Map/TileMap/TileMap.hpp"
#include "Engine/Inputs/PollEvent.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Services/ScenesManager.hpp"
#include "Engine/Services/Services.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include "Game/Systems/Systems.hpp"
#include "SDL3/SDL_properties.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3_mixer/SDL_mixer.h"
/// | ------------------------------------ |
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
/// | ------------------------------------|
#define PLAYER 1
/// | ------------------------------------|

namespace APP
{
  DemoScene::DemoScene(const std::string& id) : ENG::Scene{}
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
    objPlayer->SetPosition(ENG::Vector2{500,500});
    objPlayer->GetTransform().m_velocity = {1000,1000};
    objPlayer->AddComponent<ENG::ISprite>("Player", "IddleS", 0, 1.0f);
    objPlayer->AddComponent<ENG::IAnimator>("Player", "IddleS", 10.f, 1, 1.0f);
    objPlayer->GetComponent<ENG::IAnimator>()->Play();
    objPlayer->AddComponent<ENG::IBoundingBox>(ENG::Vector2{64,64});
    
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
    cam->SetZoom(1.0f);
    ENG::Render::Get().GetBatcher().SetCamera2D(cam.get());
   
    // ###############################
    //  audio
    //  loading audios part
    // ###############################

    /// option for this music
    SDL_PropertiesID options;
    options = SDL_CreateProperties();
    SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    ENG::Services::Music().LoadMusic("Music/MarineHoloLive.mp3", "Marine", options);
    // ENG::Services::Music().SetVolume("Marine", 10.0f);
    // ENG::Services::Music().PlayMusic("Marine");

    ENG::Services::SFX().LoadSFX("SFX/Shot_Gun.mp3", "GunShot", 2);
    ENG::Services::SFX().SetVolume("GunShot", 10.0f);

    // ###############################
    // Final
    // Final configurations
    // ###############################
    isInit = true;
    isRunning = true;
    this->renderQueue = pool.Sort();
  }

  void DemoScene::Inputs(float dt)
  {
    auto& pollEvent = ENG::PollEvent::Get();
    auto player = pool.Get(PLAYER);
    auto& transform = player->GetTransform();

    if (pollEvent.IsKeyPress(SDL_SCANCODE_P))
    {
      isRunning = false;
      ENG::Services::Scenes().PedingScene("MainMenu");
      ENG::Services::Music().StopMusic("Marine");
    }

    if (pollEvent.IsKeyPress(SDL_SCANCODE_L))
    {
      pool.Add(CreateEntity(cam->GetRectCamera(), "Native", "Native"));
      this->renderQueue = pool.Sort();
    }

    if (pollEvent.IsKeyDown(SDL_SCANCODE_W))
    {
      transform.m_direction.y -= 1.0f;
      if(pollEvent.IsKeyDown(SDL_SCANCODE_A))
      {
        transform.m_direction.x -= 1.0f;
      }
      else if (pollEvent.IsKeyDown(SDL_SCANCODE_D))
      {
        transform.m_direction.x += 1.0f;
      }
      player->GetComponent<ENG::IAnimator>()->SetAnimation(GetAnimationLooking("Walk", transform.m_direction));
      player->GetComponent<ENG::IAnimator>()->Resume();
    }
    else if (pollEvent.IsKeyDown(SDL_SCANCODE_S))
    {
      transform.m_direction.y += 1.0f;
      if(pollEvent.IsKeyDown(SDL_SCANCODE_A))
      {
        transform.m_direction.x -= 1.0f;
      }
      else if(pollEvent.IsKeyDown(SDL_SCANCODE_D))
      {
        transform.m_direction.x += 1.0f;
      }
      player->GetComponent<ENG::IAnimator>()->SetAnimation(GetAnimationLooking("Walk", transform.m_direction));
      player->GetComponent<ENG::IAnimator>()->Resume();
    }
    else if (pollEvent.IsKeyDown(SDL_SCANCODE_A))
    {
      transform.m_direction.x -= 1.0f;
      player->GetComponent<ENG::IAnimator>()->SetAnimation(GetAnimationLooking("Walk", transform.m_direction));
      player->GetComponent<ENG::IAnimator>()->Resume();
    }
    else if (pollEvent.IsKeyDown(SDL_SCANCODE_D))
    {
      transform.m_direction.x += 1.0f;
      player->GetComponent<ENG::IAnimator>()->SetAnimation(GetAnimationLooking("Walk", transform.m_direction));
      player->GetComponent<ENG::IAnimator>()->Resume();
    }
    if(player->GetTransform().m_direction == ENG::Vector2{0.f,0.f})
    {
      player->GetComponent<ENG::IAnimator>()->Stop();
    }

    float wheel = pollEvent.GetMouseWheel();
    if (wheel != 0.0f)
    {
      float zoom = cam->GetZoom() + wheel * 0.1f;
      cam->SetZoom(std::clamp(zoom, 1.5f, 2.0f));
    }

  }

  void DemoScene::Update(float dt)
  {
    for (auto& o : pool.GetAllIDs())
    {
      auto obj = pool.Get(o);
      if(obj->GetName() == "Player")
      {
        System_PlayerMovement(*obj, dt);
      }
      obj->Update(dt);
    }
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
      obj->Draw(b);
    }
    RenderTileMaps(b, LAYER_PLAYER, LAYER_MAX);    /// Layer front of the player

    ENG::Drawer::DrawRectangleOutline(cam->GetRectCamera(),ENG::Color::Blue);
    
  }

  void DemoScene::Destroy()
  {
    pool.Clear();
    renderQueue.clear();
    isInit = false;
    isRunning = false;
    tileMaps.clear();
  }

  bool DemoScene::IsRunning()
  {
    return isRunning;
  }
} // namespace APPProperties for the layer of the map
