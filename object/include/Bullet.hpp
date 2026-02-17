#ifndef BULLET_H
#define BULLET_H

#include <type_traits>

#include "ObjBase.hpp"
#include "helpers.hpp"

namespace kalika
{
  struct Bullet : internal::ObjBase<Bullet> {
    // Lifetime of the object
    float lifetime_;

    // Constructor
    Bullet(ObjInfo const& info) :
      ObjBase<Bullet>(info), lifetime_(info.lifetime)
    {
      static_assert(std::is_move_assignable_v<internal::Movable>);
    }

    /**
     * @brief Update the object status by a frame
     */
    void update(WorldContext const& ctx, float dt);

    // Check if the bullet is alive and toggle it
    void set_alive(WorldContext const& ctx);

    // Rebuild an inactive object
    void rebuild(ObjInfo const& info);
  };
}  //namespace kalika

#endif
