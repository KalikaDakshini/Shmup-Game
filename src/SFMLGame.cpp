#include <SFMLGame.hpp>
#include <iostream>

namespace kalika
{
  // Constructor
  SFMLGame::SFMLGame(sf::Vector2u dimensions, char const* title) :
    window_(dimensions, title, &(this->bus_)),
    world_(
      {
        // Phase
        .position = sf::Vector2<float>(dimensions / 2U),
        .velocity = sf::Vector2f(sf::Vector2u(dimensions.x / 5, 0U)),
        .dir = sf::Vector2f(0.0F, -1.0F),
        // Textures
        .player_tex = internal::body_texture(),
        .reticle_tex = internal::reticle_texture(),
        // Size
        .size = 72.F,
        // Reticle information
        .radius = static_cast<float>(dimensions.y) / 4.F,
        .responsiveness = 4.F,
      },
      &(this->bus_)
    ),
    ctx(
      // Clock
      this->clock_,
      // World Boundary
      {sf::Vector2f(dimensions) * 0.05F, sf::Vector2f(dimensions) * 0.95F},
      // Player
      this->world_.player,
      // Frame count
      this->frame_count_
    )
  {}

  // Run the game
  void SFMLGame::run()
  {
    float last_stamp = 0.0F;
    // Game loop
    while (this->window_.is_active()) {
      // Timer data
      this->dt_ = this->clock_.getElapsedTime().asSeconds() - last_stamp;
      last_stamp = this->clock_.getElapsedTime().asSeconds();
      this->frame_count_++;

      // 1. Handle input events
      this->window_.handle_input();
      // 2. Process game events
      this->process_events();
      // 3. Update world;
      this->world_.update(this->ctx, this->dt_);
      // 4. Draw world
      this->window_.draw(this->world_.sprites());
    }
  }

  // Process events
  void SFMLGame::process_events()
  {
    while (!this->bus_.empty()) {
      this->bus_.front().visit([this](auto& arg) { this->handle(arg); });
      this->bus_.pop();
    }
  }

  // Update context
  void SFMLGame::update_ctx()
  {}

  // Get the player object
  Player& SFMLGame::player()
  {
    return this->world_.player;
  }

  // Move player
  void SFMLGame::handle(GameEvent::MoveEvent event)
  {
    this->player().set_strength(event.l_strength, event.r_strength);
  }

  // Spawn Bullets
  void SFMLGame::handle(GameEvent::FireEvent event)
  {
    auto const& wrapper = this->bullets_.acquire(
      event.position,
      event.velocity,
      event.texture,
      event.size,
      event.lifetime,
      get_behaviour<Dasher>(),
      &(this->bus_)
    );

    this->world_.add_bullet(wrapper);
  }

  // Release Bullets
  void SFMLGame::handle(GameEvent::ReleaseEvent event)
  {
    this->bullets_.release(event.idx);
  }

  // Spawn Enemies
  void SFMLGame::handle(GameEvent::SpawnEvent)
  {}

  namespace internal
  {
    // Get player texture
    sf::Texture& body_texture()
    {
      static sf::Texture t;
      load_texture(t, "resources/player.png");
      return t;
    }

    // Get reticle texture
    sf::Texture& reticle_texture()
    {
      static sf::Texture t;
      load_texture(t, "resources/reticle.png");
      return t;
    }
  }  //namespace internal

}  //namespace kalika
