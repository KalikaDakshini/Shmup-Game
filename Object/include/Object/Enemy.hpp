#ifndef ENEMY_H
#define ENEMY_H

#include "ObjBase.hpp"
#include "Player.hpp"
#include "helpers.hpp"

namespace kalika
{
  /**
   * @brief Class representing an enemy object
   */
  struct Enemy : internal::ObjBase {
    /**
     * @brief Check if object is alive
     */
    bool is_alive() { return (this->health_ > 0.F); }

    /**
     * @brief Update the object by one frame
     */
    void update(GameContext const& ctx, float dt);

    /**
     * @brief Return the texture associated with the enemy
     */
    template<typename EnemyType> static sf::Texture& texture()
    {
      static sf::Texture tex;
      load_texture(tex, EnemyType::tex_path);
      return tex;
    }

  private:
    float health_;

    // Private constructor
    Enemy(ObjInfo info) : ObjBase(info), health_(info.health) {}
  };
}  //namespace kalika

#endif
