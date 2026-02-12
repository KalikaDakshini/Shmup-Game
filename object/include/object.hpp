#ifndef OBJECT_H
#define OBJECT_H

#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

namespace kalika
{
  struct Movable {
    float velocity;
    sf::Vector2f strength;
  };

  struct Shootable {
    sf::Vector2f strength;
    sf::Vector2f prev_strength;
    float angle;
    float radius;
  };

  struct Player {
  private:
    sf::Texture body_tex_;
    sf::Texture reticle_tex_;

  public:
    sf::Sprite body;
    sf::Sprite reticle;
    Movable mov;
    Shootable shoot;

    // Constructor
    Player(
      std::filesystem::path const& body_path,
      std::filesystem::path const& reticle_path
    );

    /**
     * @brief Move and orient the ship in the corresponding direction
     */
    void move(float dt);

    /**
     * @brief Crop the sprite to texture
     */
    void crop(sf::Vector2i start, sf::Vector2i end)
    {
      this->body.setTextureRect(sf::IntRect(start, end));
    }
  };

  // Helper functions
  // Build player
  Player
  build_player(sf::Vector2f position, float radius, float velocity = 3.0F);

  // Cast other vectors to float
  template<typename T, typename U>
  inline static constexpr sf::Vector2<U> cast(sf::Vector2<T> const& vec)
  {
    return {static_cast<U>(vec.x), static_cast<U>(vec.y)};
  }

}  // namespace kalika

#endif
