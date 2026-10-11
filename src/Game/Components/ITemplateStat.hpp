#pragma once

#include "Engine/Component/Component.hpp"


namespace APP
{
  template <typename T>
  class ITemplateStat : public ENG::IComponents
  {

    public:
        ITemplateStat(T max, T min, T current, std::string name);

        explicit ITemplateStat(std::string name);

        ITemplateStat(T min, T current, std::string name);

        ~ITemplateStat() override = default;

        T getCurrent() const;

        T getMax() const;

        T getMin() const;

        void setCurrent(T newCurrent);

        void setMax(T newMax);

        void setMin(T newMin);

    protected:
        std::string stat_name;
        T max_value{};
        T current_value{};
        T min_value{};
  };
}
