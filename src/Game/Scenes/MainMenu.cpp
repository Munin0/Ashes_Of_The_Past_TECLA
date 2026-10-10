/// | ------------------------------------ |
#include "MainMenu.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/GUI/Button.hpp"
#include "Engine/Inputs/Mouse.hpp"
#include "Engine/Layer/Scene.hpp"
#include "Engine/Map/TileMap/TileMap.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Physics/Collision.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Render/Color/RColor.hpp"
#include "Engine/Render/Render.hpp"
#include "Engine/Text/Text.hpp"
#include "Engine/Text/TextAPI.hpp"
#include "Engine/Utils/Log.hpp"
#include "Engine/Utils/Vector2.hpp"
#include "Engine/Object/ObjectPool.hpp"
#include "Engine/Inputs/PollEvent.hpp"
#include "Engine/Services/ScenesManager.hpp"
#include "Engine/Services/Services.hpp"
  /// | ------------------------------------ |
#include "Game/Game.hpp"
#include "Game/Systems/Functions.hpp"
  /// | ------------------------------------ |
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
  /// | ------------------------------------ |
#include <cmath>
#include <memory>
#include <string>
  /// | ------------------------------------ |

  namespace APP
  {
    MainMenu::MainMenu(const std::string& id)
      : ENG::Scene{}
    {
      sceneID = id;
    }

    void MainMenu::Init(void)
    {
      if(isInit)
      {
        LOG_INFO(" | << SCENE MAINMENU ALREADY INIT");
        return;
      }
      // ###############################
      /// Screen Size
      auto sizeScreen = ENG::Render::Get().GetScreenSize();
      auto f_CabinItalic = ENG::Services::Fonts().GetFont("CabinItalic");
      auto f_Ithaca = ENG::Services::Fonts().GetFont("Ithaca");

      // ###############################
      // Objects
      // Configuration of the Object entities
      // ###############################
      ENG::ObjectID btStartID = pool.Add(std::make_unique<ENG::Button>("Button_Play", ENG::Vector2{300,150} ));
      auto startBT = static_cast<ENG::Button*>(pool.Get(btStartID));
      startBT->SetPosition({sizeScreen.x / 2.0f - 150.0f, sizeScreen.y / 2.0f});
      startBT->SetData(std::string("DemoScene"));
      startBT->SetFunction(ChangeSceneButton);
      startBT->SetLayer(LAYER_UI);
      startBT->SetTexture("GUI", "Btn_Gray_Normal");
      startBT->SetFont(*f_Ithaca);
      startBT->SetText("Start Game");
      startBT->SetTextSize(48);
      startBT->SetTextColor(ENG::Color::Black);


      ENG::ObjectID btExitID = pool.Add(std::make_unique<ENG::Button>("Button_Exit", ENG::Vector2{300,150} ));
      auto exitBT = static_cast<ENG::Button*>(pool.Get(btExitID));
      exitBT->SetPosition({sizeScreen.x / 2.0f - 150.0f, sizeScreen.y / 2.0f + 200.f});
      exitBT->SetData(&this->isRunning);
      exitBT->SetFunction(ExitGameButton);
      exitBT->SetLayer(LAYER_UI);
      exitBT->SetTexture("GUI", "Btn_Gray_Normal");
      exitBT->SetFont(*f_Ithaca);
      exitBT->SetText("Exit Game");
      exitBT->SetTextSize(48);
      exitBT->SetTextColor(ENG::Color::Black);
      // ###############################
      // Objects Text
      // Configuration of the Object Text
      // ###############################
      auto v = ENG::TextAPI::Get().GetMeasureTextEx(f_Ithaca, "Ashes of the past", 120.f);
      ENG::ObjectID textID = pool.Add(std::make_unique<ENG::Text>(ENG::Vector2{sizeScreen.x / 2.f - v.x / 2.0f, 300.f}, 120.0f, "Ashes of the past"));
      auto* textObj = static_cast<ENG::Text*>(pool.Get(textID));
      textObj->SetFont(f_Ithaca);
      textObj->AddComponent<ENG::IColor>(ENG::Color::Black);

      // ###############################
      // Objects Label
      // Configuration of the Object Labels
      // ###############################
      // ENG::ObjectID labelID = pool.Add(std::make_unique<ENG::Label>("IniciarPrueba", ENG::Vector2{250,150}));
      // auto labelObj = static_cast<ENG::Label*>(pool.Get(labelID));
      // labelObj->SetFont(*f_CabinItalic);
      // labelObj->SetText("Iniciar Partida");
      // labelObj->SetPosition({sizeScreen.x/2.0f - 250/2.0f,sizeScreen.y/2.0f - 150/2.0f});
      // labelObj->SetFontSize(64);
      // labelObj->SetOffset({5.0f,5.0f});
      // labelObj->SetTextAlign(ENG::AlignText::TEXT_ALIGN_CENTER);
      // labelObj->SetColorText(ENG::Color::Black);
      // labelObj->SetBackgoundColor(ENG::Color::White);

      // ###############################
      // Final configurations
      // Final configurations for the GameLayer
      // ###############################
      isInit = true;
      isRunning = true;
      this->renderQueue = pool.Sort();
    }

    void MainMenu::Destroy(void)
    {
      pool.Clear();
      renderQueue.clear();
      isInit = false;
      isRunning = false;
    }

    bool MainMenu::IsRunning()
    {
      return isRunning;
    }

    void MainMenu::Inputs(float dt)
    {
      auto& pollEvent = ENG::PollEvent::Get();
      
      if(pollEvent.IsKeyPress(SDL_SCANCODE_P))
      {   
        mousePressed = false;
        isRunning = false;
        ENG::Services::Scenes().PedingScene("DemoScene");
      }

      if(pollEvent.IsMouseButtonPress(SDL_BUTTON_LEFT))
      {
        mousePressed = true;
      }
    }

    void MainMenu::Update(float dt)
    {
      for (auto& o : pool.GetAllIDs())
      {
        pool.Get(o)->Update(dt);

        if(auto b = dynamic_cast<ENG::Button*>(pool.Get(o)))
        {
          if(auto resolution = ENG::CollisionPointRect(ENG::GetMousePosition(), b->GetRect()))
          {
            if(resolution)
              b->SetTexture("GUI", "Btn_Gray_Hover");
            if (resolution && mousePressed)
            {
              b->Action();
              isRunning = false;
              mousePressed = false;
            }
          }
          else
            b->SetTexture("GUI", "Btn_Gray_Normal");
        }
      }
      mousePressed = false;
    }

    void MainMenu::UpdateFixed(float dt)
    {
      (void)dt;
    }

    void MainMenu::Render(ENG::Batcher& b)
    {
      for(auto& entry : renderQueue)
      {
        pool.Get(entry.id)->Draw(b);
      }
    }
  }
