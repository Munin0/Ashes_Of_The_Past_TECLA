/// | ------------------------------------ |
#include "Functions.hpp"
/// | ------------------------------------ |
#include "Engine/Services/ScenesManager.hpp"
#include "Engine/Services/Services.hpp"
/// | ------------------------------------ |
#include <any>
#include <string>
/// | ------------------------------------ |


namespace APP
{
  void ChangeSceneButton(std::any _)
  {
    if(auto* idScene = std::any_cast<std::string>(&_))
    {
      ENG::Services::Scenes().PedingScene(*idScene);
    }
  }
  
  void ExitGameButton(std::any _)
  {
    if(auto* p = std::any_cast<bool>(&_))
    {
      *p = !*p;
    }
  }
}
