#include "SFMLApp.hpp"

#include <iostream>
#include <random>

namespace kalika
{
  // Constructor
  SFMLApp::SFMLApp(sf::Vector2u dimensions) :
    window_(
      sf::VideoMode(dimensions),
      "Twin Stick",
      sf::Style::Titlebar | sf::Style::Close
    ),
    up({0.F, -1.F}),
    player_(build_player(
      cast<unsigned int, float>(dimensions / 2U),
      static_cast<float>(dimensions.y) / 5.F
    ))
  {
    // Window configuration
    this->window_.setFramerateLimit(60);
    // TODO(kalika): Compute this instead of hardcoding it
    this->window_.setPosition(
      cast<unsigned int, int>(
        (sf::VideoMode::getDesktopMode().size - dimensions) / 2U
      )
    );
    this->window_.setKeyRepeatEnabled(false);

    // Anti-aliasing
    this->settings.antiAliasingLevel = 8;

    // Logs configuration
    this->log_text_.setFillColor(sf::Color::Yellow);

    if (sf::Joystick::isConnected(0)) {
      this->update_log("Joystick 0 connected");
    }
  }

  // Run the SFMLApp
  void SFMLApp::run()
  {
    sf::Clock timer;

    // Window loop
    while (this->window_.isOpen()) {
      // Timer
      float const dt = timer.restart().asSeconds();

      // Clean slate
      this->window_.clear();

      // Handle events
      this->window_.handleEvents([this](auto const& event) {
        this->handle(event);
      });

      // Move the objects
      this->player_.move(dt);

      // Draw objects
      this->window_.draw(this->player_.body);
      this->window_.draw(this->player_.reticle);

      // Log messages to window
      this->log();

      // Render window
      this->window_.display();
    }
  }

  // Log messages
  void SFMLApp::log()
  {
    // Draw the contents of the log to the window
    for (std::size_t i = 0; i < this->logs_.size(); ++i) {
      this->log_text_.setPosition(
        {50.F, static_cast<float>(i * 20) + 50.F}
      );
      this->log_text_.setString(this->logs_[i]);
      this->window_.draw(this->log_text_);
    }
  }

  // Update message logs
  void SFMLApp::update_log(std::string const& text)
  {
    // Upper limit on messages
    if (this->logs_.size() == 12U) {
      this->logs_.pop_front();
    }

    this->logs_.emplace_back(text);
  }

  // Handle closing events
  void SFMLApp::handle(sf::Event::Closed const&)
  {
    this->window_.close();
  }

  // Key Press event
  void SFMLApp::handle(sf::Event::KeyPressed const& event)
  {
    if (event.code == sf::Keyboard::Key::Escape) {
      this->window_.close();
    }
  }

  void SFMLApp::handle(sf::Event::JoystickButtonPressed const& event)
  {
    this->update_log(std::format("Pressed Button: {}", event.button));
  }

  template<typename T>
  void
  SFMLApp::deadzone(sf::Vector2<T>& in, sf::Vector2<T> out, float zone)
  {
    if (out.lengthSquared() >= zone * zone) {
      in.x = out.x;
      in.y = out.y;
    }
    else {
      in.x = 0;
      in.y = 0;
    }
  }

  void SFMLApp::handle(sf::Event::JoystickMoved const& event)
  {
    auto print_vec = [](auto const& id, auto const& vec) {
      return std::format("{}: {}, {}\n", id, vec.x, vec.y);
    };

    this->update_log(
      print_vec("Player", this->player_.body.getPosition())
    );
    this->update_log(
      print_vec("Reticle", this->player_.reticle.getPosition())
    );

    // Control movement
    float const pos = event.position;
    if (event.axis == sf::Joystick::Axis::X) {
      this->deadzone(
        this->player_.mov.strength, {pos, this->player_.mov.strength.y}
      );
    }
    else if (event.axis == sf::Joystick::Axis::Y) {
      this->deadzone(
        this->player_.mov.strength, {this->player_.mov.strength.x, pos}
      );
    }
    // Control direction
    if (event.axis == sf::Joystick::Axis::U) {
      this->deadzone(
        this->player_.shoot.strength, {pos, this->player_.shoot.strength.y}
      );
    }
    else if (event.axis == sf::Joystick::Axis::V) {
      this->deadzone(
        this->player_.shoot.strength, {this->player_.shoot.strength.x, pos}
      );
    }
  }

  // All remaining events
  void SFMLApp::handle(auto const&)
  {
    // this->update_log("Unhandled Event\n");
  }

}  //namespace kalika
