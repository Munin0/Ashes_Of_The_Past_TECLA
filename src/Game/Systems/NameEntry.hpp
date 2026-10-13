/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Render/Camera/Camera2D.hpp"
#include "Game/Systems/HighScores.hpp"
/// | ------------------------------------ |
#include <cstdint>
#include <string>
/// | ------------------------------------ |

namespace APP
{
  /**
   * @brief Menu de fin de partida estilo maquinita.
   *
   * Cuando el jugador muere, esta clase toma el control: muestra "GAME OVER"
   * con tres letras (A | B | C) que se pueden cambiar, y al confirmar guarda
   * el "Nombre | Puntuaje" usando HighScores.
   *
   * La escena solo necesita abrirlo al morir, pasarle los inputs, dibujarlo y
   * consultar WantsExit() para saber cuando volver al menu.
   */
  class NameEntry
  {
    public:
      /// @brief Estado del menu de ingreso.
      enum class State { Inactive, Editing, Saved };

      /// @brief Abre el menu con el puntaje obtenido y lo reinicia.
      void Open(uint32_t score);

      /// @brief Procesa el teclado (flechas, letras, Enter).
      void Input(float dt);

      /// @brief Dibuja el overlay de fin de partida.
      void Render(const ENG::Camera2D& cam) const;

      /// @brief true mientras el menu esta visible (editando o ya guardado).
      bool IsActive(void) const { return m_state != State::Inactive; }

      /// @brief Estado actual del menu.
      State GetState(void) const { return m_state; }

      /// @brief true cuando el jugador confirma volver al menu tras guardar.
      bool WantsExit(void) const { return m_exit; }

    private:
      /// @brief Nombre de 3 letras actual.
      std::string NameString(void) const;

      State    m_state = State::Inactive;
      char     m_initials[3] = {'A', 'A', 'A'};
      int      m_selected = 0;
      uint32_t m_score = 0;
      bool     m_exit = false;

      HighScores m_highScores;
  };
}
