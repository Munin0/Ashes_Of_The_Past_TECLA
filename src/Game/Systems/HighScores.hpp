/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
/// | ------------------------------------ |

namespace APP
{
  /// @brief Una fila de la tabla de puntuaciones.
  struct HighScoreEntry
  {
    std::string name;
    uint32_t    score;
  };

  /**
   * @brief Sistema de guardado y lectura de las mejores puntuaciones.
   *
   * Lee y escribe el archivo de puntajes ("data/scores.txt") con el formato
   * "Nombre | Puntuaje" (una entrada por linea). Al cargar, ordena de mayor a
   * menor y conserva solo las 10 mejores.
   */
  class HighScores
  {
    public:
      /// @brief Cuantas puntuaciones se conservan como maximo.
      static constexpr uint32_t MaxEntries = 10;

      /// @brief Lee el archivo de puntajes y llena la lista (solo las 10 mejores).
      void Load(void);

      /// @brief Agrega una puntuacion al archivo y a la lista en memoria.
      void Add(const std::string& name, uint32_t score);

      /// @brief Entradas cargadas, ordenadas de mayor a menor puntaje.
      const std::vector<HighScoreEntry>& GetEntries(void) const { return m_entries; }

      /// @brief Ruta del archivo de puntajes.
      static std::filesystem::path GetFilePath(void);

    private:
      /// @brief Ordena de mayor a menor y recorta a MaxEntries.
      void SortAndTrim(void);

      std::vector<HighScoreEntry> m_entries;
  };
}
