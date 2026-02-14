#include "Player.hpp"

#include <iostream>

namespace kalika
{
  // ====== Player Functions ====== //
  // Constructor
  Player::Player(sf::Vector2f position, float velocity, sf::Vector2f dir) :
    reticle_tex_("resources/reticle.png"),
    body_tex_("resources/player.png"),
    body(body_tex_),
    reticle(reticle_tex_)
  {
    // Load sprites
    auto const tex_size = this->body_tex_.getSize();
    this->body.setOrigin(sf::Vector2<float>(tex_size / 2U));
    this->reticle.setOrigin(
      sf::Vector2<float>(this->reticle_tex_.getSize() / 2U)
    );

    // Set movement options
    this->body.setPosition(position);
    this->mov.up = dir.normalized();
    this->mov.right = dir.perpendicular().normalized();
    this->mov.body_vel = velocity;

    // Set default fire mode
    this->set_mode(std::make_unique<RapidFire>());
  }

  // Move the player
  void Player::move(WorldContext const& ctx, float dt)
  {
    // Transform the co-ordinate frame to the new position
    this->update_frame(ctx, dt);
  }

  void Player::update_frame(WorldContext const& ctx, float dt)
  {
    // Rotate the frame
    if (this->shoot.strength.lengthSquared() > internal::EPSILON) {
      this->mov.up = this->shoot.strength;
    }
    if (fabsf(this->mov.up.lengthSquared() - 1.F) > internal::EPSILON) {
      this->mov.up = this->mov.up.normalized();
    }
    this->mov.right = this->mov.up.perpendicular().normalized();

    // Transform the ship
    sf::Vector2f dir;
    if (this->mov.strength.lengthSquared() > 0) {
      dir = this->mov.strength.normalized();
    }
    // Bind displacement to world boundaries
    auto disp = dir * this->mov.body_vel * dt;
    this->bind(ctx.world_size, disp);
    // Move the ship
    this->body.move(disp);
    this->body.setRotation(this->mov.up.angle() + sf::radians(M_PIf / 2));

    // Disable the cursor when not moving
    if (this->shoot.strength.lengthSquared() == 0) {
      this->reticle.setColor(sf::Color::Transparent);
      return;
    }

    this->reticle.setColor(sf::Color::Cyan);

    // Set reticle position
    sf::Vector2f ret_disp;
    if (this->mov.strength.lengthSquared() > 0) {
      ret_disp = -this->mov.strength.normalized();
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

  void Player::bind(sf::FloatRect bounds, sf::Vector2f& disp)
  {
    auto new_pos = this->body.getPosition() + disp;
    if (!bounds.contains(new_pos)) {
      disp = {};
    }
  }

  // Set Firing mode
  void Player::set_mode(std::unique_ptr<FireMode> mode)
  {
    this->mode_ = std::move(mode);
  }

  // Fire ship's guns
  std::vector<ObjInfo<Bullet>>
  Player::fire(WorldContext const& ctx, float dt) const
  {
    return this->mode_->fire(*this, ctx, dt);
  }

  // Rapid fire bullets
  std::vector<ObjInfo<Bullet>>
  RapidFire::fire(Player const& p, WorldContext const& ctx, float dt)
  {
    std::vector<ObjInfo<Bullet>> infos;
    // Generate bullets if not in cooldown
    if (!in_cooldown) {
      // 1. Set positions of bullets
      auto [px, py] = sf::Vector2f(p.body.getTexture().getSize());
      py /= 2.F;

      auto pos1 = p.body.getPosition() + p.forward() * px + p.right() * py;
      auto pos2 = p.body.getPosition() + p.forward() * px - p.right() * py;

      // 2. Generate info and add bullets
      infos.emplace_back(
        BulletType::STRAIGHT, pos1, this->velocity, p.forward()
      );
      infos.emplace_back(
        BulletType::STRAIGHT, pos2, this->velocity, p.forward()
      );

      // 3. Set timestamp
      this->last_stamp = ctx.cur_time();
    }
    // Toggle cooldown if passed
    // 1. Record current time
    auto const duration =
      static_cast<float>(ctx.cur_time() - this->last_stamp);
    // Set cooldown status
    this->in_cooldown = (duration / (1000 * dt)) < this->cooldown;

    return infos;
  }

  // // Spread fire bullets
  // std::vector<ObjInfo<Bullet>> SpreadFire::fire(float dt) const
  // {
  //   return {};
  // }

  // // Homing bullets
  // std::vector<ObjInfo<Bullet>> HomingFire::fire(float dt) const
  // {
  //   return {};
  // }

  // Adjust the object based on L-stick position

  sf::Vector2f Player::smoothen(sf::Vector2f vec, int factor)
  {
    auto smoothen = [&factor](float x) {
      auto xi = static_cast<int>(x);
      return static_cast<float>(xi - (xi % factor));
    };

    return {smoothen(vec.x), smoothen(vec.y)};
  }

  // Build player object
  Player
  Player::create(sf::Vector2f position, float velocity, float radius)
  {
    Player player{
      position,
      velocity,
      sf::Vector2f({0.0F, -1.0F}),
    };

    // Player config
    player.body.scale({2.0F, 2.0F});
    player.reticle.scale({3.0F, 3.0F});
    player.shoot.radius = radius;
    player.reticle.setColor(sf::Color::Cyan);

    return player;
  }

}  //namespace kalika
