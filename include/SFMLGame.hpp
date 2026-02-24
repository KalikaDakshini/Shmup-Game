#ifndef SFML_APP_H
#define SFML_APP_H

#include <Event/GameEvent.hpp>
#include <Object/World.hpp>
#include <Window/Window.hpp>

namespace kalika
{
  namespace internal
  {
    // Texture Cache
    sf::Texture& body_texture();

    sf::Texture& reticle_texture();
  }  // namespace internal

  struct SFMLGame {
    // Constructor
    SFMLGame(sf::Vector2u dimensions, char const* title);

    /**
     * @brief Run the game
     */
    void run();

  private:
    EventBus bus_;
    SFMLWindow window_;
    World world_;
    Pool<Bullet> bullets_;

    GameContext ctx;

    // Timer information
    float dt_ = 0.0F;
    size_t frame_count_ = 0UL;
    sf::Clock clock_;

    // ====== Helper functions ====== //
    // Process the event bus
    void process_events();
    // Update world context
    void update_ctx();
    // Get player object
    Player& player();

    // ======= Event handlers ======= //

    // Move Player
    void handle(GameEvent::MoveEvent event);
    // Spawn Bullets
    void handle(GameEvent::FireEvent event);
    // Spawn Enemies
    void handle(GameEvent::SpawnEvent event);
    // Release Objects
    void handle(GameEvent::ReleaseEvent event);
  };

}  //namespace kalika

#endif
