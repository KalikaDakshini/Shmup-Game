#ifndef HELPERS_H
#define HELPERS_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <vector>

namespace kalika
{
  using std::size_t;

  inline static sf::Vector2f AXIS_X = {1.F, 0.F};

  namespace internal
  {
    /**
     * @brief Movement information of the object
     */
    struct Movable {
      // Orientation
      sf::Vector2f up = sf::Vector2f({0.0F, -1.0F});
      sf::Vector2f right = sf::Vector2f({1.0F, 0.0F});

      // Phase vector
      sf::Vector2f pos;
      sf::Vector2f vel;
      sf::Vector2f strength;
    };

    struct Drawable {
      // Player data
      sf::Sprite sprite;

      Drawable(sf::Texture& texture) : sprite(texture) {}
    };

    // Load texture given a file
    inline void load_texture(sf::Texture& t, std::filesystem::path path)
    {
      (void)t.loadFromFile(path);
      t.setSmooth(false);
    }

  }  //namespace internal

  // Forward declarations
  struct Player;

  /**
   * @brief Current state of the world
   */
  struct WorldContext {
    sf::Clock& timer;
    sf::FloatRect world_size;
    Player& player;
    size_t& frame_count;

    /**
     * @brief Return time elapsed
     */
    float cur_time() const { return timer.getElapsedTime().asSeconds(); }
  };

  /**
   * @brief Check equality between floating point variables given a
   * threshold
   */
  inline bool equal(float f1, float f2, float threshold = 1e-3)
  {
    return fabsf(f1 - f2) < threshold;
  }

  /**
   * @brief Better normalize function for sf::Vector
   */
  template<typename T>
  std::optional<sf::Vector2<T>> normalize(sf::Vector2<T> vec)
  {
    if (vec.lengthSquared() > 0) {
      return vec.normalized();
    }

    return {};
  }
}  //namespace kalika

#endif
