#include <algorithm>

#include <Object/World.hpp>

namespace kalika
{
  // Update the state of objects
  void World::update(GameContext const& ctx, float dt)
  {
    // Update player
    this->player.update(ctx, dt);
    if (this->player.shoot.strength.lengthSquared() > 0) {
      this->player.fire(ctx, dt);
    }

    // Update bullets
    for (auto& bullet : bullets_) {
      bullet->update(ctx, dt);
      if (!bullet->is_alive()) {
        bus->emplace(GameEvent::ReleaseEvent{bullet.idx});
      }
    }

    // Update current cache
    std::erase_if(this->bullets_, [](auto& bullet) {
      return !bullet->is_alive();
    });
  }

  std::vector<World::SpriteRef> World::sprites() const
  {
    std::vector<SpriteRef> container;
    container.emplace_back(player.sprite());
    container.emplace_back(player.reticle_sprite());
    for (auto const& bullet : this->bullets_) {
      container.emplace_back(bullet->sprite());
    }

    return container;
  }
}  //namespace kalika
