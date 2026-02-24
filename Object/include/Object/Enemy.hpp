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
  struct Enemy : internal::ObjBase<Enemy> {
    // Create an enemy
    static Enemy create(ObjInfo const& info) { return info; }

    /**
     * @brief Check if object is alive
     */
    void set_alive(GameContext const& ctx)
    {
      (void)ctx;
      this->alive_ = (this->health_ > 0.F);
    }

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

    // For debugging purposes only
    template<typename Type>
    static ObjInfo spawn(sf::Vector2f pos, sf::Vector2f vel_dir)
    {
      return {
        .behaviour = get_behaviour<Type>(),
        .texture = Enemy::texture<Type>(),
        .position = pos,
        .velocity = vel_dir.normalized() * Type::abs_vel,
        .animate = true,
        .frame_count = Type::frame_count,
        .interval = Type::interval,
      };
    }

  private:
    float health_;

    // Private constructor
    Enemy(ObjInfo info) : ObjBase(info), health_(info.health) {}
  };
}  //namespace kalika

#endif
