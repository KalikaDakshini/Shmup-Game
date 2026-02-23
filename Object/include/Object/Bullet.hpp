#ifndef BULLET_H
#define BULLET_H

#include <type_traits>

#include <Object/ObjBase.hpp>

namespace kalika
{
  struct Bullet : internal::ObjBase {
    // Lifetime of the object
    float lifetime_;

    /**
     * @brief Update the object status by a frame
     */
    void update(WorldContext const& ctx, float dt);

    // Check if the bullet is alive and toggle it
    void set_alive(WorldContext const& ctx);

    // Rebuild an inactive object
    void rebuild();

    // Factory function to create bullets
    static Bullet create();

  private:
    // Constructor
    Bullet(ObjInfo const& info) :
      ObjBase<Bullet>(info), lifetime_(info.lifetime)
    {}

    // // Find the enemy closest to the bullet and retrieve its target
    // internal::Movable const& find_closest(WorldContext const& ctx)
    // {
    //   return ctx.enemies[0].mov;
    // }
  };
}  //namespace kalika

#endif
