#include "HUD.hpp"
/// | -------------------------------------------|
#include "Engine/Object/Object.hpp"
#include "Engine/Render/Batching/RAPIBatch.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Engine/Render/Color/RColor.hpp"
#include "Engine/Render/Render.hpp"
#include "Engine/Services/Services.hpp"
#include "Engine/Text/Font/Font.hpp"
#include "Engine/Text/TextAPI.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | -------------------------------------------|
#include "Game/Components/Dash.hpp"
#include "Game/Components/Heal.hpp"
#include "Game/Components/Movement.hpp"
#include "Game/Components/Stats.hpp"
#include "Game/Systems/HighScores.hpp"
/// | -------------------------------------------|
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
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

  void PlayerHUD(ENG::Batcher& batcher, const ENG::Camera2D& cam, ENG::Object& who)
  {
    (void)batcher;

    const ENG::Font* font = ENG::Services::Fonts().GetFont("Ithaca");
    if(!font) return;

    auto& base = who.GetStats();

    const auto* health   = who.GetComponent<HealtStat>();
    const auto* strength = who.GetComponent<StrengthStat>();
    const auto* xp       = who.GetComponent<XPStat>();
    const auto* dash     = who.GetComponent<Dash>();
    const auto* move     = who.GetComponent<Movement>();
    const auto* heal     = who.GetComponent<Heal>();

    const uint32_t hp    = static_cast<uint32_t>(health   ? health->getCurrent()   : base.m_hp);
    const uint32_t hpMax = static_cast<uint32_t>(health   ? health->getMax()       : base.m_hp_max);
    const uint32_t dmg   = static_cast<uint32_t>(strength ? strength->getCurrent() : base.m_str);
    const uint32_t force = static_cast<uint32_t>(strength ? strength->getMax()     : base.m_str);
    const uint32_t xpNow = static_cast<uint32_t>(xp       ? xp->getCurrent()       : base.m_xp);

    bool  dashReady = true;
    float dashLeft  = 0.0f;
    if(dash)
    {
      dashReady = (dash->GetState() == DashState::Ready);
      if(dash->GetState() == DashState::Dashing)
        dashLeft = std::max(0.0f, dash->GetDuration() - dash->GetTimer()) + dash->GetCooldown();
      else if(dash->GetState() == DashState::Cooldown)
        dashLeft = std::max(0.0f, dash->GetCooldown() - dash->GetTimer());
    }

    bool  runReady = true;
    float runLeft  = 0.0f;
    if(move)
    {
      runReady = (move->GetState() != MovementState::Cooldown);
      if(move->GetState() == MovementState::Running)
        runLeft = std::max(0.0f, move->GetDuration() - move->GetTimer());
      else if(move->GetState() == MovementState::Cooldown)
        runLeft = std::max(0.0f, move->GetCooldown() - move->GetTimer());
    }

    bool  healReady = true;
    float healLeft  = 0.0f;
    if(heal)
    {
      healReady = heal->IsReady();
      if(!healReady)
        healLeft = std::max(0.0f, heal->GetCooldown() - heal->GetTimer());
    }

    const float zoom = cam.GetZoom();
    const ENG::Rectangle view = cam.GetRectCamera();

    const float margin = 20.0f;
    const float panelW = 300.0f;
    const float lineH  = 24.0f;
    const float pad    = 12.0f;
    const float barH   = 12.0f;
    const float gap    = 6.0f;
    const float panelH = pad * 2.0f + lineH + barH + gap + lineH * 7.0f;

    const float screenX = margin;
    const float screenY = margin + 40.0f;

    auto W  = [&zoom](float v)  { return v / zoom; };
    auto PX = [&view, &zoom](float sx) { return view.x + sx / zoom; };
    auto PY = [&view, &zoom](float sy) { return view.y + sy / zoom; };

    ENG::Rectangle panel = {PX(screenX), PY(screenY), W(panelW), W(panelH)};
    ENG::Drawer::DrawRectangle(panel, ENG::Color(0.0f, 0.0f, 0.0f, 0.55f));
    ENG::Drawer::DrawRectangleOutline(panel, ENG::Color(1.0f, 1.0f, 1.0f, 0.85f), W(1.0f));

    const float textX = screenX + pad;
    float textY = screenY + pad;

    auto line = [&](const std::string& str, const ENG::Color& color)
    {
      ENG::TextAPI::Get().DrawTextEx(*font, ENG::Vector2{PX(textX), PY(textY)}, str, W(16.0f), color);
      textY += lineH;
    };

    line("Estado del jugador", ENG::Color(1.0f, 1.0f, 0.6f));

    const int barX = static_cast<int>(textX);
    const int barY = static_cast<int>(textY + gap * 0.5f);
    const int barW = static_cast<int>(panelW - pad * 2.0f);
    float hpf = (hpMax > 0) ? static_cast<float>(hp) / static_cast<float>(hpMax) : 0.0f;
    hpf = std::min(1.0f, std::max(0.0f, hpf));

    ENG::Drawer::DrawRectangle({PX(static_cast<float>(barX)), PY(static_cast<float>(barY)), W(static_cast<float>(barW)), W(barH)},
                               ENG::Color(0.25f, 0.0f, 0.0f, 0.9f));
    ENG::Drawer::DrawRectangle({PX(static_cast<float>(barX)), PY(static_cast<float>(barY)), W(static_cast<float>(barW) * hpf), W(barH)},
                               ENG::Color(0.15f, 0.8f, 0.2f, 0.95f));
    textY += barH + gap;

    line(ENG::TextAPI::Get().FormatText("Vida: %u / %u", hp, hpMax), ENG::Color(0.5f, 1.0f, 0.5f));
    line(ENG::TextAPI::Get().FormatText("Experiencia: %u (Nv. %u)", xpNow, static_cast<uint32_t>(base.m_level)),
         ENG::Color(0.95f, 0.85f, 0.3f));
    line(ENG::TextAPI::Get().FormatText("Dano actual: %u", dmg), ENG::Color(1.0f, 0.5f, 0.25f));
    line(ENG::TextAPI::Get().FormatText("Fuerza: %u", force), ENG::Color(1.0f, 0.75f, 0.4f));
    line(dashReady ? std::string("Dash: Listo") : ENG::TextAPI::Get().FormatText("Dash: %.1fs", dashLeft),
         ENG::Color(0.4f, 0.85f, 1.0f));
    line(runReady ? std::string("Correr: Listo") : ENG::TextAPI::Get().FormatText("Correr: %.1fs", runLeft),
         ENG::Color(0.4f, 1.0f, 0.8f));
    line(healReady ? std::string("Curar: Listo") : ENG::TextAPI::Get().FormatText("Curar: %.1fs", healLeft),
         ENG::Color(0.6f, 1.0f, 0.6f));
  }

  void ScoreHUD(ENG::Batcher& batcher, const ENG::Camera2D& cam, uint32_t enemiesKilled, uint32_t score)
  {
    (void)batcher;

    const ENG::Font* font = ENG::Services::Fonts().GetFont("Ithaca");
    if(!font) return;

    const float zoom = cam.GetZoom();
    const ENG::Rectangle view = cam.GetRectCamera();
    const ENG::Vector2 viewport = cam.GetViewportSize();

    const float margin = 20.0f;
    const float panelW = 280.0f;
    const float lineH  = 24.0f;
    const float pad    = 12.0f;
    const float panelH = pad * 2.0f + lineH * 3.0f;

    auto W = [&zoom](float v) { return v / zoom; };

    /// Pegado al borde superior derecho de la pantalla.
    const float screenX = viewport.x - margin - panelW;
    const float screenY = margin;

    ENG::Rectangle panel = {view.x + screenX / zoom, view.y + screenY / zoom, W(panelW), W(panelH)};
    ENG::Drawer::DrawRectangle(panel, ENG::Color(0.0f, 0.0f, 0.0f, 0.55f));
    ENG::Drawer::DrawRectangleOutline(panel, ENG::Color(1.0f, 1.0f, 1.0f, 0.85f), W(1.0f));

    const float textX = screenX + pad;
    float textY = screenY + pad;

    auto line = [&](const std::string& str, const ENG::Color& color)
    {
      ENG::TextAPI::Get().DrawTextEx(*font,
        ENG::Vector2{view.x + textX / zoom, view.y + textY / zoom}, str, W(16.0f), color);
      textY += lineH;
    };

    line("Puntuacion", ENG::Color(1.0f, 1.0f, 0.6f));
    line(ENG::TextAPI::Get().FormatText("Enemigos matados: %u", enemiesKilled), ENG::Color(1.0f, 0.5f, 0.5f));
    line(ENG::TextAPI::Get().FormatText("Puntuaje: %u", score), ENG::Color(0.6f, 1.0f, 0.6f));
  }

  void DrawHighScoresPanel(const HighScores& scores)
  {
    const ENG::Font* font = ENG::Services::Fonts().GetFont("Ithaca");
    if(!font) return;

    const ENG::Vector2 screen = ENG::Render::Get().GetScreenSize();
    ENG::TextAPI& text = ENG::TextAPI::Get();

    const auto& entries = scores.GetEntries();

    const float panelW  = 360.0f;
    const float rowH    = 34.0f;
    const float headerH = 44.0f;
    const float pad     = 16.0f;
    const float margin  = 40.0f;

    const float x = screen.x - panelW - margin;
    const float y = margin;

    const float panelH = pad * 2.0f + headerH + rowH * static_cast<float>(entries.empty() ? 1u : entries.size());

    ENG::Rectangle panel = {x - pad, y - pad, panelW, panelH};
    ENG::Drawer::DrawRectangle(panel, ENG::Color(0.0f, 0.0f, 0.0f, 0.85f));
    ENG::Drawer::DrawRectangleOutline(panel, ENG::Color(1.0f, 1.0f, 1.0f, 0.85f), 2.0f);

    text.DrawTextEx(*font, {x, y + 10.f}, "Mejores Puntuaciones", 26.0f, ENG::Color(1.0f, 0.9f, 0.4f));

    float ry = y + headerH;
    if(entries.empty())
    {
      text.DrawTextEx(*font, {x, ry}, "Sin puntuaciones", 20.0f, ENG::Color(0.8f, 0.8f, 0.8f));
      return;
    }

    for(size_t i = 0; i < entries.size(); ++i)
    {
      const std::string rank  = std::to_string(i + 1) + ".";
      const std::string value = text.FormatText("%u", entries[i].score);

      text.DrawTextEx(*font, {x + 5.f, ry}, rank, 22.0f, ENG::Color(0.85f, 0.85f, 0.85f));
      text.DrawTextEx(*font, {x + 44.0f, ry}, entries[i].name, 22.0f, ENG::Color::White);

      const float sw = text.GetMeasureTextEx(font, value, 22.0f).x;
      text.DrawTextEx(*font, {x + panelW - pad - sw - 25.f, ry}, value, 22.0f, ENG::Color(0.6f, 1.0f, 0.6f));

      ry += rowH;
    }
  }

}
