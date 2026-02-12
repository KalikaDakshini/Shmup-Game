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
    // Move along the L-stick direction in the global frame
    sf::Vector2f const l_offset =
      this->mov.strength * this->mov.velocity * dt;

    // Turn the frame towards the R-stick
    sf::Vector2f const r_offset = this->shoot.strength;
    if (r_offset.lengthSquared() > 0) {
      this->mov.up = r_offset;
    }

    // Transform the co-ordinate frame to the new position
    this->update_frame(l_offset);
  }

  // Build player object
  Player build_player(sf::Vector2f position, float radius, float velocity)
  {
    Player player{"resources/player.png", "resources/reticle.png"};

    // Player config
    player.body.scale({4.0F, 4.0F});
    player.reticle.scale({3.0F, 3.0F});
    player.shoot.radius = radius;
    player.mov.velocity = velocity;
    player.reticle.setColor(sf::Color::Cyan);

    // Update frame of reference
    player.update_frame(position);

    return player;
  }
}  //namespace kalika
