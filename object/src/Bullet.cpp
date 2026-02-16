#include "Bullet.hpp"

#include <cmath>
#include <utility>

namespace kalika
{
  Bullet::Bullet(ObjInfo<Bullet> info) :
    sprite(TextureStore::get_tex(info.obj_type)), lifetime(info.lifetime)
  {
    // Choose bullet behaviour
    this->set_behaviour(info.obj_type);

    // Position data
    this->sprite.setPosition(info.position);
    this->sprite.scale({2.5F, 2.5F});

    this->sprite.setOrigin(
      sf::Vector2<float>(this->sprite.getTexture().getSize() / 2U)
    );

    // Orientation data
    this->mov.setVelocity(info.velocity);
    this->update_frame();
  }

  // Update bullet per frame
  void Bullet::update(WorldContext const& wld_ctx, float dt)
  {
    // Update co-ordinate frame
    this->update_frame();

    // Update lifetime
    this->lifetime -= dt;

    // Update kinetic data
    auto accel = std::visit(
      [&wld_ctx](auto const& var) { return var.accel(wld_ctx); },
      this->behaviour
    );
    this->mov.setVelocity(this->mov.velocity() + accel * dt);
    auto disp = this->mov.velocity() * dt;
    this->sprite.move(disp);

    // Rotate the bullet
    this->sprite.setRotation(
      this->mov.up.angle() + sf::radians(M_PIf / 2)
    );

    this->set_alive(wld_ctx);
  }

  // Rebuild a bullet using info
  void Bullet::rebuild(ObjInfo<Bullet> info)
  {
    // Toggle alive status
    this->alive = true;

    // Set parameters
    this->set_behaviour(info.obj_type);
    this->mov.setVelocity(info.velocity);
    this->sprite.setPosition(info.position);
    this->lifetime = info.lifetime;

    // Update frame of reference
    this->update_frame();
  }

  // Updates co-ordinate data
  void Bullet::update_frame()
  {
    if (!equal(this->mov.up.lengthSquared(), 1.0F)) {
      this->mov.up = this->mov.up.normalized();
    }
    this->mov.right = this->mov.up.perpendicular().normalized();
  }

  void Bullet::set_alive(WorldContext const& ctx)
  {
    bool const area_check =
      ctx.world_size.contains(this->sprite.getPosition());
    bool const time_check = this->lifetime > 0;

    alive = (area_check && time_check);
  }

  void Bullet::set_behaviour(BulletType type)
  {
    if (type == BulletType::HOMING) {
      behaviour = Homing();
    }
    else {
      behaviour = Straight();
    }
  }

  Bullet Bullet::create(ObjInfo<Bullet> info)
  {
    return {info};
  }

}  //namespace kalika
