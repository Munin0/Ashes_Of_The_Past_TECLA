/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include "Engine/Utils/Vector2.hpp"
/// | ------------------------------------ |

namespace APP
{
    struct Aim : public ENG::IComponents
    {
        ENG::Vector2 direction = {0.0f, 1.0f};
    };
}
