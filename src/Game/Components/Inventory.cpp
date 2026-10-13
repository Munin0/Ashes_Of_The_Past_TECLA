/// | ------------------------------------ |
#include "Inventory.hpp"
/// | ------------------------------------|
#include <algorithm>
#include <cstdint>
#include <vector>
/// | ------------------------------------|

namespace APP
{
  const std::vector<Slot>& Inventory::GetSlots() const
  {
    return slots;
  }

  bool Inventory::IsFull() const
  {
    for(const Slot& s : slots)
    {
      if(s.id == 0)
        return false;
    }
    return true;
  }

  uint32_t Inventory::CountItem(ItemId id) const
  {
    if(id == 0) return 0;

    uint32_t totalItems = 0;

    for(const Slot& s : slots)
    {
      if(id == s.id)
      {
        totalItems += s.count;
      }
    }
    return totalItems;
  }

  uint16_t Inventory::RemoveItem(ItemId id, uint16_t count)
  {
    if(id == 0) return 0;

    uint16_t itemsCount = count;

    for(Slot& s : slots)
    {
      if(id == s.id)
      {
        uint16_t toRemove = std::min(itemsCount, s.count);
        s.count -= toRemove;
        itemsCount -= toRemove;
        if(s.count == 0) s.id = 0;
        if(itemsCount == 0) break;
      }
    }

    return count - itemsCount;
  }

  bool Inventory::AddItem(ItemId id, uint16_t count)
  {
    if(id == 0) return count;
    if(count == 0) return 0;

    auto it = items.find(id);
    if(it == items.end()) return count;
    uint16_t maxStack = it->second.maxStack;
    if(maxStack == 0) return count;

    uint16_t itemsCount = count;

    for(Slot& s : slots)
    {
      if(id == s.id)
      {
        if(s.count < maxStack)
        {
          uint16_t remainingSpace = maxStack - s.count;
          uint16_t toMove = std::min(itemsCount, remainingSpace);
          s.count += toMove;
          itemsCount -= toMove;
          if(itemsCount == 0) return itemsCount;
        }
      }
    }

    for(Slot& s: slots)
    {
      if(s.id == 0)
      {
        s.id = id;
        uint16_t toMove = std::min(itemsCount, maxStack);
        s.count = toMove;
        itemsCount -= toMove;
        if(itemsCount == 0) return itemsCount;
      }
    }

    return itemsCount;
  }
}
