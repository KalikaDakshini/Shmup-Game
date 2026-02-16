#ifndef HELPERS_H
#define HELPERS_H

#include <SFML/System.hpp>
#include <cstddef>
#include <iostream>
#include <optional>

namespace kalika
{
  using std::size_t;

  /**
   * @brief Context view of the world to steer objects
   */
  struct SteerContext {
    using enemy_pos = sf::Vector2f;
    std::vector<enemy_pos> positions;
  };

  /**
   * @brief Current state of the world
   */
  struct WorldContext {
    sf::FloatRect world_size;
    sf::Clock& timer;

    WorldContext(sf::Vector2u size, sf::Clock& clock) :
      world_size(sf::Vector2f(size) * 0.05F, sf::Vector2f(size)),
      timer(clock)
    {}

    int32_t cur_time() const
    {
      return timer.getElapsedTime().asMilliseconds();
    }
  };

  // Object information to pass for construction
  template<typename Object> struct ObjInfo {
    Object::type obj_type;
    sf::Vector2f position;
    float velocity;
    sf::Vector2f dir;
  };

  namespace internal
  {
    constexpr float EPSILON = 1e-6;

    /**
     * @brief Movement information of the object
     */
    struct Movable {
      // Orientation
      sf::Vector2f up = sf::Vector2f({0.0F, -1.0F});
      sf::Vector2f right = sf::Vector2f({1.0F, 0.0F});

      // Movement
      float vel_scale;
      sf::Vector2f vel_dir;
      sf::Vector2f strength;

      sf::Vector2f velocity() const { return this->vel_dir * vel_scale; }

      // Note: Do not use for player
      void setVelocity(sf::Vector2f velocity)
      {
        if (velocity.lengthSquared() == 0) {
          this->vel_scale = 0;
          this->vel_dir = {};
          return;
        }
        this->vel_scale = velocity.length();
        this->vel_dir = velocity.normalized();
        this->up = this->vel_dir;
      }
    };

    /**
     * @brief Homing behaviour
     */
    struct Tracker {
      // TODO(kalika): Implement this
      sf::Vector2f accel(SteerContext const& ctx) const
      {
        (void)ctx;
        return {};
      }
    };

  }  //namespace internal
}  //namespace kalika

#endif
