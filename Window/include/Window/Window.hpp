#ifndef WINDOW_H
#define WINDOW_H

#include <format>
#include <functional>
#include <optional>

#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/System.hpp>

#include <Event/GameEvent.hpp>

namespace kalika
{
  /**
   * @brief Application class
   */
  struct SFMLWindow {
    // Constructor
    SFMLWindow(sf::Vector2u dimensions, char const* title, EventBus* bus);

    /**
     * @brief Run the application
     */
    bool is_active() { return this->window_.isOpen(); }

    /**
     * @brief Draw all objects in the world
     *
     *
     * @param world
     */
    using SpriteRef = std::reference_wrapper<sf::Sprite const>;
    /**
     * @brief Draw the sprites given
     */
    void draw(std::vector<SpriteRef> const& sprites);
    /**
     * @brief Handle user input through SFML events
     */
    void handle_input();

  private:
    // Window information
    sf::RenderWindow window_;
    sf::ContextSettings settings_;

    // Log information
    sf::Font const font_{"resources/tuffy.ttf"};
    sf::Text log_text_{font_};
    std::deque<std::string> logs_;

    // World info
    sf::Vector2f x_axis_;

    // Game Event Handler
    EventBus* bus_ = nullptr;

    // Remember stick positions
    sf::Vector2f l_strength_;
    sf::Vector2f r_strength_;

    // ======== Helper functions ======== //

    // Log to window
    void log();
    // Update logs
    void update_log(std::string const& text);

    // Clamp deadzone
    template<typename T>
    sf::Vector2<T> deadzone(sf::Vector2<T> strength, float zone = 20.F);

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
