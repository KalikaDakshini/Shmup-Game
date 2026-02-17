#include "Player.hpp"

#include <iostream>
#include <vector>

namespace kalika
{
  namespace internal
  {
    sf::Texture& body_texture()
    {
      static sf::Texture t;
      (void)t.loadFromFile("resources/player.png");
      return t;
    }

    sf::Texture& reticle_texture()
    {
      static sf::Texture t;
      (void)t.loadFromFile("resources/reticle.png");
      return t;
    }

    sf::Texture& bullet_texture()
    {
      static sf::Texture t;
      (void)t.loadFromFile("resources/bullet.png");
      return t;
    }
  }  // namespace internal

  // ====== Player Functions ====== //
  // Constructor
  Player::Player(PlayerInfo info) :
    body(internal::body_texture()), reticle(internal::reticle_texture())
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

    // Reticle position
    this->shoot.radius = info.radius;
    this->shoot.resp = info.responsiveness;
    this->shoot.cur_offset = this->shoot.radius * this->mov.up;

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
    this->update_body(ctx, dt);
    this->update_reticle(dt);
  }

  void Player::update_body(WorldContext const& ctx, float dt)
  {
    // Rotate the frame
    if (this->shoot.strength.lengthSquared() > 0.0F) {
      this->mov.up = this->shoot.strength;
    }
    if (!equal(this->shoot.strength.lengthSquared(), 1.0F)) {
      this->mov.up = this->mov.up.normalized();
    }
    this->mov.right = this->mov.up.perpendicular().normalized();

    // Transform the ship
    if (this->mov.strength.lengthSquared() > 0.0F) {
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
  }

  void Player::update_reticle(float dt)
  {
    // Set reticle position
    sf::Vector2f ret_disp;
    if (this->mov.strength.lengthSquared() > 0) {
      ret_disp = -this->mov.strength.normalized();
    }

    // Simulate reticle displacement to lag behind the ship
    auto const par_disp = ret_disp.projectedOnto(this->mov.up);
    auto const per_disp = ret_disp - par_disp;
    auto const disp_vector = (0.5F * par_disp) + (0.4F * per_disp);
    auto const target_offset =
      (disp_vector + this->mov.up) * this->shoot.radius;

    this->shoot.cur_offset +=
      (target_offset - this->shoot.cur_offset) * this->shoot.resp * dt;

    // Set reticle's position
    this->reticle.setPosition(
      this->body.getPosition() + this->shoot.cur_offset
    );

    // Set active state of the reticle
    this->reticle.setColor(sf::Color::Transparent);
    if ((this->shoot.strength.lengthSquared() > 0.F)) {
      this->is_active() = true;
    }
    else if (this->is_active() &&
             equal(
               this->shoot.cur_offset.length(), this->shoot.radius, 2.0F
             ) &&
             this->shoot.update_timer(dt)) {
      this->is_active() = false;
      this->shoot.reset_timer();
    }

    // Set reticle display on
    if (this->is_active()) {
      this->reticle.setColor(sf::Color::Cyan);
    }
  }

  void Player::bind(sf::FloatRect bounds, sf::Vector2f& disp)
  {
    auto new_pos = this->body.getPosition() + disp;
    if (!bounds.contains(new_pos)) {
      disp = {};
    }
  }

  // Fire ship's guns
  std::vector<ObjInfo>
  Player::fire(WorldContext const& ctx, float dt) const
  {
    return this->fire_mode(ctx, dt);
  }

  // Rapid fire bullets
  std::vector<ObjInfo> RapidFire::fire(WorldContext const& ctx, float dt)
  {
    auto& p = ctx.player;
    std::vector<ObjInfo> infos;
    // Spawn bullets if cooldown has passed
    if (this->spawn_check(dt)) {
      this->spawn = false;
      // 1. Set positions of bullets
      // Assume texture is a square
      auto [px, _] = sf::Vector2f(p.body.getTexture().getSize());

      auto const start_pos = p.position();
      std::array<sf::Vector2f, RapidFire::count> const pos = {
        start_pos + p.forward().rotatedBy(sf::degrees(45)) * px,
        start_pos + p.forward().rotatedBy(sf::degrees(-45)) * px,
      };

      for (auto idx = 0UL; idx < RapidFire::count; idx++) {
        infos.emplace_back(
          get_behaviour<Straight>(),
          internal::bullet_texture(),
          pos[idx],
          this->velocity * p.forward(),
          this->lifetime
        );
      }
    }

    return infos;
  }

  // Rapid fire bullets
  std::vector<ObjInfo> SpreadFire::fire(WorldContext const& ctx, float dt)
  {
    auto& p = ctx.player;
    std::vector<ObjInfo> infos;
    // Spawn bullets if offset has passed
    if (this->spawn_check(dt)) {
      this->spawn = false;
      // 1. Set positions of bullets
      // Assume texture is a square
      auto [px, _] = sf::Vector2f(p.body.getTexture().getSize());

      // Set directions
      std::array<sf::Vector2f, SpreadFire::count> const dir = {
        p.forward().rotatedBy(sf::degrees(-14)),
        p.forward().rotatedBy(sf::degrees(-30)),
        p.forward().rotatedBy(sf::degrees(15)),
        p.forward().rotatedBy(sf::degrees(30)),
        p.forward(),
      };

      // Set positions
      std::array<sf::Vector2f, SpreadFire::count> const pos = {
        p.body.getPosition() + (dir[0]) * px,
        p.body.getPosition() + (dir[1]) * px,
        p.body.getPosition() + (dir[2]) * px,
        p.body.getPosition() + (dir[3]) * px,
        p.body.getPosition() + (dir[4]) * px / 2.F
      };

      // 2. Generate info and add bullets
      for (auto idx = 0UL; idx < SpreadFire::count; idx++) {
        auto const new_vel = this->velocity * dir[idx];
        infos.emplace_back(
          get_behaviour<Straight>(),
          internal::bullet_texture(),
          pos[idx],
          new_vel,
          this->lifetime
        );
      }
    }

    return infos;
  }

  // Homing fire bullets
  std::vector<ObjInfo> ChaserFire::fire(WorldContext const& ctx, float dt)
  {
    auto& p = ctx.player;
    std::vector<ObjInfo> infos;
    // Spawn bullets if offset has passed
    if (this->spawn_check(dt)) {
      // 1. Set positions of bullets
      auto [px, _] = sf::Vector2f(p.body.getTexture().getSize());

      // 2. Generate info and add bullets
      auto const new_vel = this->velocity * p.forward();
      auto angle = toggle ? sf::degrees(45) : sf::degrees(-45);

      infos.emplace_back(
        get_behaviour<Chaser>(),
        internal::bullet_texture(),
        p.position() + p.forward().rotatedBy(angle) * px,
        new_vel,
        this->lifetime
      );

      // Toggle Positions
      toggle = !toggle;
    }

    return infos;
  }

  // Build player object
  Player Player::create(PlayerInfo info)
  {
    return info;
  }

}  //namespace kalika
