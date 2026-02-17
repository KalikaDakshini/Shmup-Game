#include "Bullet.hpp"

namespace kalika
{
  // Update the bullet by frame
  void Bullet::update(WorldContext const& ctx, float dt)
  {
    this->lifetime_ -= dt;
    this->move(ctx, dt);
  }

  // Check if the bullet is alive
  void Bullet::set_alive(WorldContext const& ctx)
  {
    bool const area_check =
      ctx.world_size.contains(this->sprite.getPosition());
    bool const time_check = this->lifetime_ > 0;

    this->alive_ = (area_check && time_check);
  }

  // Rebuild bullet object
  void Bullet::rebuild(ObjInfo const& info)
  {
    ObjBase<Bullet>::rebuild(info);
    this->lifetime_ = info.lifetime;
  }
}  //namespace kalika
