#include <Object/Bullet.hpp>

namespace kalika
{
  Bullet::Bullet(
    sf::Vector2f position,
    sf::Vector2f velocity,
    sf::Texture& tex,
    float bul_size,
    float lifetime,
    Behaviour behaviour,
    EventBus* bus
  ) :
    ObjBase(position, velocity, velocity.normalized(), tex, bul_size, bus),
    lifetime_(lifetime)
  {
    this->behaviour_ = behaviour;
  }

  // Update the bullet by frame
  void Bullet::update(GameContext const& ctx, float dt)
  {
    this->lifetime_ -= dt;
    // auto const& target = this->find_closest(ctx);
    auto const target = internal::Movable{};
    this->move(ctx, target, dt);
    this->check_alive(ctx);
  }

  // Check if the bullet is alive
  void Bullet::check_alive(GameContext const& ctx)
  {
    bool const area_check =
      ctx.world_size.contains(this->sprite().getPosition());
    bool const time_check = this->lifetime_ > 0;

    this->alive_ = area_check && time_check;
  }

  // Rebuild bullet object
  void Bullet::rebuild(
    sf::Vector2f position,
    sf::Vector2f velocity,
    sf::Texture& tex,
    float bul_size,
    float lifetime,
    Behaviour behaviour,
    EventBus* bus
  )
  {
    this->mov_.pos = position;
    this->mov_.vel = velocity;
    this->draw_.sprite.setTexture(tex);
    this->lifetime_ = lifetime;
    this->behaviour_ = behaviour;
    this->bus_ = bus;
    this->mov_.up = velocity.normalized();
    (void)bul_size;

    this->update_frame();
  }
}  //namespace kalika
