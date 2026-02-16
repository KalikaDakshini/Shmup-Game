#include "SFMLApp.hpp"

#include <algorithm>
#include <iostream>
#include <random>

namespace kalika
{
  // Constructor
  SFMLApp::SFMLApp(sf::Vector2u dimensions, char const* title) :
    // Window infor
    window_(
      sf::VideoMode(dimensions),
      title,
      sf::Style::Titlebar | sf::Style::Close
    ),
    // World axis
    up({
      0.F, -1.F
  }),
    // Player Information
    player_(
      Player::create({
        .position = sf::Vector2<float>(dimensions / 2U),
        .vel_dir = {},
        .vel_scale = static_cast<float>(dimensions.x) / 5.F,
        .radius = static_cast<float>(dimensions.y) / 4.F,
        .dir = up,
      })
    ),
    wld_ctx(
      this->clock_,
      {{0.F, 0.F}, sf::Vector2f(this->window_.getSize())},
      this->player_,
      {}
    )
  {
    // Window configuration
    this->window_.setFramerateLimit(60);
    this->window_.setPosition(
      sf::Vector2<int>(
        (sf::VideoMode::getDesktopMode().size - dimensions) / 2U
      )
    );
    this->window_.setKeyRepeatEnabled(false);
    this->log_text_.setCharacterSize(dimensions.y / 50U);

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
    // Window loop
    float last_stamp = 0.0F;
    while (this->window_.isOpen()) {
      // Timer data
      this->dt = this->clock_.getElapsedTime().asSeconds() - last_stamp;
      last_stamp = this->clock_.getElapsedTime().asSeconds();
      this->frame_count++;

      // Clean slate
      this->window_.clear();

      // Handle events
      this->window_.handleEvents([this](auto const& event) {
        this->handle(event);
      });
      this->fire_check();

      // Move the objects
      // Custom event
      this->player_.move(wld_ctx, dt);
      for (auto& obj : this->bullet_pool_) {
        obj.update(this->wld_ctx, dt);
      }

      // Draw objects
      this->window_.draw(this->player_.body);
      this->window_.draw(this->player_.reticle);
      for (auto const& obj : this->bullet_pool_) {
        this->window_.draw(obj.sprite);
      }

      // Clear pool
      this->bullet_pool_.release();

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
    auto [w, h] = this->window_.getSize();
    auto const x_disp = w / 30U;
    auto const y_disp = h / 20U;
    for (auto i = 0UL; i < this->logs_.size(); ++i) {
      this->log_text_.setPosition(
        {static_cast<float>(x_disp), static_cast<float>((i + 1) * y_disp)}
      );
      this->log_text_.setString(this->logs_[i]);
      this->window_.draw(this->log_text_);
    }

    // Log Object count
    this->log_text_.setPosition(
      {static_cast<float>(x_disp), static_cast<float>(h - (y_disp * 2))}
    );
    this->log_text_.setString(
      std::format("Bullet Count: {}", this->bullet_pool_.size())
    );
    this->window_.draw(this->log_text_);
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

  void SFMLApp::fire_check()
  {
    if (this->player_.shoot.strength.lengthSquared() > 0) {
      std::ranges::for_each(
        this->player_.fire(this->wld_ctx, dt),
        [this](auto const& info) { this->bullet_pool_.get(info); }
      );
    }
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
    // Set fire modes
    if (sf::Joystick::isButtonPressed(0, 1)) {
      this->player_.set_mode<HomingFire>();
    }
    if (sf::Joystick::isButtonPressed(0, 2)) {
      this->player_.set_mode<SpreadFire>();
    }
    if (sf::Joystick::isButtonPressed(0, 3)) {
      this->player_.set_mode<RapidFire>();
    }
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
    // auto print_vec = [](auto const& id, auto const& vec) {
    //   return std::format("{}: {}, {}\n", id, vec.x, vec.y);
    // };

    // Control movement
    float const pos = event.position;
    if (event.axis == sf::Joystick::Axis::X) {
      this->deadzone(
        this->player_.mov.strength, {pos, this->player_.mov.strength.y}
      );
    }
    if (event.axis == sf::Joystick::Axis::Y) {
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
    if (event.axis == sf::Joystick::Axis::V) {
      this->deadzone(
        this->player_.shoot.strength, {this->player_.shoot.strength.x, pos}
      );
    }

    // this->update_log(print_vec("L", this->player_.shoot.strength));
    // this->update_log(print_vec("R", this->player_.mov.strength));
  }

  // All remaining events
  void SFMLApp::handle(auto const&)
  {
    // this->update_log("Unhandled Event\n");
  }

}  //namespace kalika
