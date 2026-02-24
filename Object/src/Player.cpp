#include <vector>

#include <Object/Player.hpp>
#include <Object/helpers.hpp>

namespace kalika
{

  namespace internal
  {
    sf::Texture& bullet_texture()
    {
      static sf::Texture t;
      load_texture(t, "resources/bullet.png");
      return t;
    }
  }  //namespace internal

  // ====== Player Functions ====== //
  // Constructor
  Player::Player(PlayerInfo info, EventBus* bus) :
    ObjBase(
      info.position,
      info.velocity,
      info.dir,
      info.player_tex,
      info.size,
      bus
    ),
    shoot(info.reticle_tex)
  {
    // Store magnitude of velocity
    this->vel_ = this->velocity().length();

    // Reticle position
    this->shoot.radius = info.radius;
    this->shoot.resp = info.responsiveness;
    this->shoot.cur_offset = this->shoot.radius * this->mov_.up;

    // Size and colour
    this->shoot.sprite.scale({3.0F, 3.0F});
    this->shoot.sprite.setColor(sf::Color::Transparent);

    // Set default fire mode
    this->set_mode<RapidFire>();

    // Update the frame of the ship
    this->update_frame();
  }

  // Move the player
  void Player::update(GameContext const& ctx, float dt)
  {
    // Transform the co-ordinate frame to the new position
    this->update_body(ctx, dt);
    this->update_reticle(dt);
  }

  // Update body status
  void Player::update_body(GameContext const& ctx, float dt)
  {
    // Rotate the frame
    this->mov_.up =
      normalize(this->shoot.strength).value_or(this->forward());

    // Transform the ship
    this->mov_.vel = normalize(this->strength).value_or({}) * this->vel_;

    // Update phase
    auto disp = this->velocity() * dt;
    this->mov_.pos = this->bind(ctx.world_size, disp);

    // Update co-ordinate frames
    this->update_frame();
  }

  // Update reticle position
  void Player::update_reticle(float dt)
  {
    // Set reticle position
    auto const ret_disp = normalize(this->shoot.strength).value_or({});

    // Simulate reticle displacement to lag behind the ship
    auto const par_disp = ret_disp.projectedOnto(this->forward());
    auto const per_disp = ret_disp - par_disp;
    auto const disp_vector = (0.5F * par_disp) + (0.4F * per_disp);
    auto const target_offset =
      (disp_vector + this->forward()) * this->shoot.radius;

    this->shoot.cur_offset +=
      (target_offset - this->shoot.cur_offset) * this->shoot.resp * dt;

    // Set reticle's position
    this->shoot.sprite.setPosition(
      this->position() + this->shoot.cur_offset
    );

    // Set active state of the reticle
    if ((this->shoot.strength.lengthSquared() > 0.F)) {
      this->active_ = true;
    }
    else if (this->active_ &&
             equal(
               this->shoot.cur_offset.length(), this->shoot.radius, 2.0F
             ) &&
             this->shoot.update_timer(dt)) {
      this->active_ = false;
      this->shoot.reset_timer();
    }

    // Set reticle display on
    if (this->active_) {
      this->shoot.sprite.setColor(sf::Color::Cyan);
    }
    else {
      this->shoot.sprite.setColor(sf::Color::Transparent);
    }
  }

  // Bind position to world boundary
  sf::Vector2f Player::bind(sf::FloatRect bounds, sf::Vector2f disp) const
  {
    auto new_pos = this->position() + disp;
    return bounds.contains(new_pos) ? new_pos : this->position();
  }

}  //namespace kalika
