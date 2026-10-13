// | -------------------------------
#include "Collision.hpp"
// | -------------------------------
#include "Engine/Component/Component.hpp"
#include "Engine/Map/TileMap/TileMap.hpp"
#include "Engine/Object/Object.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Vector2.hpp"
// | -------------------------------
#include "glm/ext/vector_float2.hpp"
#include "glm/geometric.hpp"
// | -------------------------------
#include <algorithm>
#include <array>
#include <cstddef>
// | -------------------------------

namespace ENG
{
  bool CollisionTileMapIsometric(Object& object, const TileMap& map, bool resolution)
  {
    IBoundingBox* box = object.GetComponent<ENG::IBoundingBox>();
    if (!box) return false;

    const Vector2 size = box->GetSize();
    const float footWidth  = size.x * 0.4f;
    const float footHeight = std::max(2.0f, size.y * 0.12f);

    auto blockedAt = [&](const Vector2& boxPos)
    {
      const float x = boxPos.x + (size.x - footWidth) * 0.5f;
      const float y = boxPos.y + size.y - footHeight;
      const Vector2 s[4] = {
        {x, y}, {x + footWidth, y},
        {x, y + footHeight}, {x + footWidth, y + footHeight}
      };
      for (const auto& p : s)
        if (map.IsBlockedAtWorld(p)) return true;
      return false;
    };

    const Vector2 boxPos = box->GetPosition();
    if (!blockedAt(boxPos)) return false;

    if (resolution)
    {
      const Vector2 cur  = object.GetPosition();
      const Vector2 prev = object.GetTransform().m_prev_position;

      Vector2 tryX = {prev.x, cur.y};
      Vector2 tryY = {cur.x,  prev.y};

      Vector2 offX = boxPos + (tryX - cur);
      Vector2 offY = boxPos + (tryY - cur);

      if (!blockedAt(offX))      object.SetPosition(tryX);
      else if (!blockedAt(offY)) object.SetPosition(tryY);
      else                       object.SetPosition(prev);
    }
    return true;
  }

  bool CollisionAABB(Object& source, Object& target, bool resolution)
  {
    Vector2 posA = source.GetPosition(), posB = target.GetPosition();
    auto dimA = source.GetSolidBox()->GetSize();
    auto dimB = target.GetSolidBox()->GetSize(); // If vaalidation
   bool r = (posA.x <= dimB.x && dimA.x >= posB.x) && (posA.y <= dimB.y && dimA.y >= posB.y);

    if(r && resolution)
    {
      auto& p = source.GetTransform();
      p.m_position = p.m_prev_position;
    }

    return r;
  }

  bool CollisionAABB(const IBoundingBox& source, const IBoundingBox& target)
  {
    Vector2 posA = source.GetPosition(), posB = target.GetPosition();
    auto dimA = source.GetSize();
    auto dimB = target.GetSize();
    return (posA.x <= dimB.x && dimA.x >= posB.x) && (posA.y <= dimB.y && dimA.y >= posB.y);
  }

  bool CollisionAABB(const Rectangle& a, const Rectangle& b)
  {
    return (a.x <= b.w && a.w >= b.x) && (a.y <= b.h && a.h >= b.y);
  }

  bool CollisionPointRect(const Vector2 &point, const Rectangle &rect)
  {
    return (point.x >= rect.x && point.x <= rect.x + rect.w) && (point.y >= rect.y && point.y <= rect.y + rect.h); 
  }

  bool CollisionCircle(const Circle& a, const Circle& b)
  {
    glm::vec2 d = {b.center.x - a.center.x, b.center.y - a.center.y};
    float distSq = glm::dot(d, d);
    float radiusSum = a.radius + b.radius;
    return distSq <= (radiusSum * radiusSum);
  }

  bool CollisionRectTriangle(const ENG::Rectangle& rect, const ENG::Triangle& triangle)
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

    for (size_t i = 0; i < triangleVertices.size(); ++i)
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

}
