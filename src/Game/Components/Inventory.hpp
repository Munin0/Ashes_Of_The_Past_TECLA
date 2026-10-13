/// | ------------------------------------ |
#pragma once
/// | ------------------------------------ |
#include "Engine/Component/Component.hpp"
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
/// | ------------------------------------ |

namespace APP
{
  using ItemId = uint32_t;

  ///definicion de un item
  struct Item
  {
    std::string name;
    std::string atlas;
    float weight = 0.0f;
    uint16_t maxStack = 1;
  };
  ///estructura del slot del inventario
  struct Slot
  {
    ItemId id = 0;
    uint16_t count = 0;
  };

  class Inventory : public ENG::IComponents
  {
    public:
      Inventory(int quickSlots, int maxInvSlots)
        : m_quickSlots{quickSlots}, m_maxInvSlots{maxInvSlots}
      {
        slots.reserve(m_maxInvSlots);
      }

      ///agrega los items al inventario y devuelve los que no cupieron o 0 si cupo todo
      bool AddItem(ItemId id, uint16_t count = 1);
      ///elimina items de tu inventario y devuelve lo eliminado
      uint16_t RemoveItem(ItemId id, uint16_t count = 1);
      ///regresa la cantidad de items que hay, podria usarse en una tienda, sistema de misiones o crafteo
      uint32_t CountItem(ItemId id) const;
      ///verifica si no hay casillas vacias
      bool IsFull() const;
      ///Regresa todos los slots en orden
      const std::vector<Slot>& GetSlots() const;
    private:
      int m_quickSlots;
      int m_maxInvSlots;

      std::unordered_map<ItemId, Item> items;
      std::vector<Slot> slots;
  };
}
