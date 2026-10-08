/// | ------------------------------------ |
#include "Systems.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Game/Components/Dash.hpp"
#include "Game/Components/Movement.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Utils.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include <memory>
#include <string>
#include <utility>
/// | ------------------------------------ |

namespace APP
{
  void System_PlayerMovement(ENG::Object& o, float dt)
  {
    auto& pPos = o.GetTransform();
    pPos.m_direction.Normalize();

    auto delta = pPos.m_direction * pPos.m_velocity * dt;
    delta.y *= 0.5f;
    pPos.m_position += delta;
    pPos.m_direction = {0,0};
  }

  void TogglePlayerRunning(ENG::Object& player)
  {
    auto* movement = player.GetComponent<APP::Movement>();
    if (!movement)
      return;

    auto* dash = player.GetComponent<APP::Dash>();
    if (dash && dash->state == Dash::State::Dashing)
      return;

    auto& transform = player.GetTransform();
    switch (movement->state)
    {
      case Movement::State::Walking:
        movement->state = Movement::State::Running;
        transform.m_velocity = movement->runningVelocity;
        break;

      case Movement::State::Running:
        movement->state = Movement::State::Walking;
        transform.m_velocity = movement->walkingVelocity;
        break;
    }
  }

  void TryStartPlayerDash(ENG::Object& player)
  {
    auto* dash = player.GetComponent<APP::Dash>();
    if (!dash || dash->state != Dash::State::Ready)
      return;

    auto& transform = player.GetTransform();
    if (transform.m_direction.x == 0.0f &&
        transform.m_direction.y == 0.0f)
      return;

    dash->direction = transform.m_direction;
    dash->direction.Normalize();
    dash->normalVelocity = transform.m_velocity;
    dash->timer = 0.0f;
    dash->state = Dash::State::Dashing;
  }

  void System_PlayerDash(ENG::Object& player, float dt)
  {
    auto* dash = player.GetComponent<APP::Dash>();
    if (!dash)
      return;

    auto& transform = player.GetTransform();
    switch (dash->state)
    {
      case Dash::State::Ready:
        break;

      case Dash::State::Dashing:
        transform.m_velocity = dash->normalVelocity * dash->speedMultiplier;
        transform.m_direction = dash->direction;
        dash->timer += dt;

        if (dash->timer >= dash->duration)
        {
          transform.m_velocity = dash->normalVelocity;
          dash->timer = 0.0f;
          dash->state = Dash::State::Cooldown;
        }
        break;

      case Dash::State::Cooldown:
        dash->timer += dt;
        if (dash->timer >= dash->cooldown)
        {
          dash->timer = 0.0f;
          dash->state = Dash::State::Ready;
        }
        break;
    }
  }


  std::string GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction)
  {
    std::string animation = prefix;
    if(direction.x > 0 && direction.y == 0)           // East
      animation.push_back('E');
    else if(direction.x < 0 && direction.y == 0)      // West
      animation.push_back('W');

    if(direction.y < 0)                               // North
    {
      animation.push_back('N');
      if(direction.x > 0)
        animation.push_back('E');
      if(direction.x < 0)
        animation.push_back('W');
    }
    if(direction.y > 0)                               // South 
    {
      animation.push_back('S');
      if(direction.x > 0)
        animation.push_back('E');
      if(direction.x < 0)
        animation.push_back('W');
    }
    return animation;
  }

  std::unique_ptr<ENG::Object> CreateEntity(const ENG::Rectangle& rect, const std::string& entityName, const std::string& entityClass)
  {
    auto entity = std::make_unique<ENG::Object>(entityName);
    /// Example Enemy43
    entity->SetName(entityName + std::to_string(entity->GetCountObj())); 
    entity->GetTransform().m_position = GetRandomPosition(rect);
    entity->GetTransform().m_velocity = {100,100};

    if(entityClass == "Native")
    {
      entity->SetClass(entityClass);
      entity->AddComponent<ENG::ISprite>("Player", "IddleS", 0, 1.0f);
      entity->AddComponent<ENG::IAnimator>("Player", "IddleS", 10.f, 1, 1.0f);
      entity->GetComponent<ENG::IAnimator>()->Play();
    }
    entity->AddComponent<ENG::IBoundingBox>(ENG::Vector2{64,64});
    // Give AI
    
    return std::move(entity);
  }

  ENG::Vector2 GetRandomPosition(const ENG::Rectangle& where)
  {
    constexpr float kMargin = 128.f;
    ENG::Vector2 newPosition = {0, 0};
    const float right = where.x +  where.w;
    const float down = where.y + where.h;

    switch (ENG::GetRandomNumber(0, 4))
    {
      case 0: // U
        newPosition.x = (float)ENG::GetRandomNumber(where.x,right);
        newPosition.y = where.y - kMargin;
        break;
      case 1: // D
        newPosition.x = (float)ENG::GetRandomNumber(where.x, right);;
        newPosition.y = down + kMargin;
        break;
      case 2: // L
        newPosition.x = where.x - kMargin;
        newPosition.y = (float)ENG::GetRandomNumber(where.y, down);
        break;
      case 3: // R
        newPosition.x = right + kMargin;
        newPosition.y = (float)ENG::GetRandomNumber(where.y,down);
        break;
    }
    return newPosition;  

  }
}
