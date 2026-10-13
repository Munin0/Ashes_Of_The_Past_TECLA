/// | ------------------------------------ |
#include "NameEntry.hpp"
/// | ------------------------------------ |
#include "Engine/Inputs/PollEvent.hpp"
#include "Engine/Render/Batching/RAPIBatch.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Engine/Render/Color/RColor.hpp"
#include "Engine/Services/Services.hpp"
#include "Engine/Text/Font/Font.hpp"
#include "Engine/Text/TextAPI.hpp"
#include "Engine/Utils/RawGeometry.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |
#include "SDL3/SDL_scancode.h"
/// | ------------------------------------ |
#include <cstdint>
#include <string>
/// | ------------------------------------ |

namespace APP
{
  std::string NameEntry::NameString(void) const
  {
    return std::string{m_initials[0], m_initials[1], m_initials[2]};
  }

  void NameEntry::Open(uint32_t score)
  {
    m_score = score;
    m_state = State::Editing;
    m_initials[0] = m_initials[1] = m_initials[2] = 'A';
    m_selected = 0;
    m_exit = false;
  }

  void NameEntry::Input(float dt)
  {
    (void)dt;
    auto& pollEvent = ENG::PollEvent::Get();

    if(m_state == State::Editing)
    {
      if(pollEvent.IsKeyPress(SDL_SCANCODE_LEFT))
        m_selected = (m_selected + 2) % 3;
      if(pollEvent.IsKeyPress(SDL_SCANCODE_RIGHT))
        m_selected = (m_selected + 1) % 3;

      auto cycle = [&](int dir)
      {
        const int idx = (m_initials[m_selected] - 'A' + dir + 26) % 26;
        m_initials[m_selected] = static_cast<char>('A' + idx);
      };
      if(pollEvent.IsKeyPress(SDL_SCANCODE_UP))   cycle(-1);
      if(pollEvent.IsKeyPress(SDL_SCANCODE_DOWN)) cycle(1);

      /// Escribir una letra directamente la coloca y avanza a la siguiente.
      for(int s = SDL_SCANCODE_A; s <= SDL_SCANCODE_Z; ++s)
      {
        if(pollEvent.IsKeyPress(static_cast<SDL_Scancode>(s)))
        {
          m_initials[m_selected] = static_cast<char>('A' + (s - SDL_SCANCODE_A));
          m_selected = (m_selected + 1) % 3;
          break;
        }
      }

      if(pollEvent.IsKeyPress(SDL_SCANCODE_RETURN) ||
         pollEvent.IsKeyPress(SDL_SCANCODE_KP_ENTER))
      {
        m_highScores.Add(NameString(), m_score);
        m_state = State::Saved;
      }
    }
    else if(m_state == State::Saved)
    {
      if(pollEvent.IsKeyPress(SDL_SCANCODE_RETURN) ||
         pollEvent.IsKeyPress(SDL_SCANCODE_KP_ENTER) ||
         pollEvent.IsKeyPress(SDL_SCANCODE_SPACE))
      {
        m_exit = true;
      }
    }
  }

  void NameEntry::Render(const ENG::Camera2D& cam) const
  {
    const ENG::Font* font = ENG::Services::Fonts().GetFont("Ithaca");
    if(!font) return;

    const float zoom = cam.GetZoom();
    const ENG::Rectangle view = cam.GetRectCamera();
    const ENG::Vector2 vp = cam.GetViewportSize();

    auto PX = [&view, &zoom](float sx) { return view.x + sx / zoom; };
    auto PY = [&view, &zoom](float sy) { return view.y + sy / zoom; };
    auto SZ = [&zoom](float s) { return s / zoom; };

    ENG::TextAPI& text = ENG::TextAPI::Get();

    /// Fondo oscuro a pantalla completa.
    ENG::Drawer::DrawRectangle({PX(0.0f), PY(0.0f), SZ(vp.x), SZ(vp.y)},
                               ENG::Color(0.0f, 0.0f, 0.0f, 0.85f));

    auto centeredText = [&](const std::string& str, float screenY, float pxSize, const ENG::Color& color)
    {
      const float w = text.GetMeasureTextEx(font, str, pxSize).x;
      text.DrawTextEx(*font, {PX((vp.x - w) * 0.5f), PY(screenY)}, str, SZ(pxSize), color);
    };

    centeredText("GAME OVER", 220.0f, 72.0f, ENG::Color(1.0f, 0.25f, 0.25f));
    centeredText(text.FormatText("Puntuaje: %u", m_score), 355.0f, 34.0f, ENG::Color(1.0f, 1.0f, 0.6f));
    centeredText("Ingresa tu nombre:", 420.0f, 24.0f, ENG::Color::White);

    /// Tres letras al estilo maquinita: A | B | C, resaltando la activa.
    const float letterPx = 64.0f;
    const float spacing  = 130.0f;
    const float slotY    = 500.0f;
    const float centerX  = vp.x * 0.5f;

    for(int i = 0; i < 3; ++i)
    {
      const std::string letter(1, m_initials[i]);
      const float w = text.GetMeasureTextEx(font, letter, letterPx).x;
      const float x = centerX + (i - 1) * spacing - w * 0.5f;

      if(i == m_selected && m_state == State::Editing)
      {
        const float pad = 16.0f;
        ENG::Drawer::DrawRectangle(
            {PX(x - pad + 5.f), PY((slotY - 20.f - pad * 2.0f)), SZ(w + pad), SZ((letterPx + pad - 13.f))},
            ENG::Color(0.9f, 0.7f, 0.1f, 0.30f));
        ENG::Drawer::DrawRectangleOutline(
            {PX(x - pad + 5.f), PY((slotY - 20.f - pad * 2.0f)), SZ(w + pad), SZ((letterPx + pad - 13.f))},
            ENG::Color(1.0f, 0.9f, 0.3f), SZ(2.0f));
      }

      const ENG::Color color = (i == m_selected && m_state == State::Editing)
        ? ENG::Color(1.0f, 1.0f, 0.4f) : ENG::Color::White;
      text.DrawTextEx(*font, {PX(x), PY(slotY)}, letter, SZ(letterPx), color);
    }

    /// Separadores entre las letras.
    const float barW = text.GetMeasureTextEx(font, "|", letterPx).x;
    for(int i = 0; i < 2; ++i)
    {
      const float cx = centerX + (i == 0 ? -spacing * 0.5f : spacing * 0.5f);
      text.DrawTextEx(*font, {PX(cx - barW * 0.5f), PY(slotY)}, "|", SZ(letterPx), ENG::Color(0.7f, 0.7f, 0.7f));
    }

    if(m_state == State::Editing)
    {
      centeredText("Flechas: cambiar letra      Enter: guardar", 720.0f, 22.0f, ENG::Color(0.8f, 0.8f, 0.8f));
    }
    else
    {
      centeredText("Guardado: " + NameString() + " | " + std::to_string(m_score),
                   720.0f, 26.0f, ENG::Color(0.5f, 1.0f, 0.5f));
      centeredText("Pulsa Enter para volver al menu", 760.0f, 22.0f, ENG::Color(0.8f, 0.8f, 0.8f));
    }
  }
}
