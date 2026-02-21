#ifndef ENEMY_H
#define ENEMY_H

#include "ObjBase.hpp"
#include "helpers.hpp"

namespace kalika
{
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

    /**
     * @brief Return the texture associated with the enemy
     */
    template<typename EnemyType> static sf::Texture& texture()
    {
      static sf::Texture tex;
      load_texture(tex, EnemyType::tex_path);
      return tex;
    }

    template<typename Type>
    static ObjInfo create(sf::Vector2f pos, sf::Vector2f vel_dir)
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
