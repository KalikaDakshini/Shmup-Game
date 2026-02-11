#include "SFMLApp.hpp"

// Constructor
SFMLApp::SFMLApp()
{
  // Window configuration
  this->window_.setFramerateLimit(60);
  this->window_.setPosition({640, 400});
  this->window_.setKeyRepeatEnabled(false);

  // Logs configuration
  this->log_text_.setFillColor(sf::Color::Yellow);
}

// Run the SFMLApp
void SFMLApp::run()
{
  // Window loop
  while (this->window_.isOpen()) {
    // Clean slate
    this->window_.clear();

    // Handle events
    this->window_.handleEvents([this](auto const& event) {
      this->handle(event);
    });

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
    this->log_text_.setPosition({50.F, static_cast<float>(i * 20) + 50.F});
    this->log_text_.setString(this->logs_[i]);
    this->window_.draw(this->log_text_);
  }
}
