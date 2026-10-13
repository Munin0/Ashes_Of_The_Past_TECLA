#include "HUD.hpp"
/// | -------------------------------------------|
#include "Engine/Object/Object.hpp"
#include "Engine/Render/Batching/RAPIBatch.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Engine/Render/Color/RColor.hpp"
#include "Engine/Utils/RawGeometry.hpp"
/// | -------------------------------------------|
#include <algorithm>
/// | -------------------------------------------|

namespace APP
{
  void EssenceBar(ENG::Batcher& batcher, const ENG::Camera2D& cam, ENG::Object& who)
  {
    auto& stats = who.GetStats();

    if(stats.m_essence_max == 0) return;

    const float zoom = cam.GetZoom();
    const ENG::Rectangle view = cam.GetRectCamera();

    const float margin = 20.0f;
    const float barW   = 200.0f;
    const float barH   = 24.0f;

    float fraction = static_cast<float>(stats.m_essence) / static_cast<float>(stats.m_essence_max);
    fraction = std::min(fraction, 1.0f);

    const float x = view.x + margin / zoom;
    const float y = view.y + margin / zoom;

    ENG::Rectangle back = {x, y, barW / zoom, barH / zoom};
    ENG::Drawer::DrawRectangle(back, ENG::Color::Gray);

    ENG::Rectangle fill = {x, y, (barW * fraction) / zoom, barH / zoom};
    ENG::Drawer::DrawRectangle(fill, ENG::Color::Blue);
  }

}
