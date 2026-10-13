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
    LOG_DEBUG("[RectTriangle] Inicio: rect=(" + std::to_string(rect.x) + ", " +
              std::to_string(rect.y) + ", " + std::to_string(rect.w) + ", " +
              std::to_string(rect.h) + "), triangle.a=(" +
              std::to_string(triangle.a.x) + ", " + std::to_string(triangle.a.y) +
              "), triangle.b=(" + std::to_string(triangle.b.x) + ", " +
              std::to_string(triangle.b.y) + "), triangle.c=(" +
              std::to_string(triangle.c.x) + ", " + std::to_string(triangle.c.y) + ")");

    const float left = std::min(rect.x, rect.x + rect.w);
    const float right = std::max(rect.x, rect.x + rect.w);
    const float top = std::min(rect.y, rect.y + rect.h);
    const float bottom = std::max(rect.y, rect.y + rect.h);
    LOG_DEBUG("[RectTriangle] Rect normalizado: left=" + std::to_string(left) +
              ", right=" + std::to_string(right) + ", top=" + std::to_string(top) +
              ", bottom=" + std::to_string(bottom));

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
    for (std::size_t i = 0; i < rectangleVertices.size(); ++i)
    {
      LOG_DEBUG("[RectTriangle] rectVertex[" + std::to_string(i) + "]=(" +
                std::to_string(rectangleVertices[i].x) + ", " +
                std::to_string(rectangleVertices[i].y) + ")");
    }
    for (std::size_t i = 0; i < triangleVertices.size(); ++i)
    {
      LOG_DEBUG("[RectTriangle] triangleVertex[" + std::to_string(i) + "]=(" +
                std::to_string(triangleVertices[i].x) + ", " +
                std::to_string(triangleVertices[i].y) + ")");
    }

    const auto separatedOnAxis = [&](const ENG::Vector2& axis, const std::string& axisName)
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

      const bool separated = rectMax < triangleMin || triangleMax < rectMin;
      LOG_DEBUG("[RectTriangle] Eje " + axisName + "=(" + std::to_string(axis.x) +
                ", " + std::to_string(axis.y) + "), proyeccion rect=[" +
                std::to_string(rectMin) + ", " + std::to_string(rectMax) +
                "], triangulo=[" + std::to_string(triangleMin) + ", " +
                std::to_string(triangleMax) + "], separados=" +
                std::string(separated ? "true" : "false"));
      return separated;
    };

    LOG_DEBUG("[RectTriangle] Probando ejes X e Y del rectangulo");
    if (separatedOnAxis({1.0f, 0.0f}, "rect-X"))
    {
      LOG_DEBUG("[RectTriangle] Resultado: no colisiona; separados en el eje X");
      return false;
    }
    if (separatedOnAxis({0.0f, 1.0f}, "rect-Y"))
    {
      LOG_DEBUG("[RectTriangle] Resultado: no colisiona; separados en el eje Y");
      return false;
    }

    for (std::size_t i = 0; i < triangleVertices.size(); ++i)
    {
      const auto& start = triangleVertices[i];
      const auto& end = triangleVertices[(i + 1) % triangleVertices.size()];
      const ENG::Vector2 edge = end - start;
      const ENG::Vector2 normal{-edge.y, edge.x};
      const std::string axisName = "triangle-edge-" + std::to_string(i);
      LOG_DEBUG("[RectTriangle] Probando arista " + std::to_string(i) +
                ": inicio=(" + std::to_string(start.x) + ", " +
                std::to_string(start.y) + "), fin=(" + std::to_string(end.x) +
                ", " + std::to_string(end.y) + "), normal=(" +
                std::to_string(normal.x) + ", " + std::to_string(normal.y) + ")");
      if (separatedOnAxis(normal, axisName))
      {
        LOG_DEBUG("[RectTriangle] Resultado: no colisiona; separados en " + axisName);
        return false;
      }
    }

    LOG_DEBUG("[RectTriangle] Resultado: colisiona; no se encontro eje separador");
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

  //Attack Systems
  bool System_MeleeAttack(ENG::Object& player, std::vector<uint32_t> allObjectsIds, ENG::ObjectPool& pool){
    static bool attackActive = true;
    static float dt_attack = 0;

    // LOG_DEBUG("[MeleeAttack] Inicio. Jugador=\"" + player.GetName() +
    //           "\", cantidad de ids=" + std::to_string(allObjectsIds.size()) +
    //           ", attackActive=" + std::string(attackActive ? "true" : "false") +
    //           ", dt_attack=" + std::to_string(dt_attack));

    auto* playerBoundingBox = player.GetComponent<ENG::IBoundingBox>();
    auto* animator = player.GetComponent<ENG::IAnimator>();
    auto* attackDistance = player.GetComponent<APP::MeleeAttackDistance>();
    auto* attackRange = player.GetComponent<APP::MeleeAttackRange>();
    auto* attackVelocity = player.GetComponent<APP::MeleeAttackVelocity>();
    auto* strength = player.GetComponent<APP::StrengthStat>();

    // LOG_DEBUG("[MeleeAttack] Componentes: IBoundingBox=" +
    //           std::string(playerBoundingBox ? "presente" : "null") +
    //           ", IAnimator=" + (animator ? "presente" : "null") +
    //           ", MeleeAttackDistance=" + (attackDistance ? "presente" : "null") +
    //           ", MeleeAttackRange=" + (attackRange ? "presente" : "null") +
    //           ", MeleeAttackVelocity=" + (attackVelocity ? "presente" : "null") +
    //           ", StrengthStat=" + (strength ? "presente" : "null"));
    if (!playerBoundingBox || !animator || !attackDistance || !attackRange ||
        !attackVelocity || !strength)
    {
      LOG_ERROR("[MeleeAttack] Cancelado: falta un componente requerido del jugador");
      return false;
    }

    constexpr float sen45 = 0.7071f;
    const auto player_position_center = playerBoundingBox->GetCenter();
    const std::string animationKey = animator->GetKeyAnimation();
    const auto directionLooking = animationKey.substr(4);
    int current_direction = 5;
    const float attk_dist = attackDistance->getCurrent();
    const float attk_range = attackRange->getCurrent();
    const float attk_vel = attackVelocity->getCurrent();
    const uint8_t attk_damage = strength->getCurrent();

    // LOG_DEBUG("[MeleeAttack] Centro jugador=(" +
    //           std::to_string(player_position_center.x) + ", " +
    //           std::to_string(player_position_center.y) + ")");
    // LOG_DEBUG("[MeleeAttack] animationKey=\"" + animationKey +
    //           "\", directionLooking=\"" + directionLooking + "\"");
    // LOG_DEBUG("[MeleeAttack] attk_dist=" + std::to_string(attk_dist) +
    //           ", attk_range=" + std::to_string(attk_range) +
    //           ", attk_vel=" + std::to_string(attk_vel) +
    //           ", attk_damage=" + std::to_string(static_cast<unsigned>(attk_damage)) +
    //           ", sen45=" + std::to_string(sen45));

    ENG::Vector2 allDirections[8] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};

    // LOG_DEBUG("[MeleeAttack] Resolviendo direccion");
    if(directionLooking == "N"){
      current_direction = 1;
    }else if(directionLooking == "S"){
      current_direction = 5;
    }else if(directionLooking == "E"){
      current_direction = 3;
    }else if(directionLooking == "W"){
      current_direction = 7;
    }else if(directionLooking == "NE"){
      current_direction = 2;
    }else if(directionLooking == "SE"){
      current_direction = 4;
    }else if(directionLooking == "NW"){
      current_direction = 0;
    }else if(directionLooking == "SW"){
      current_direction = 6;
    }else
    {
      current_direction = 5;
    }
    // LOG_DEBUG("[MeleeAttack] current_direction=" + std::to_string(current_direction) +
    //           ", vector=(" + std::to_string(allDirections[current_direction].x) +
    //           ", " + std::to_string(allDirections[current_direction].y) + ")");

    ENG::Vector2 attack_vertex[4]; //points of the triangle attack
    attack_vertex[0] = player_position_center; //the first point is the player position

    //calculation of the two remaining attack points
    bool isDiagonal = (std::abs(allDirections[current_direction].x) + std::abs(allDirections[current_direction].y)) == 2;
    // LOG_DEBUG("[MeleeAttack] isDiagonal=" + std::string(isDiagonal ? "true" : "false") +
    //           ", vertice0=(" + std::to_string(attack_vertex[0].x) + ", " +
    //           std::to_string(attack_vertex[0].y) + ")");
    
    if(isDiagonal){
      float dist = attk_dist * sen45; //sen(45) = 0.7071 (law of sines)
      // LOG_DEBUG("[MeleeAttack] Rama diagonal: dist=" + std::to_string(dist));

      attack_vertex[1] = ENG::Vector2( // center point of attack
        player_position_center.x + (dist * allDirections[current_direction].x),
        player_position_center.y + (dist * allDirections[current_direction].y)
      );
      // LOG_DEBUG("[MeleeAttack] vertice1=(" + std::to_string(attack_vertex[1].x) +
      //           ", " + std::to_string(attack_vertex[1].y) + ")");

      float dist_for_range = (attk_range / 2) * sen45;
      int idx = current_direction - 2 < 0 ?  (( 8 + current_direction) - 2) : current_direction - 2;
      // LOG_DEBUG("[MeleeAttack] dist_for_range=" + std::to_string(dist_for_range) +
      //           ", indice lateral 1=" + std::to_string(idx));

      attack_vertex[2] = ENG::Vector2( //range vertex 1
        attack_vertex[1].x + (dist_for_range * allDirections[idx].x),
        attack_vertex[1].y + (dist_for_range * allDirections[idx].y)
      );
      // LOG_DEBUG("[MeleeAttack] vertice2=(" + std::to_string(attack_vertex[2].x) +
      //           ", " + std::to_string(attack_vertex[2].y) + ")");

      idx = (current_direction + 2) % 8;
      // LOG_DEBUG("[MeleeAttack] indice lateral 2=" + std::to_string(idx));

      attack_vertex[3] = ENG::Vector2( //range vertex 2
        attack_vertex[1].x + (dist_for_range * allDirections[idx].x),
        attack_vertex[1].y + (dist_for_range * allDirections[idx].y)
      );
      // LOG_DEBUG("[MeleeAttack] vertice3=(" + std::to_string(attack_vertex[3].x) +
      //           ", " + std::to_string(attack_vertex[3].y) + ")");

    }else{
      float dist_for_range = attk_range / 2;
      bool isVertical = allDirections[current_direction].y != 0;
      // LOG_DEBUG("[MeleeAttack] Rama cardinal: dist_for_range=" +
      //           std::to_string(dist_for_range) + ", isVertical=" +
      //           std::string(isVertical ? "true" : "false"));

      attack_vertex[1] = ENG::Vector2( // center point of attack
        player_position_center.x + (attk_dist * allDirections[current_direction].x),
        player_position_center.y + (attk_dist * allDirections[current_direction].y)
      );
      // LOG_DEBUG("[MeleeAttack] vertice1=(" + std::to_string(attack_vertex[1].x) +
      //           ", " + std::to_string(attack_vertex[1].y) + ")");

      attack_vertex[2] = ENG::Vector2(
        attack_vertex[1].x + (dist_for_range * (isVertical ? -1 : 0)),
        attack_vertex[1].y + (dist_for_range * (isVertical ? 0 : -1))
      );
      // LOG_DEBUG("[MeleeAttack] vertice2=(" + std::to_string(attack_vertex[2].x) +
      //           ", " + std::to_string(attack_vertex[2].y) + ")");

      attack_vertex[3] = ENG::Vector2(
        attack_vertex[1].x + (dist_for_range * (isVertical ? 1 : 0)),
        attack_vertex[1].y + (dist_for_range * (isVertical ? 0 : 1))
      );
      // LOG_DEBUG("[MeleeAttack] vertice3=(" + std::to_string(attack_vertex[3].x) +
      //           ", " + std::to_string(attack_vertex[3].y) + ")");
    }

    //get possibly enemies in attack area
    std::vector<ENG::Object*> close_enemies;

    // LOG_DEBUG("[MeleeAttack] Inicio de revision de objetivos");
    for (const auto id : allObjectsIds)
    {
      // LOG_DEBUG("[MeleeAttack] Buscando id=" + std::to_string(id));
      auto* enemy = pool.Get(id);
      if (!enemy)
      {
        LOG_ERROR("[MeleeAttack] No se encontro objeto con id=" +
                  std::to_string(id) + "; se omite");
        continue;
      }
      // LOG_DEBUG("[MeleeAttack] Objeto encontrado: \"" + enemy->GetName() + "\"");

      if(enemy->GetName().substr(0, 6) != "Native")
      {
        // LOG_DEBUG("[MeleeAttack] Objeto omitido: no coincide con el filtro Native");
        continue;
      }

      const auto& boundingBoxe = enemy->GetComponent<ENG::IBoundingBox>();
      if (!boundingBoxe)
      {
        LOG_ERROR("[MeleeAttack] Objeto \"" + enemy->GetName() +
                  "\" no tiene bounding box; se omite");
        continue;
      }

      auto* enemyHealth = enemy->GetComponent<APP::HealtStat>();
      if (!enemyHealth)
      {
        LOG_ERROR("[MeleeAttack] Objeto \"" + enemy->GetName() +
                  "\" no tiene HealtStat; se omite");
        continue;
      }

      const auto& boundingBox = boundingBoxe[0];
      ENG::Vector2 position = boundingBox.GetPosition();
      ENG::Vector2 size = boundingBox.GetSize();
      // LOG_DEBUG("[MeleeAttack] Rect enemigo: posicion=(" +
      //           std::to_string(position.x) + ", " + std::to_string(position.y) +
      //           "), tamano=(" + std::to_string(size.x) + ", " +
      //           std::to_string(size.y) + ")");

      ENG::Vector2 center = ENG::Vector2(
        (attack_vertex[0].x + attack_vertex[2].x + attack_vertex[3].x)/2,
        (attack_vertex[0].y + attack_vertex[2].y + attack_vertex[3].y)/2
      );
      // LOG_DEBUG("[MeleeAttack] Centro triangulo=(" + std::to_string(center.x) +
      //           ", " + std::to_string(center.y) + ")");

      //in collision?
      const ENG::Rectangle enemyRect{position.x, position.y, size.x, size.y};
      const ENG::Triangle attackTriangle{
        attack_vertex[0], attack_vertex[2], attack_vertex[3], center
      };
      // LOG_DEBUG("[MeleeAttack] Llamando System_CollisionRectTriangle");
      const bool collision = System_CollisionRectTriangle(enemyRect, attackTriangle);
      // LOG_DEBUG("[MeleeAttack] Resultado de colision=" +
      //           std::string(collision ? "true" : "false"));

      if(collision){
        const auto healthBefore = enemyHealth->getCurrent();
        // LOG_DEBUG("[MeleeAttack] Aplicando dano=" +
        //           std::to_string(static_cast<unsigned>(attk_damage)) +
        //           ", vida antes=" +
        //           std::to_string(static_cast<unsigned>(healthBefore)));
        enemyHealth->receiveDamage(attk_damage);
        const auto healthAfter = enemyHealth->getCurrent();
        // LOG_DEBUG("[MeleeAttack] Vida despues=" +
        //           std::to_string(static_cast<unsigned>(healthAfter)));
        return true;
      }
    }

    return false;
  }
}
