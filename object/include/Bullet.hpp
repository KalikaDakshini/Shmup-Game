#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>
#include <array>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <variant>

#include "helpers.hpp"

namespace kalika
{
  enum class BulletType : std::uint8_t {
    STRAIGHT,
    HOMING,
    CLUSTER,
    NUM_TYPES
  };

  // Homing Bullet
  struct Homing {
    internal::Tracker track;

    sf::Vector2f accel(SteerContext const& ctx) const
    {
      return this->track.accel(ctx);
    }
  };

  // Straight Bullet
  struct Straight {
    sf::Vector2f accel(SteerContext const&) const { return {}; }
  };

  /**
   * @brief Bullet object
   */
  struct Bullet {
    sf::Sprite sprite;
    internal::Movable mov;

    // Constructor
    Bullet(ObjInfo<Bullet> info);

    using type = BulletType;

    /**
     * @brief Update the kinetic data
     */
    void update(
      WorldContext const& wld_ctx, SteerContext const& str_ctx, float dt
    );

    /**
     * @brief Rebuild an object using info
     */
    void rebuild(ObjInfo<Bullet> info);

    /**
     * @brief Check if bullet is still alive
     */
    bool isAlive() const { return this->alive; }

    // Factory function for making a Bullet
    static Bullet create(ObjInfo<Bullet> info);

  private:
    bool alive = true;
    float lifetime;
    using BulletVariant = std::variant<Homing, Straight>;
    BulletVariant behaviour;

    // ====== Helper functions ====== //
    void update_frame();

    // Check if bullet is alive
    void set_alive(WorldContext const& ctx);

    void set_behaviour(BulletType type);
  };

  /**
   * @brief Specialized template for bullet
   */

  template<> struct ObjInfo<Bullet> {
    Bullet::type obj_type;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime = 1.0F;
  };

  /**
   * @brief Texture Cache
   */
  struct TextureStore {
    static sf::Texture& get_tex(BulletType type)
    {
      static std::array<
        sf::Texture,
        static_cast<size_t>(BulletType::NUM_TYPES)>
        textures = [] {
          std::
            array<sf::Texture, static_cast<size_t>(BulletType::NUM_TYPES)>
              texs;
          (void)texs[static_cast<size_t>(BulletType::STRAIGHT)]
            .loadFromFile("resources/bullet.png");
          (void)texs[static_cast<size_t>(BulletType::CLUSTER)]
            .loadFromFile("resources/bullet.png");
          (void)texs[static_cast<size_t>(BulletType::HOMING)].loadFromFile(
            "resources/bullet.png"
          );
          return texs;
        }();
      return textures[static_cast<size_t>(type)];
    }
  };

}  //namespace kalika

#endif
