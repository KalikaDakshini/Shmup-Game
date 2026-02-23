#include "FireMode.hpp"

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
    auto [px, _] = sf::Vector2f(p.body().getTexture().getSize());

    auto const start_pos = p.position();
    std::array<sf::Vector2f, RapidFire::count> const pos = {
      start_pos + p.forward().rotatedBy(sf::degrees(45)) * px,
      start_pos + p.forward().rotatedBy(sf::degrees(-45)) * px,
    };

    for (auto idx = 0UL; idx < RapidFire::count; idx++) {
      infos.emplace_back(
        get_behaviour<Dasher>(),
        bul_size,
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
    auto [px, _] = sf::Vector2f(p.body().getTexture().getSize());

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
      p.body().getPosition() + (dir[0]) * px,
      p.body().getPosition() + (dir[1]) * px,
      p.body().getPosition() + (dir[2]) * px,
      p.body().getPosition() + (dir[3]) * px,
      p.body().getPosition() + (dir[4]) * px / 2.F
    };

    // 2. Generate info and add bullets
    for (auto idx = 0UL; idx < SpreadFire::count; idx++) {
      auto const new_vel = this->velocity * dir[idx];
      infos.emplace_back(
        get_behaviour<Dasher>(),
        bul_size,
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
    auto [px, _] = sf::Vector2f(p.body().getTexture().getSize());

    // 2. Generate info and add bullets
    auto const new_vel = this->velocity * p.forward();
    auto angle = toggle ? sf::degrees(45) : sf::degrees(-45);

    infos.emplace_back(
      get_behaviour<Chaser>(),
      bul_size,
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
