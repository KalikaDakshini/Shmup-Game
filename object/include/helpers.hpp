#ifndef HELPERS_H
#define HELPERS_H

#include <SFML/System.hpp>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

namespace kalika
{
  using std::size_t;

  // Forward declarations
  struct Player;
  struct Bullet;

  inline bool equal(float f1, float f2, float threshold = 1e-3)
  {
    return fabsf(f1 - f2) < threshold;
  }

  inline void load_texture(sf::Texture& t, std::filesystem::path path)
  {
    (void)t.loadFromFile(path);
    t.setSmooth(false);
  }

  /**
   * @brief Current state of the world
   */
  struct WorldContext {
    sf::Clock& timer;
    sf::FloatRect world_size;
    Player& player;
    size_t& frame_count;
    std::vector<sf::Vector2f> enemy_pos;

    float cur_time() const { return timer.getElapsedTime().asSeconds(); }
  };

  namespace internal
  {
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

  }  //namespace internal
}  //namespace kalika

#endif
