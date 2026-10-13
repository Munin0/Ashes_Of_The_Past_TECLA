#include "ITemplateStat.hpp"
#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace APP 
{
  template<typename T>
  ITemplateStat<T>::ITemplateStat(T max, T min, T current, std::string name)
    : stat_name(std::move(name)), max_value(max), current_value(current), min_value(min)
  {
    if (min_value > max_value)
      throw std::invalid_argument("ITemplateStat minimum cannot exceed maximum");

    current_value = std::clamp(current_value, min_value, max_value);
  }

  template<typename T>
  ITemplateStat<T>::ITemplateStat(std::string name)
    : ITemplateStat(T{}, T{}, T{}, std::move(name))
  {}

  template<typename T>
  ITemplateStat<T>::ITemplateStat(T min, T current, std::string name)
    : stat_name(std::move(name)), max_value(std::numeric_limits<T>::max()), min_value(min), current_value(current)
  {
    if (min_value > max_value)
      throw std::invalid_argument("ITemplateStat minimum cannot exceed maximum");

    current_value = std::clamp(current_value, min_value, max_value);
  }


  template<typename T>
    T ITemplateStat<T>::getCurrent() const
    {
      return current_value;
    }

  template<typename T>
    T ITemplateStat<T>::getMax() const
    {
      return max_value;
    }

  template<typename T>
    T ITemplateStat<T>::getMin() const
    {
      return min_value;
    }

  template<typename T>
    void ITemplateStat<T>::setCurrent(T newCurrent)
    {
      current_value = std::clamp(newCurrent, min_value, max_value);
    }

  template<typename T>
    void ITemplateStat<T>::setMax(T newMax)
    {
      if (newMax < min_value)
        throw std::invalid_argument("ITemplateStat maximum cannot be less than minimum");

      max_value = newMax;
      current_value = std::clamp(current_value, min_value, max_value);
    }

  template<typename T>
    void ITemplateStat<T>::setMin(T newMin)
    {
      if (newMin > max_value)
        throw std::invalid_argument("ITemplateStat minimum cannot exceed maximum");

      min_value = newMin;
      current_value = std::clamp(current_value, min_value, max_value);
    }

  template class ITemplateStat<uint8_t>;
  template class ITemplateStat<uint32_t>;
  template class ITemplateStat<uint64_t>;
  template class ITemplateStat<uint16_t>;
  template class ITemplateStat<int>;
  template class ITemplateStat<float>;
  template class ITemplateStat<double>;
}
