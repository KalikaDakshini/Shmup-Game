#include <Object/ObjBase.hpp>

namespace kalika::internal
{

  ObjBase::ObjBase(
    sf::Vector2f position,
    sf::Vector2f velocity,
    sf::Vector2f dir,
    sf::Texture& sprite_tex,
    float obj_size,
    EventBus* bus
  ) :
    bus_(bus), draw_(sprite_tex)
  {
    // Customize sprite
    auto size = sf::Vector2f(this->sprite().getTexture().getSize());
    this->sprite().setTextureRect({{}, sf::Vector2i(size)});
    this->sprite().setOrigin(size / 2.F);
    float const s = obj_size / size.y;
    this->sprite().scale({s, s});

    // Set Phase
    this->mov_.pos = position;
    this->mov_.vel = velocity;
    this->mov_.up =
      (dir.lengthSquared() > 0) ? dir : sf::Vector2f(0.F, -1.F);

    // Update the frame of the ship
    this->update_frame();
  }

  // Move the derived object by a frame
  void ObjBase::move(
    WorldContext const& ctx, internal::Movable const& target, float dt
  )
  {
    // Swap frames for animations
    if (animate_) {
      this->animate(ctx);
    }

    // Update kinetic data
    auto accel = std::visit(
      [&target, this](auto const& var) {
        return var.accel(this->mov_, target);
      },
      *(this->behaviour_)
    );

    this->mov_.pos += this->velocity() * dt;
    this->mov_.vel = this->velocity() + accel * dt;

    this->update_frame();
  }

  // Updates co-ordinate data
  void ObjBase::update_frame()
  {
    // Normalize co-ordinate axes
    this->mov_.up = normalize(this->mov_.up).value_or(this->forward());
    this->mov_.right = this->mov_.up.perpendicular();

    // Move the sprite
    this->draw_.sprite.setPosition(this->position());
    this->draw_.sprite.setRotation(this->forward().angleTo(AXIS_X) * -1.F);
  }

  void ObjBase::animate(WorldContext const& ctx)
  {
    // Get the frame number
    auto frame_num = (ctx.frame_count / this->interval_) % frame_count_;
    auto xs = (frame_num == 0) ? 0UL : fx * frame_num - 1;
    // Set the sprite to the matching frame
    this->set_frame(sf::Vector2<int>(xs, 0UL), sf::Vector2<int>(fx, fy));
  }
}  //namespace kalika::internal
