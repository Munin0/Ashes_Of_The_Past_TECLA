/// | ------------------------------------ |
#include "Systems.hpp"
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Game/Components/Dash.hpp"
#include "Game/Components/Movement.hpp"
#include "Game/Components/Stats.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Utils.hpp"
#include "Engine/Utils/Vector2.hpp"
#include "Engine/Render/Batching/RAPIBatch.hpp"
#include "Engine/Render/Color/RColor.hpp"
#include "Engine/Services/Services.hpp"
/// | ------------------------------------ |
#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
/// | ------------------------------------ |
#include "Engine/Utils/Log.hpp"
/// | ------------------------------------ |
#include <string>
#include <iostream>
/// | ------------------------------------ |

namespace APP
{
  bool System_CollisionRectTriangle(const ENG::Rectangle& rect, const ENG::Triangle& triangle)
  {
    const float left = std::min(rect.x, rect.x + rect.w);
    const float right = std::max(rect.x, rect.x + rect.w);
    const float top = std::min(rect.y, rect.y + rect.h);
    const float bottom = std::max(rect.y, rect.y + rect.h);

    const std::array<ENG::Vector2, 4> rectangleVertices = {{
      {left, top},
      {right, top},
      {right, bottom},
      {left, bottom}
    }};
    const std::array<ENG::Vector2, 3> triangleVertices = {{
      triangle.a,
      triangle.b,
      triangle.c
    }};

    const auto separatedOnAxis = [&](const ENG::Vector2& axis)
    {
      float rectMin = rectangleVertices[0].x * axis.x + rectangleVertices[0].y * axis.y;
      float rectMax = rectMin;
      for (const auto& vertex : rectangleVertices)
      {
        const float projection = vertex.x * axis.x + vertex.y * axis.y;
        rectMin = std::min(rectMin, projection);
        rectMax = std::max(rectMax, projection);
      }

      float triangleMin = triangleVertices[0].x * axis.x + triangleVertices[0].y * axis.y;
      float triangleMax = triangleMin;
      for (const auto& vertex : triangleVertices)
      {
        const float projection = vertex.x * axis.x + vertex.y * axis.y;
        triangleMin = std::min(triangleMin, projection);
        triangleMax = std::max(triangleMax, projection);
      }

      return rectMax < triangleMin || triangleMax < rectMin;
    };

    if (separatedOnAxis({1.0f, 0.0f}))
      return false;
    if (separatedOnAxis({0.0f, 1.0f}))
      return false;

    for (std::size_t i = 0; i < triangleVertices.size(); ++i)
    {
      const auto& start = triangleVertices[i];
      const auto& end = triangleVertices[(i + 1) % triangleVertices.size()];
      const ENG::Vector2 edge = end - start;
      const ENG::Vector2 normal{-edge.y, edge.x};
      if (separatedOnAxis(normal))
        return false;
    }

    return true;
  }

  constexpr uint8_t MaxLevel = 255;
  constexpr int StandardThreshold = 1000;
  // Se define un valor maximo para la experiencia total y evitar el overflow
  constexpr uint32_t MaxXp = (StandardThreshold * (MaxLevel * (MaxLevel+1))/2);

  // Se declara la funcion estatica ya que solo se utilizara dentro de este archivo
  static uint32_t System_thresholdXp(uint8_t level)
  {
    return uint32_t(level * StandardThreshold);
  }

  static bool System_GetMeleeAttackTriangle(ENG::Object& player, ENG::Triangle& attackTriangle)
  {
    auto* playerBoundingBox = player.GetComponent<ENG::IBoundingBox>();
    auto* animator = player.GetComponent<ENG::IAnimator>();
    auto* attackDistance = player.GetComponent<APP::MeleeAttackDistance>();
    auto* attackRange = player.GetComponent<APP::MeleeAttackRange>();
    if (!playerBoundingBox || !animator || !attackDistance || !attackRange)
      return false;

    constexpr float diagonalFactor = 0.7071f;
    const ENG::Vector2 playerPosition = player.GetTransform().m_position;
    const ENG::Vector2 playerSize = playerBoundingBox->GetSize();
    const ENG::Vector2 playerCenter{
      playerPosition.x + playerSize.x / 2.0f,
      playerPosition.y + playerSize.y / 2.0f
    };

    const std::string animationKey = animator->GetKeyAnimation();
    if (animationKey.size() < 4)
      return false;
    const std::string directionLooking = animationKey.substr(4);

    int currentDirection = 5;
    if (directionLooking == "N") currentDirection = 1;
    else if (directionLooking == "S") currentDirection = 5;
    else if (directionLooking == "E") currentDirection = 3;
    else if (directionLooking == "W") currentDirection = 7;
    else if (directionLooking == "NE") currentDirection = 2;
    else if (directionLooking == "SE") currentDirection = 4;
    else if (directionLooking == "NW") currentDirection = 0;
    else if (directionLooking == "SW") currentDirection = 6;

    const float attackDistanceValue = attackDistance->getCurrent();
    const float attackRangeValue = attackRange->getCurrent();
    const ENG::Vector2 directions[8] = {
      {-1, -1}, {0, -1}, {1, -1}, {1, 0},
      {1, 1}, {0, 1}, {-1, 1}, {-1, 0}
    };

    ENG::Vector2 attackVertices[3];
    attackVertices[0] = playerCenter;
    const bool isDiagonal =
      (std::abs(directions[currentDirection].x) +
       std::abs(directions[currentDirection].y)) == 2;

    if (isDiagonal)
    {
      const float distance = attackDistanceValue * diagonalFactor;
      const ENG::Vector2 tip{
        playerCenter.x + distance * directions[currentDirection].x,
        playerCenter.y + distance * directions[currentDirection].y
      };
      const float halfRange = (attackRangeValue / 2.0f) * diagonalFactor;
      const int firstSide = (currentDirection + 6) % 8;
      const int secondSide = (currentDirection + 2) % 8;

      attackVertices[1] = {
        tip.x + halfRange * directions[firstSide].x,
        tip.y + halfRange * directions[firstSide].y
      };
      attackVertices[2] = {
        tip.x + halfRange * directions[secondSide].x,
        tip.y + halfRange * directions[secondSide].y
      };
    }
    else
    {
      const float halfRange = attackRangeValue / 2.0f;
      const bool isVertical = directions[currentDirection].y != 0.0f;
      const ENG::Vector2 tip{
        playerCenter.x + attackDistanceValue * directions[currentDirection].x,
        playerCenter.y + attackDistanceValue * directions[currentDirection].y
      };

      attackVertices[1] = {
        tip.x + halfRange * (isVertical ? -1.0f : 0.0f),
        tip.y + halfRange * (isVertical ? 0.0f : -1.0f)
      };
      attackVertices[2] = {
        tip.x + halfRange * (isVertical ? 1.0f : 0.0f),
        tip.y + halfRange * (isVertical ? 0.0f : 1.0f)
      };
    }

    const ENG::Vector2 triangleCenter{
      (attackVertices[0].x + attackVertices[1].x + attackVertices[2].x) / 3.0f,
      (attackVertices[0].y + attackVertices[1].y + attackVertices[2].y) / 3.0f
    };
    attackTriangle = {
      attackVertices[0], attackVertices[1], attackVertices[2], triangleCenter
    };
    return true;
  }

  void System_DrawMeleeDebug(ENG::Object& player, std::vector<uint32_t> allObjectsIds, ENG::ObjectPool& pool)
  {
    ENG::Triangle attackTriangle;
    if (System_GetMeleeAttackTriangle(player, attackTriangle))
    {
      ENG::Drawer::DrawTriangleOutline(
        attackTriangle.a, attackTriangle.b, attackTriangle.c, ENG::Color::Red, 2.0f);
    }

    for (const uint32_t id : allObjectsIds)
    {
      auto* enemy = pool.Get(id);
      if (!enemy || enemy == &player || enemy->GetName().rfind("Native", 0) != 0)
        continue;

      auto* enemyBoundingBox = enemy->GetComponent<ENG::IBoundingBox>();
      auto* enemyHealth = enemy->GetComponent<APP::HealtStat>();
      if (!enemyBoundingBox || (enemyHealth && enemyHealth->isDead()))
        continue;

      const ENG::Vector2 enemySize = enemyBoundingBox->GetSize();
      const ENG::Vector2 enemyPosition = enemy->GetTransform().m_position;
      const float left = std::min(enemyPosition.x, enemyPosition.x + enemySize.x);
      const float right = std::max(enemyPosition.x, enemyPosition.x + enemySize.x);
      const float top = std::min(enemyPosition.y, enemyPosition.y + enemySize.y);
      const float bottom = std::max(enemyPosition.y, enemyPosition.y + enemySize.y);
      ENG::Drawer::DrawRectangleOutline(
        ENG::Rectangle{left, top, right - left, bottom - top}, ENG::Color::Red, 2.0f);
    }
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
      case Movement::State::Running:
        movement->state = Movement::State::Walking;
        transform.m_velocity = movement->walkingVelocity;
        break;
      case Movement::State::Walking:
        movement->state = Movement::State::Running;
        transform.m_velocity = movement->runningVelocity;
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
      aSource->SetAnimation(System_GetAnimationLooking("Walk",  direction));

      source.GetTransform().m_position = source.GetTransform().m_position + movement;
    }
  }

  std::string System_GetAnimationLooking(const std::string& prefix, const ENG::Vector2& direction)
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

  std::unique_ptr<ENG::Object> System_CreateEntity(const ENG::Rectangle& rect, const std::string& entityName, const std::string& entityClass)
  {
    auto entity = std::make_unique<ENG::Object>(entityName);
    /// Example Enemy43
    entity->SetName(entityName + std::to_string(entity->GetCountObj())); 
    entity->GetTransform().m_position = System_GetRandomPosition(rect);
    entity->GetTransform().m_velocity = {100,100};

    if(entityClass == "Native")
    {
      entity->SetClass(entityClass);
      entity->AddComponent<ENG::ISprite>("Player", "IddleS", 0, 1.0f);
      entity->AddComponent<ENG::IAnimator>("Player", "IddleS", 10.f, 1, 1.0f);
      entity->GetComponent<ENG::IAnimator>()->Play();
    }
    entity->AddComponent<ENG::IBoundingBox>(ENG::Vector2{64,64});
    auto hp = entity->AddComponent<APP::HealtStat>();
    auto str = entity->AddComponent<APP::StrengthStat>();
    auto def = entity->AddComponent<APP::DefenseStat>();

    // Give AI
    
    return std::move(entity);
  }

  ENG::Vector2 System_GetRandomPosition(const ENG::Rectangle& where)
  {
    constexpr float kMargin = 128.f;
    ENG::Vector2 newPosition = {0, 0};
    const float right = where.x +  where.w;
    const float down = where.y + where.h;

    switch (ENG::GetRandomNumber(0, 3))
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

  //Attack Systems////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool System_MeleeAttack(ENG::Object& player, std::vector<uint32_t> allObjectsIds, ENG::ObjectPool& pool)
  {
    auto* playerBoundingBox = player.GetComponent<ENG::IBoundingBox>();
    auto* strength = player.GetComponent<APP::StrengthStat>();
    ENG::Triangle attackTriangle;
    if (!playerBoundingBox || !strength || !System_GetMeleeAttackTriangle(player, attackTriangle))
      return false;


    const ENG::Vector2 playerPosition = player.GetTransform().m_position;
    const ENG::Vector2 playerSize = playerBoundingBox->GetSize();
    const ENG::Vector2 playerCenter{
      playerPosition.x + playerSize.x / 2.0f,
      playerPosition.y + playerSize.y / 2.0f
    };

    const uint8_t attackDamage = strength->getCurrent();

    const auto distanceSquared = [](const ENG::Vector2& a, const ENG::Vector2& b)
    {
      const float dx = a.x - b.x;
      const float dy = a.y - b.y;
      return dx * dx + dy * dy;
    };
    const float maxReachSquared = std::max({
      distanceSquared(playerCenter, attackTriangle.a),
      distanceSquared(playerCenter, attackTriangle.b),
      distanceSquared(playerCenter, attackTriangle.c)
    });

    bool hitAnyEnemy = false;
    for (const uint32_t id : allObjectsIds)
    {
      auto* enemy = pool.Get(id);
      if (!enemy || enemy == &player || enemy->GetName().rfind("Native", 0) != 0)
        continue;

      auto* enemyBoundingBox = enemy->GetComponent<ENG::IBoundingBox>();
      auto* enemyHealth = enemy->GetComponent<APP::HealtStat>();
      if (!enemyBoundingBox || !enemyHealth || enemyHealth->isDead())
        continue;

      const ENG::Vector2 enemySize = enemyBoundingBox->GetSize();
      const ENG::Vector2 enemyPosition = enemy->GetTransform().m_position;
      const float left = std::min(enemyPosition.x, enemyPosition.x + enemySize.x);
      const float right = std::max(enemyPosition.x, enemyPosition.x + enemySize.x);
      const float top = std::min(enemyPosition.y, enemyPosition.y + enemySize.y);
      const float bottom = std::max(enemyPosition.y, enemyPosition.y + enemySize.y);
      const ENG::Vector2 nearestPoint{
        std::clamp(playerCenter.x, left, right),
        std::clamp(playerCenter.y, top, bottom)
      };

      if (distanceSquared(playerCenter, nearestPoint) > maxReachSquared)
        continue;

      const ENG::Rectangle enemyRect{
        enemyPosition.x, enemyPosition.y, enemySize.x, enemySize.y
      };
      if (!System_CollisionRectTriangle(enemyRect, attackTriangle))
        continue;

      hitAnyEnemy = true;
      enemyHealth->receiveDamage(attackDamage);
      if (enemyHealth->isDead())
        LOG_INFO("El enemigo murio: " + enemy->GetName());
    }

    return hitAnyEnemy;
  }
  //Attack Systems////////////////////////////////////////////////////////////////////////////////////////////////////////
}
