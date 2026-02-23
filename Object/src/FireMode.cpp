#include <Object/Player.hpp>

namespace kalika
{
  // Rapid fire bullets
  void RapidFire::fire(WorldContext const& ctx, float dt, EventBus* bus)
  {
    auto& p = ctx.player;
    // Spawn bullets if cooldown has passed
    if (this->spawn_check(dt)) {
      this->spawn = false;
      // 1. Set positions of bullets
      // Assume texture is a square
      auto [px, _] = sf::Vector2f(p.sprite().getTexture().getSize());

      auto const start_pos = p.position();
      std::array<sf::Vector2f, RapidFire::count> const pos = {
        start_pos + p.forward().rotatedBy(sf::degrees(45)) * px,
        start_pos + p.forward().rotatedBy(sf::degrees(-45)) * px,
      };

      // Push events for spawning bullets
      for (auto idx = 0UL; idx < RapidFire::count; idx++) {
        bus->emplace(
          GameEvent::FireEvent{
            .size = bul_size,
            .texture = internal::bullet_texture(),
            .position = pos[idx],
            .velocity = this->velocity * p.forward(),
            .lifetime = this->lifetime,
          }
        );
      }
    }
  }

  // Rapid fire bullets
  void SpreadFire::fire(WorldContext const& ctx, float dt, EventBus* bus)
  {
    auto& p = ctx.player;
    // Spawn bullets if offset has passed
    if (this->spawn_check(dt)) {
      this->spawn = false;
      // 1. Set positions of bullets
      // Assume texture is a square
      auto [px, _] = sf::Vector2f(p.sprite().getTexture().getSize());

      // Set directions
      std::array<sf::Vector2f, SpreadFire::count> const dir = {
        p.forward().rotatedBy(sf::degrees(-15)),
        p.forward().rotatedBy(sf::degrees(-30)),
        p.forward().rotatedBy(sf::degrees(15)),
        p.forward().rotatedBy(sf::degrees(30)),
        p.forward(),
      };

      // Set positions
      std::array<sf::Vector2f, SpreadFire::count> const pos = {
        p.position() + (dir[0]) * px,
        p.position() + (dir[1]) * px,
        p.position() + (dir[2]) * px,
        p.position() + (dir[3]) * px,
        p.position() + (dir[4]) * px / 2.F
      };

      // 2. Generate info and add bullets
      for (auto idx = 0UL; idx < SpreadFire::count; idx++) {
        bus->emplace(
          GameEvent::FireEvent{
            .size = bul_size,
            .texture = internal::bullet_texture(),
            .position = pos[idx],
            .velocity = this->velocity * dir[idx],
            .lifetime = this->lifetime,
          }
        );
      }
    }
  }

  // Homing fire bullets
  void ChaserFire::fire(WorldContext const& ctx, float dt, EventBus* bus)
  {
    auto& p = ctx.player;
    // Spawn bullets if offset has passed
    if (this->spawn_check(dt)) {
      // 1. Set positions of bullets
      auto [px, _] = sf::Vector2f(p.sprite().getTexture().getSize());

      // 2. Generate info and add bullets
      auto angle = this->toggle_ ? sf::degrees(45) : sf::degrees(-45);
      bus->emplace(
        GameEvent::FireEvent{
          .size = bul_size,
          .texture = internal::bullet_texture(),
          .position = p.position() + p.forward().rotatedBy(angle) * px,
          .velocity = this->velocity * p.forward(),
          .lifetime = this->lifetime,
        }
      );

      // Toggle Positions
      this->toggle_ != this->toggle_;
    }
  }

}  //namespace kalika
