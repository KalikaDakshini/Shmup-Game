#ifndef WORLD_H
#define WORLD_H

#include <functional>
#include <vector>

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
    // Event Bus
    EventBus* bus;

    World(PlayerInfo info, EventBus* e_bus) :
      player(info, e_bus), bus(e_bus)
    {}

    /**
     * @brief Add a bullet to the world
     */
    void add_bullet(Wrapper<Bullet> const& b_wrap)
    {
      this->bullets_.push_back(b_wrap);
    }

    /**
     * @brief Update the state of objects
     */
    void update(GameContext const& ctx, float dt);

    using SpriteRef = std::reference_wrapper<sf::Sprite const>;

    std::vector<SpriteRef> sprites() const;

    /**
     * @brief Number of active bullets on screen
     */
    size_t bullet_count() const { return this->bullets_.size(); }

    // template<typename... Args> void spawn_enemy(Args... args)
    // {
    //   this->enemies.acquire(args...);
    // }

  private:
    // Object Pools
    std::vector<Wrapper<Bullet>> bullets_;

    // Pool<Enemy> enemies_;
  };
}  //namespace kalika

#endif
