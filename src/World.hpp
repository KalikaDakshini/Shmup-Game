#ifndef WORLD_H
#define WORLD_H

#include <SFML/Window.hpp>

#include <Event/GameEvent.hpp>
#include <Object/Bullet.hpp>
#include <Object/Player.hpp>
#include <Object/Pool.hpp>

namespace kalika
{
  struct World {
    // Player Object
    Player player;
    EventBus* bus;

    World(PlayerInfo info, EventBus* e_bus) :
      player(info, e_bus), bus(e_bus)
    {}

    /**
     * @brief Update the state of objects
     */
    void update(WorldContext const& ctx, float dt);

    /**
     * @brief Draw the objects to window
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Spawn bullet object
     */
    template<typename... Args> void spawn_bullet(Args... args)
    {
      this->bullets_.acquire(args...);
    }

    /**
     * @brief Despawn bullet object
     */
    void despawn_bullet(size_t idx) { this->bullets_.release(idx); }

    size_t bullet_capacity() const { return this->bullets_.capacity(); }

    // template<typename... Args> void spawn_enemy(Args... args)
    // {
    //   this->enemies.acquire(args...);
    // }

  private:
    // Object Pools
    Pool<Bullet> bullets_;

    // Pool<Enemy> enemies_;
  };
}  //namespace kalika

#endif
