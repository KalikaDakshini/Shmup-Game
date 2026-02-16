#include "Player.hpp"

#include <iostream>
#include <vector>

namespace kalika
{

  // ====== Player Functions ====== //
  // Constructor
  Player::Player(PlayerInfo info) :
    body(body_texture()), reticle(reticle_texture())
  {
    // Load sprites
    auto const tex_size = this->body.getTexture().getSize();
    this->body.setOrigin(sf::Vector2<float>(tex_size / 2U));
    this->reticle.setOrigin(
      sf::Vector2<float>(this->reticle.getTexture().getSize() / 2U)
    );

    // Set movement options
    this->body.setPosition(info.position);
    this->mov.up = info.dir.normalized();
    this->mov.right = info.dir.perpendicular().normalized();
    this->mov.vel_dir = info.vel_dir;
    this->mov.vel_scale = info.vel_scale;
    this->shoot.radius = info.radius;

    // Size and colour
    this->body.scale({2.0F, 2.0F});
    this->reticle.scale({3.0F, 3.0F});
    this->reticle.setColor(sf::Color::Cyan);

    // Set default fire mode
    this->set_mode<RapidFire>();
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
    if (!equal(this->shoot.strength.lengthSquared(), 0.0F)) {
      this->mov.up = this->shoot.strength;
    }
    if (!equal(this->shoot.strength.lengthSquared(), 1.0F)) {
      this->mov.up = this->mov.up.normalized();
    }
    this->mov.right = this->mov.up.perpendicular().normalized();

    // Transform the ship
    if (this->mov.strength.lengthSquared() > 0) {
      this->mov.vel_dir = this->mov.strength.normalized();
    }
    else {
      this->mov.vel_dir = {};
    }
    // Bind displacement to world boundaries
    auto disp = this->mov.velocity() * dt;
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

  // Fire ship's guns
  std::vector<ObjInfo<Bullet>>
  Player::fire(WorldContext const& ctx, float dt) const
  {
    return this->fire_mode(ctx, dt);
  }

  // Rapid fire bullets
  std::vector<ObjInfo<Bullet>>
  RapidFire::fire(WorldContext const& ctx, float dt)
  {
    if (this->elapsed < this->cooldown) {
      this->elapsed += 1;
    }
    else {
      this->elapsed = 0;
      this->spawn = true;
    }

    auto& p = ctx.player;
    std::vector<ObjInfo<Bullet>> infos;
    // Spawn bullets if cooldown has passed
    if (this->spawn) {
      this->spawn = false;
      // 1. Set positions of bullets
      // Assume texture is a square
      auto [px, _] = sf::Vector2f(p.body.getTexture().getSize());

      auto const start_pos = p.position();
      std::array<sf::Vector2f, RapidFire::count> const pos = {
        start_pos + (p.forward() + p.right()) * px * norm2,
        start_pos + (p.forward() - p.right()) * px * norm2,
      };

      auto dir = p.forward();
      auto const new_vel = this->velocity * dir + p.right_vel();

      for (auto idx = 0UL; idx < RapidFire::count; idx++) {
        infos.emplace_back(
          BulletType::STRAIGHT, pos[idx], new_vel, this->lifetime
        );
      }
    }

    return infos;
  }

  // Rapid fire bullets
  std::vector<ObjInfo<Bullet>>
  SpreadFire::fire(WorldContext const& ctx, float dt)
  {
    if (this->elapsed < this->cooldown) {
      this->elapsed += 1;
    }
    else {
      this->elapsed = 0;
      this->spawn = true;
    }

    auto& p = ctx.player;
    std::vector<ObjInfo<Bullet>> infos;
    // Spawn bullets if offset has passed
    if (this->spawn) {
      this->spawn = false;
      // 1. Set positions of bullets
      // Assume texture is a square
      auto [px, _] = sf::Vector2f(p.body.getTexture().getSize());

      // Set directions
      std::array<sf::Vector2f, SpreadFire::count> const dir = {
        (p.forward() + p.right() / 2.F) * norm1,
        (p.forward() + p.right()) * norm2,
        (p.forward() - p.right() / 2.F) * norm1,
        (p.forward() - p.right()) * norm2,
        p.forward(),
      };

      // Set positions
      std::array<sf::Vector2f, SpreadFire::count> const pos = {
        p.body.getPosition() + (dir[0]) * px * norm1,
        p.body.getPosition() + (dir[1]) * px * norm2,
        p.body.getPosition() + (dir[2]) * px * norm1,
        p.body.getPosition() + (dir[3]) * px * norm2,
        p.body.getPosition() + (dir[4]) * px / 2.F
      };

      // 2. Generate info and add bullets
      for (auto idx = 0UL; idx < SpreadFire::count; idx++) {
        auto const new_vel = this->velocity * dir[idx] + p.right_vel();
        infos.emplace_back(
          BulletType::STRAIGHT, pos[idx], new_vel, this->lifetime
        );
      }
    }

    return infos;
  }

  // Homing fire bullets
  std::vector<ObjInfo<Bullet>>
  HomingFire::fire(WorldContext const& ctx, float dt)
  {
    if (this->elapsed < this->cooldown) {
      this->elapsed += 1;
    }
    else {
      this->elapsed = 0;
      this->spawn = true;
    }

    auto& p = ctx.player;
    std::vector<ObjInfo<Bullet>> infos;
    // Spawn bullets if offset has passed
    if (spawn) {
      spawn = false;
      // 1. Set positions of bullets
      auto [px, _] = sf::Vector2f(p.body.getTexture().getSize());

      auto dir1 = (p.forward() + p.right()) * norm2;
      auto dir2 = (p.forward() - p.right()) * norm2;

      std::array<sf::Vector2f, HomingFire::count> const pos = {
        p.body.getPosition() + dir1 * px / 2.F,
        p.body.getPosition() + dir2 * px / 2.F
      };

      // 2. Generate info and add bullets
      auto const new_vel = this->velocity * p.forward() + p.right_vel();
      if (toggle) {
        infos.emplace_back(
          BulletType::HOMING, pos[0], new_vel, this->lifetime
        );
      }
      else {
        infos.emplace_back(
          BulletType::HOMING, pos[1], new_vel, this->lifetime
        );
      }

      // Toggle Positions
      toggle = !toggle;
    }

    return infos;
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
  Player Player::create(PlayerInfo info)
  {
    return info;
  }

}  //namespace kalika
