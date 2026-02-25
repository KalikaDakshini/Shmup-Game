#include <Object/Bullet.hpp>

namespace kalika
{
  Bullet::Bullet(GameEvent::FireEvent event, EventBus* bus) :
    ObjBase(
      event.position,
      event.velocity,
      event.velocity.normalized(),
      event.texture,
      event.size,
      bus
    ),
    lifetime_(event.lifetime)
  {
    this->behaviour_ = get_behaviour(event.behaviour_id);
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
  void Bullet::rebuild(GameEvent::FireEvent event, EventBus* bus)
  {
    this->mov_.pos = event.position;
    this->mov_.vel = event.velocity;
    this->draw_.sprite.setTexture(event.texture);
    this->lifetime_ = event.lifetime;
    this->behaviour_ = get_behaviour(event.behaviour_id);
    this->bus_ = bus;
    this->mov_.up = event.velocity.normalized();
    // TODO(kalika): Implement bullet size change if needed
    (void)bul_size;

    this->update_frame();
  }
}  //namespace kalika
