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
    ///tamano de la barra de acceso rapido
    constexpr int QuickSlots = 10;
    ///tamano del inventario interno
    constexpr int InventorySlots = 50;
    ///tamano total del inventario contando la barra de acceso rapido
    constexpr int totalInventory = QuickSlots + InventorySlots; 

    using ItemId = uint32_t;

    ///definicion de un item
    struct ItemDef
    {
      std::string name;
      float weight = 0.0f;
      uint16_t maxStack = 1;
    };
    ///estructura del slot del inventario
    struct Slot
    {
      ItemId id = 0;
      uint16_t count = 0;
    };

    ///un diccionario de los items con su id y definicion
    using ItemDefMap = std::unordered_map<ItemId, ItemDef>;


  class Inventory : public ENG::IComponents
  {
    private:
        std::vector<Slot> slots;

    public:
      Inventory() : slots(totalInventory) {}
      ///agrega los items al inventario y devuelve los que no cupieron o 0 si cupo todo
      uint16_t AddItem(ItemId id, const ItemDefMap& items, uint16_t count = 1);
      ///elimina items de tu inventario y devuelve lo eliminado
      uint16_t RemoveItem(ItemId id, uint16_t count = 1);
      ///regresa la cantidad de items que hay, podria usarse en una tienda, sistema de misiones o crafteo
      uint32_t CountItem(ItemId id) const;
      ///verifica si no hay casillas vacias
      bool IsFull() const;
      ///Regresa todos los slots en orden
      const std::vector<Slot>& GetSlots() const;
        
  };
}
