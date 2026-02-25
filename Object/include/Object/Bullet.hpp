#ifndef BULLET_H
#define BULLET_H

#include <type_traits>

#include <Event/GameEvent.hpp>
#include <Object/ObjBase.hpp>

namespace kalika
{
  struct Bullet : internal::ObjBase {
    // Lifetime of the object
    float lifetime_;

    // Constructor
    Bullet(GameEvent::FireEvent event, EventBus* bus);

    /**
     * @brief Update the object status by a frame
     */
    void update(GameContext const& ctx, float dt);

    // Rebuild an inactive object
    void rebuild(GameEvent::FireEvent event, EventBus* bus);

  private:
    // Check if the bullet is alive and toggle it
    void check_alive(GameContext const& ctx);
    // // Find the enemy closest to the bullet and retrieve its target
    // internal::Movable const& find_closest(GameContext const& ctx)
    // {
    //   return ctx.enemies[0].mov;
    // }
  };
}  //namespace kalika

#endif
