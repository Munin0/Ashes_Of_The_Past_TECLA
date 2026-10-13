/// | ------------------------------------ |
#include "HighScores.hpp"
/// | ------------------------------------ |
#include "Engine/Utils/Log.hpp"
#include "Engine/Utils/Path.hpp"
/// | ------------------------------------ |
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <system_error>
/// | ------------------------------------ |

namespace APP
{
  // Quita espacios y saltos de linea de los extremos.
  static std::string Trim(const std::string& s)
  {
    const auto first = s.find_first_not_of(" \t\r\n");
    if(first == std::string::npos)
      return {};
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
  }

  std::filesystem::path HighScores::GetFilePath(void)
  {
    return ENG::Path::Get().DataPath / "scores.txt";
  }

  void HighScores::SortAndTrim(void)
  {
    std::sort(m_entries.begin(), m_entries.end(),
      [](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });

    if(m_entries.size() > MaxEntries)
      m_entries.resize(MaxEntries);
  }

  void HighScores::Load(void)
  {
    m_entries.clear();

    std::ifstream in(GetFilePath());
    if(!in.is_open())
      return;

    std::string line;
    while(std::getline(in, line))
    {
      const auto sep = line.find('|');
      if(sep == std::string::npos)
        continue;

      const std::string name  = Trim(line.substr(0, sep));
      const std::string value = Trim(line.substr(sep + 1));
      if(name.empty() || value.empty())
        continue;

      uint32_t score = 0;
      try
      {
        score = static_cast<uint32_t>(std::stoul(value));
      }
      catch(...)
      {
        continue;
      }

      m_entries.push_back({name, score});
    }

    SortAndTrim();
  }

  void HighScores::Add(const std::string& name, uint32_t score)
  {
    const std::filesystem::path file = GetFilePath();

    std::error_code ec;
    std::filesystem::create_directories(file.parent_path(), ec);

    std::ofstream out(file, std::ios::out | std::ios::app);
    if(!out.is_open())
    {
      LOG_ERROR(" | << No se pudo abrir el archivo de puntajes: " + file.string());
      return;
    }

    out << name << " | " << score << '\n';
    LOG_INFO(" | << Puntaje guardado: " + name + " | " + std::to_string(score));

    m_entries.push_back({name, score});
    SortAndTrim();
  }
}
