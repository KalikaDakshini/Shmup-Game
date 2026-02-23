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
    void check_alive(WorldContext const& ctx);

    // Rebuild an inactive object
    void rebuild(
      size_t idx,
      sf::Vector2f position,
      sf::Vector2f velocity,
      sf::Texture& tex,
      float bul_size,
      float lifetime,
      Behaviour behaviour,
      EventBus* bus
    );

    // Constructor
    Bullet(
      size_t idx,
      sf::Vector2f position,
      sf::Vector2f velocity,
      sf::Texture& tex,
      float bul_size,
      float lifetime,
      Behaviour behaviour,
      EventBus* bus
    );

  private:
    size_t pool_id_;
    // // Find the enemy closest to the bullet and retrieve its target
    // internal::Movable const& find_closest(WorldContext const& ctx)
    // {
    //   return ctx.enemies[0].mov;
    // }
  };
}  //namespace kalika

#endif
