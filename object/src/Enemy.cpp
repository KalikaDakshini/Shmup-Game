#include "Enemy.hpp"

namespace kalika
{
  sf::Texture& enemy_texture()
  {
    static sf::Texture tex;
    load_texture(tex, "resources/Enemies/Dasher.png");
    return tex;
  }

  void Enemy::update(WorldContext const& ctx, float dt)
  {
    auto [px, py] = this->sprite.getPosition();

    this->move(ctx, dt);
    // Flip direction if going out of bounds
    if (this->bound(ctx.world_size)) {
      this->mov.setVelocity(
        std::visit(
          [&ctx, this](auto const& var) {
            return var.bound_velocity(
              ctx, this->sprite.getPosition(), this->mov.velocity()
            );
          },
          this->behaviour_
        )
      );
    }
  }
};  //namespace kalika
