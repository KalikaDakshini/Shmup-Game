#include <Object/Enemy.hpp>

namespace kalika
{

  void Enemy::update(GameContext const& ctx, float dt)
  {
    auto [px, py] = this->sprite().getPosition();

    this->move(ctx, ctx.player.movable(), dt);
    // Flip direction if going out of bounds
    if (this->bound(ctx.world_size)) {
      this->mov.setVelocity(
        std::visit(
          [&ctx, this](auto const& var) {
            return var.bound_velocity(
              ctx, this->sprite().getPosition(), this->mov.velocity()
            );
          },
          this->behaviour_
        )
      );
    }
  }
};  //namespace kalika
