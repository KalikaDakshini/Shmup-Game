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
    // Orientation
    sf::Vector2f up = sf::Vector2f({0.0F, -1.0F});
    sf::Vector2f right = sf::Vector2f({1.0F, 0.0F});

    // Movement
    float velocity = 3.0f;
    sf::Vector2f strength;
  };

  struct Shootable {
    sf::Vector2f strength;
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
     * @brief Return the forward direction
     */
    sf::Vector2f const& forward() const { return this->mov.up; }

    /**
     * @brief Move and orient the ship in the corresponding direction
     */
    void move(float dt);

    /**
     * @brief Update frame of reference
     * @param pos New position to be moved to
     */
    void update_frame(sf::Vector2f const& offset)
    {
      // Rotate the frame
      if (fabsf(this->mov.up.lengthSquared() - 1.f) < 1e-6) {
        this->mov.up = this->mov.up.normalized();
      }
      this->mov.right = this->mov.up.perpendicular();

      // Transform the ship
      this->body.move(offset);
      this->body.setRotation(
        this->mov.up.angle() + sf::radians(M_PIf / 2)
      );

      // Move the reticle
      this->reticle.setPosition(
        this->body.getPosition() + this->forward() * this->shoot.radius
      );
    }

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
