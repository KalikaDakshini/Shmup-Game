#ifndef SFML_APP_H
#define SFML_APP_H

#include <cmath>
#include <deque>
#include <format>
#include <optional>
#include <utility>

#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/System.hpp>

#include <Event/GameEvent.hpp>
#include <Object/Player.hpp>
#include <Object/Pool.hpp>

namespace kalika
{
  namespace internal
  {
    // Texture Cache
    sf::Texture& body_texture();

    sf::Texture& reticle_texture();

    sf::Texture& bullet_texture();
  }  // namespace internal

  /**
   * @brief Application class
   */
  struct SFMLApp {
    // Constructor
    SFMLApp(
      sf::Vector2u dimensions = {1280U, 800U},
      char const* title = "Smol Shmup"
    );

    /**
     * @brief Run the application
     */
    void run();

  private:
    // Window information
    float dt = 0.0F;
    size_t frame_count = 0UL;
    sf::Clock clock_;
    sf::RenderWindow window_;
    sf::ContextSettings settings;

    // Log information
    sf::Font const font_{"resources/tuffy.ttf"};
    sf::Text log_text_{font_};
    std::deque<std::string> logs_;

    // World info
    sf::Vector2f up;

    // Game Event Handler
    EventBus bus_;

    // Remember stick positions
    sf::Vector2f l_strength;
    sf::Vector2f r_strength;

    // Objects
    Player player_;
    // Pool<Enemy> enemies_;
    // Pool<Bullet> bullets_;

    // Context objects
    WorldContext wld_ctx;

    // ======== Helper functions ======== //
    // Log to window
    void log();
    // Update logs
    void update_log(std::string const& text);
    // Update context
    void update_ctx();

    // Clamp deadzone
    template<typename T>
    sf::Vector2<T> deadzone(sf::Vector2<T> strength, float zone = 20.F);

    // Handling JoystickHeld event
    void fire_check();
    // Handle closing events
    void handle(sf::Event::Closed const&);
    // Key Press event
    void handle(sf::Event::KeyPressed const& event);
    // Joystick Button event
    void handle(sf::Event::JoystickButtonPressed const& event);
    // Joystick Moved event
    void handle(sf::Event::JoystickMoved const& event);
    // All remaining events
    void handle(auto const&);
  };

}  //namespace kalika

#endif
