#ifndef ENEMY_H
#define ENEMY_H

#include "ObjBase.hpp"
#include "helpers.hpp"

namespace kalika
{
  // Texture Cache
  sf::Texture& enemy_texture();

  /**
   * @brief Class representing an enemy object
   */
  struct Enemy : internal::ObjBase<Enemy> {
    // Create an enemy
    static Enemy create(ObjInfo info) { return {info}; }

    /**
     * @brief Check if object is alive
     */
    void set_alive(WorldContext const& ctx)
    {
      (void)ctx;
      this->alive_ = (this->health_ > 0.F);
    }

    /**
     * @brief Update the object by one frame
     */
    void update(WorldContext const& ctx, float dt);

  private:
    float health_;

    // Private constructor
    Enemy(ObjInfo info) : ObjBase(info), health_(info.health) {}
  };
}  //namespace kalika

#endif
