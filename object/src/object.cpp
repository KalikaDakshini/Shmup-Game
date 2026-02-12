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
      this->mov.strength * this->mov.body_vel * dt;

    // Turn the frame towards the R-stick
    if (this->shoot.strength.lengthSquared() > 0) {
      this->mov.up = this->shoot.strength;
    }

    // Transform the co-ordinate frame to the new position
    this->update_frame(l_offset);
  }

  void Player::update_frame(sf::Vector2f const& l_offset)
  {
    // Rotate the frame
    if (fabsf(this->mov.up.lengthSquared() - 1.F) > 1e-6) {
      this->mov.up = this->mov.up.normalized();
    }
    this->mov.right = this->mov.up.perpendicular().normalized();

    // Transform the ship
    this->body.move(l_offset);
    this->body.setRotation(this->mov.up.angle() + sf::radians(M_PIf / 2));

    // Disable the cursor when not moving
    if (this->shoot.strength.lengthSquared() == 0) {
      this->reticle.setColor(sf::Color::Transparent);
      return;
    }

    this->reticle.setColor(sf::Color::Cyan);

    // Set reticle position
    sf::Vector2f ret_disp;
    if (l_offset.lengthSquared() > 0) {
      ret_disp = -l_offset.normalized();
    }

    // Scale reticle displacement to resemble an ellipse
    auto const par_disp = ret_disp.projectedOnto(this->mov.up);
    auto const per_disp = ret_disp - par_disp;
    auto const adjusted_disp =
      (4.F * par_disp + 3.F * per_disp) * this->shoot.radius / 8.F;
    auto const total_disp =
      adjusted_disp + 1.5F * this->shoot.radius * this->mov.up;

    // Move the reticle around
    this->reticle.setPosition(
      smoothen(this->body.getPosition() + total_disp, 3)
    );
  }

  sf::Vector2f Player::smoothen(sf::Vector2f vec, int factor)
  {
    auto smoothen = [&factor](float x) {
      auto xi = static_cast<int>(x);
      return static_cast<float>(xi - (xi % factor));
    };

    return {smoothen(vec.x), smoothen(vec.y)};
  }

  // Build player object
  Player build_player(sf::Vector2f position, float radius, float velocity)
  {
    Player player{"resources/player.png", "resources/reticle.png"};

    // Player config
    player.body.scale({4.0F, 4.0F});
    player.reticle.scale({3.0F, 3.0F});
    player.shoot.radius = radius;
    player.mov.body_vel = velocity;
    player.body.move(position);
    player.reticle.setColor(sf::Color::Cyan);

    return player;
  }
}  //namespace kalika
