#ifndef SFML_APP_H
#define SFML_APP_H

#include <deque>
#include <format>
#include <optional>

#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>

/**
 * @brief Application class
 */
struct SFMLApp {
  // Constructor
  SFMLApp();

  /**
   * @brief Run the application
   */
  void run();

private:
  sf::RenderWindow window_{
    sf::VideoMode({1280, 800}),
    "SFML Window",
    sf::Style::Titlebar | sf::Style::Close
  };

  sf::Font const font_{"resources/tuffy.ttf"};
  sf::Text log_text_{font_, "", 20};
  std::deque<std::string> logs_;

  // ======== Helper functions ======== //
  // Log to window
  void log();

  // Update logs
  void update_log(std::string&& text)
  {
    // Upper limit on messages
    if (this->logs_.size() == 24U) {
      this->logs_.pop_front();
    }

    this->logs_.emplace_back(text);
  }

  // Handle closing events
  void handle(sf::Event::Closed const&) { this->window_.close(); }

  // Key Press event
  void handle(sf::Event::KeyPressed const& event)
  {
    if (event.code == sf::Keyboard::Key::Escape) {
      this->window_.close();
    }
  }

  // Mouse Move event
  void handle(sf::Event::MouseMoved const& event)
  {
    this->update_log(
      std::format(
        "Mouse Moved: ({}, {})", event.position.x, event.position.y
      )
    );
  }

  // All remaining events
  void handle(auto const&)
  {
    // this->update_log("Unhandled Event\n");
  }
};

#endif
