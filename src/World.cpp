#include <World.hpp>

namespace kalika
{
  // Update the state of objects
  void World::update(WorldContext const& ctx, float dt)
  {
    // Update player
    this->player.update(ctx, dt);
    if (this->player.shoot.strength.lengthSquared() > 0) {
      this->player.fire(ctx, dt);
    }

    // Update bullets
    for (Bullet& bullet : bullets_) {
      bullet.update(ctx, dt);
    }
  }

  // Draw the objects to window
  void World::draw(sf::RenderWindow& window)
  {
    window.draw(this->player.sprite());
    for (auto const& bullet : bullets_) {
      window.draw(bullet.sprite());
    }
  }
}  //namespace kalika
