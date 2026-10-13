#pragma once
/// | ------------------------------------ |
#include "Engine/Object/Object.hpp"
#include "Engine/Render/Batching/RBatch.hpp"
#include "Engine/Render/Camera/Camera2D.hpp"
/// | ------------------------------------ |

namespace APP
{
  [[maybe_unused]] void EssenceBar(ENG::Batcher& batcher, const ENG::Camera2D& cam, ENG::Object& who);
}
