/// | ------------------------------------ |
#include "Systems.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Game/Components/Aim.hpp"
#include "Game/Components/Dash.hpp"
#include "Game/Components/Movement.hpp"
#include "Game/Components/Shield.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Utils.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include <memory>
#include <string>
#include <utility>
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

  void System_Aim(ENG::Object& entity, const ENG::Vector2& targetPosition)
  {
    auto* aim = entity.GetComponent<APP::Aim>();
    if (!aim)
      return;

    auto direction = targetPosition - entity.GetPosition();
    constexpr float minimumAimDistance = 0.0001f; //seguridad
    if (direction.Length() <= minimumAimDistance)
      return;

    direction.Normalize(); 
    aim->direction = direction; //save direction//
  }

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

  void TogglePlayerShield(ENG::Object& player)
  {
    auto* shield = player.GetComponent<APP::Shield>();
    if (!shield || shield->state == Shield::State::Cooldown)
      return;

    if(shield->state == Shield::State::Active){
      shield->state = Shield::State::Ready;
      return;
    }

    if(shield->state == Shield::State::Ready){
      shield->state = Shield::State::Active;
    }
  }

  void System_PlayerShield(ENG::Object& player, float dt)
  {
    auto* shield = player.GetComponent<APP::Shield>();
    if(!shield)
      return;

    switch (shield->state)
    {
      case Shield::State::Ready:
        break;

      case Shield::State::Active: //Futura implementacion mecanica de escudo mientras esta activo
        break;

      case Shield::State::Cooldown:
        shield->timer += dt;
    
        if (shield->timer >= shield->cooldown)
        {
          shield->hp = shield->MaxHp;
          shield->timer = 0.0f;
          shield->state = Shield::State::Ready;
        }
        break;
    }
  }

  void ApplyPlayerDamage(ENG::Object& player, uint8_t damage, const ENG::Vector2& directionToSource){
    auto* shield = player.GetComponent<APP::Shield>();
    bool resultShieldDamage = false;

    if(shield && shield->state == Shield::State::Active)
      resultShieldDamage = TryBlockShield(player,damage,directionToSource);
    if(resultShieldDamage)
    {
      ApplyShieldDamage(player, damage);
      return;
    }

    if(damage >= player.GetStats().m_hp){
      player.GetStats().m_hp = 0;
    } else{
      player.GetStats().m_hp -= damage;
    }
  }


  bool TryBlockShield(ENG::Object& player, uint8_t damage, const ENG::Vector2& directionToSource){
      auto* shield = player.GetComponent<APP::Shield>();
      auto* aim = player.GetComponent<APP::Aim>();
      if(!aim || !shield || shield->state != Shield::State::Active)
        return false;

      auto incomingDirection = directionToSource;
      if (incomingDirection.Length() <= 0.0001f) // evitar normalizacion de vectores nulo.s
        return false;

      incomingDirection.Normalize();

      const float alignment =
        aim->direction.x * incomingDirection.x + 
        aim->direction.y * incomingDirection.y;

      if (alignment < shield->minimumBlockDot)
        return false;

      return true;
  }

  void ApplyShieldDamage(ENG::Object& player, uint8_t damage){
     auto* shield = player.GetComponent<APP::Shield>();
      if(!shield || shield->state != Shield::State::Active)
        return;
     if(damage >= shield->hp){
      shield->hp = 0;
      shield->timer = 0.0f;
      shield->state = Shield::State::Cooldown;
    } else {
      shield->hp -= damage;
    }
  }

  void System_PlayerTrabuco(ENG::Object& player){
    
  }


  void System_AIMovement(ENG::Object& target, ENG::Object& source, float dt)
  {
    if(&target == &source)
      return;
    
    auto* aSource = source.GetComponent<ENG::IAnimator>();

    ENG::Vector2 direction = target.GetTransform().m_position - source.GetTransform().m_position;
    float distance = direction.Length();

    if (distance > 0.0001f)
    {
      direction.Normalize();
      ENG::Vector2 movement = direction * source.GetTransform().m_velocity * dt;
      aSource->SetAnimation(GetAnimationLooking("Walk",  direction));

      source.GetTransform().m_position = source.GetTransform().m_position + movement;
    }
  }

  std::string GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction)
  {
    // Rotation sin.
    constexpr float T = 0.38f;
    std::string anim = prefix;

    if (direction.y < -T)      anim.push_back('N');
    else if (direction.y > T)  anim.push_back('S');

    if (direction.x > T)       anim.push_back('E');
    else if (direction.x < -T) anim.push_back('W');

    return anim;
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
