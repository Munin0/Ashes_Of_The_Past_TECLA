/// | ------------------------------------ |
#include "Utils.hpp"
#include <random>
/// | ------------------------------------ |
/// | ------------------------------------ |

namespace ENG
{
  int GetRandomNumber(const int& a, const int& b)
  {
    static thread_local std::mt19937 gen{ std::random_device{}() };
    std::uniform_int_distribution<> dist(a, b);
    return dist(gen);
  }
  
  float GetRandomNumber(const float& a, const float& b)
  {
    static thread_local std::mt19937 gen{ std::random_device{}() };
    std::uniform_real_distribution<> dist(a, b);
    return dist(gen);
  }
}
