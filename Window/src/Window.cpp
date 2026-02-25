#include <Window/Window.hpp>

namespace kalika
{
  // Constructor
  SFMLWindow::SFMLWindow(
    sf::Vector2u dimensions, char const* title, EventBus* bus
  ) :
    // Window infor
    window_(
      sf::VideoMode(dimensions),
      title,
      sf::Style::Titlebar | sf::Style::Close
    ),
    // World axis
    x_axis_({1.F, 0.F}),
    bus_(bus)
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
    this->settings_.antiAliasingLevel = 8;

    // Logs configuration
    this->log_text_.setFillColor(sf::Color::Yellow);

    if (sf::Joystick::isConnected(0)) {
      this->update_log("Joystick 0 connected");
    }
  }

  void SFMLWindow::handle_input()
  {
    // Handle Window Events
    this->window_.handleEvents([this](auto const& event) {
      this->handle(event);
    });
  }

  // Run the SFMLWindow
  void SFMLWindow::draw(std::vector<SpriteRef> const& sprites)
  {
    // Clear display before drawing
    this->window_.clear();

    // Draw the collection of sprites provided
    for (auto sprite : sprites) {
      this->window_.draw(sprite);
    }

    // Log messages to window
    this->log();

    // Render window
    this->window_.display();
  }

  // Log messages
  void SFMLWindow::log()
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
    // this->log_text_.setPosition(
    //   {static_cast<float>(x_disp), static_cast<float>(h - (y_disp * 2))}
    // );
    // this->log_text_.setString(
    //   std::format(
    //     "Bullet Count: {}", this->world_.bullet_capacity()
    //     // this->enemy_pool_.capacity()
    //   )
    // );
    this->window_.draw(this->log_text_);
  }

  // Update message logs
  void SFMLWindow::update_log(std::string const& text)
  {
    // Upper limit on messages
    if (this->logs_.size() == 12U) {
      this->logs_.pop_front();
    }

    this->logs_.emplace_back(text);
  }

  // Handle closing events
  void SFMLWindow::handle(sf::Event::Closed const&)
  {
    this->window_.close();
  }

  // Key Press event
  void SFMLWindow::handle(sf::Event::KeyPressed const& event)
  {
    if (event.code == sf::Keyboard::Key::Escape) {
      this->window_.close();
    }
  }

  // Button press event
  void SFMLWindow::handle(sf::Event::JoystickButtonPressed const& event)
  {
    this->update_log(std::format("Pressed Button: {}", event.button));
    // Set fire modes
    if (sf::Joystick::isButtonPressed(0, 1)) {
      this->bus_->emplace(GameEvent::SwitchEvent{2});
    }
    if (sf::Joystick::isButtonPressed(0, 2)) {
      this->bus_->emplace(GameEvent::SwitchEvent{1});
    }
    if (sf::Joystick::isButtonPressed(0, 3)) {
      this->bus_->emplace(GameEvent::SwitchEvent{0});
    }
  }

  template<typename T>
  sf::Vector2<T> SFMLWindow::deadzone(sf::Vector2<T> strength, float zone)
  {
    if (strength.lengthSquared() >= zone * zone) {
      return strength;
    }
    return {};
  }

  // Joystick moved event
  void SFMLWindow::handle(sf::Event::JoystickMoved const& event)
  {
    // Control movement direction
    float const stick_pos = event.position;
    if (event.axis == sf::Joystick::Axis::X) {
      this->l_strength_ =
        this->deadzone<float>({stick_pos, this->l_strength_.y});
    }
    if (event.axis == sf::Joystick::Axis::Y) {
      this->l_strength_ =
        this->deadzone<float>({this->l_strength_.x, stick_pos});
    }
    // Control aiming direction
    if (event.axis == sf::Joystick::Axis::U) {
      this->r_strength_ =
        this->deadzone<float>({stick_pos, this->r_strength_.y});
    }
    if (event.axis == sf::Joystick::Axis::V) {
      this->r_strength_ =
        this->deadzone<float>({this->r_strength_.x, stick_pos});
    }

    // Trigger a move event
    this->bus_->emplace(
      GameEvent::MoveEvent{
        .l_strength = this->l_strength_, .r_strength = this->r_strength_
      }
    );

    // Show stick positions
    auto print_vec = [](auto const& id, auto const& vec) {
      return std::format("{}: {}, {}\n", id, vec.x, vec.y);
    };

    if (l_strength_.lengthSquared() > 0) {
      this->update_log(print_vec("L", this->l_strength_));
    }

    if (r_strength_.lengthSquared() > 0) {
      this->update_log(print_vec("R", this->r_strength_));
    }
  }

  // All remaining events
  void SFMLWindow::handle(auto const&)
  {
    // this->update_log("Unhandled Event\n");
  }

}  //namespace kalika
