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

#include "Bullet.hpp"
#include "Object.hpp"
#include "Player.hpp"

namespace kalika
{
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

    // Objects
    Player player_;
    ObjectPool<Bullet> bullet_pool_;

    // Context objects
    WorldContext wld_ctx;
    SteerContext str_ctx;

    // ======== Helper functions ======== //
    // Log to window
    void log();
    // Update logs
    void update_log(std::string const& text);

    // Clamp deadzone
    template<typename T>
    void
    deadzone(sf::Vector2<T>& in, sf::Vector2<T> out, float zone = 20.F);

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
