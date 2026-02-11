#include <deque>
#include <format>
#include <iostream>
#include <optional>

#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>

/**
 * @brief Application class
 */
class Application
{
public:
  // Constructor
  Application()
  {
    this->window_.setFramerateLimit(60);
    this->window_.setPosition({});
    this->log_text_.setFillColor(sf::Color::White);
  }

  /**
   * @brief Run the application
   */
  void run()
  {
    // Window loop
    while (this->window_.isOpen()) {
      // Poll events
      while (std::optional const event = this->window_.pollEvent()) {
        // Close button pressed
        if (event->is<sf::Event::Closed>()) {
          this->window_.close();
          break;
        }
        // Keyboard keys pressed
        if (auto const* key_event =
              event->getIf<sf::Event::KeyPressed>()) {
          // Close the window on Esc
          if (key_event->code == sf::Keyboard::Key::Escape) {
            this->window_.close();
            break;
          }
          // Update log otherwise
          update_log(
            std::format(
              "Key Pressed: {}",
              sf::Keyboard::getDescription(key_event->scancode)
                .toAnsiString()
            )
          );
        }

        // Mouse moved
        else if (auto const* mouse_event =
                   event->getIf<sf::Event::MouseMoved>()) {
          update_log(
            std::format(
              "Mouse moved to ({} {})",
              mouse_event->position.x,
              mouse_event->position.y
            )
          );
        }

        // Mouse button pressed
        else if (event->is<sf::Event::MouseButtonPressed>()) {
          update_log("Mouse Pressed");
        }
      }

      this->window_.clear();

      // Draw the contents of the log to the window
      for (std::size_t i = 0; i < this->logs_.size(); ++i) {
        this->log_text_.setPosition(
          {50.F, static_cast<float>(i * 20) + 50.F}
        );
        this->log_text_.setString(this->logs_[i]);
        this->window_.draw(this->log_text_);
      }

      this->window_.display();
    }
  }

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

  // Update logs
  void update_log(std::string&& text)
  {
    // Upper limit on messages
    if (this->logs_.size() == 24U) {
      this->logs_.pop_front();
    }

    this->logs_.emplace_back(text);
  }
};

int main()
{
  Application application;
  // Run application
  try {
    application.run();
  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
