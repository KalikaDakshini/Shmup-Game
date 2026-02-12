#include "object.hpp"

#include <filesystem>
#include <iostream>

namespace kalika
{
  constexpr float EPSILON = 10.F;

  // Constructor
  Player::Player(
    std::filesystem::path const& body_path,
    std::filesystem::path const& reticle_path
  ) :
    body_tex_(body_path),
    reticle_tex_(reticle_path),
    body(body_tex_),
    reticle(reticle_tex_)
  {
    sf::Vector2i const size({13, 13});
    // Set base texture
    this->crop({0, 0}, size);

    // Load sprites
    this->body.setOrigin(cast<int, float>(size / 2));
    this->reticle.setOrigin(
      cast<unsigned int, float>(this->reticle_tex_.getSize() / 2U)
    );
  }

  // Move the player
  void Player::move(float dt)
  {
    // Move according to L-stick direction
    sf::Vector2f const l_offset =
      this->mov.strength * this->mov.velocity * dt;
    this->body.move(l_offset);

    // TODO(kalika): aiming direction should persist
    // Aim in the R-stick direction
    sf::Vector2f const r_offset = this->shoot.strength;
    if (r_offset.lengthSquared() > 0) {
      this->body.setRotation(
        sf::Vector2f({-r_offset.y, r_offset.x}).angle()
      );
      this->shoot.prev_strength = this->shoot.strength;
    }

    // Set reticle position according to r_stick
    sf::Vector2f const apparent_dir =
      this->shoot.prev_strength.normalized();
    this->reticle.setPosition(
      this->body.getPosition() + apparent_dir * this->shoot.radius
    );
  }

  // Build player object
  Player build_player(sf::Vector2f position, float radius, float velocity)
  {
    Player player{"resources/player.png", "resources/reticle.png"};

    // Initial player config
    player.body.setPosition(position);
    player.body.scale({4.0F, 4.0F});

    // Initial reticle config
    player.shoot.radius = radius;
    player.shoot.strength = sf::Vector2f(100, sf::degrees(-90));
    player.shoot.prev_strength = sf::Vector2f(100, sf::degrees(-90));
    player.reticle.setPosition(position + sf::Vector2f({0, -radius}));
    player.reticle.scale({3.0F, 3.0F});
    player.reticle.setColor(sf::Color::Cyan);

    // Update other information
    player.mov.velocity = velocity;

    return player;
  }
}  //namespace kalika
