#include <algorithm>
#include <random>

#include <SFMLApp.hpp>

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
    up({0.F, -1.F}),
    // Player Information
    world_(
      {
        // Phase
        .position = sf::Vector2<float>(dimensions / 2U),
        .velocity = sf::Vector2f(sf::Vector2u(dimensions.x / 5, 0U)),
        .dir = up,
        // Textures
        .player_tex = internal::body_texture(),
        .reticle_tex = internal::reticle_texture(),
        // Size
        .size = 72.F,
        // Reticle information
        .radius = static_cast<float>(dimensions.y) / 4.F,
        .responsiveness = 4.F,
      },
      &(this->bus_)
    ),
    wld_ctx(
      // Clock
      this->clock_,
      // World Boundary
      {sf::Vector2f(this->window_.getSize()) * 0.05F,
       sf::Vector2f(this->window_.getSize()) * 0.95F},
      // Player
      this->world_.player,
      // Frame count
      this->frame_count
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

      // Handle Window Events
      this->window_.handleEvents([this](auto const& event) {
        this->handle(event);
      });

      // Update objects
      this->world_.update(this->wld_ctx, dt);

      // Handle game events
      this->handle_events();

      // Draw objects
      this->world_.draw(this->window_);

      // Log messages to window
      this->log();

      // Update context
      this->update_ctx();

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
      std::format(
        "Bullet Count: {}", this->world_.bullet_capacity()
        // this->enemy_pool_.capacity()
      )
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

  void SFMLApp::update_ctx()
  {
    // TODO(kalika): Update with enemy position changes
  }

  void SFMLApp::handle_events()
  {
    while (!this->bus_.empty()) {
      this->bus_.front().visit([this](auto& arg) {
        Handler::handle(arg, this->world_);
      });
      this->bus_.pop();
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
      this->player().set_mode<ChaserFire>();
    }
    if (sf::Joystick::isButtonPressed(0, 2)) {
      this->player().set_mode<SpreadFire>();
    }
    if (sf::Joystick::isButtonPressed(0, 3)) {
      this->player().set_mode<RapidFire>();
    }
  }

  template<typename T>
  sf::Vector2<T> SFMLApp::deadzone(sf::Vector2<T> strength, float zone)
  {
    if (strength.lengthSquared() >= zone * zone) {
      return strength;
    }
    return {};
  }

  void SFMLApp::handle(sf::Event::JoystickMoved const& event)
  {
    // Control movement direction
    float const stick_pos = event.position;
    if (event.axis == sf::Joystick::Axis::X) {
      this->l_strength =
        this->deadzone<float>({stick_pos, this->l_strength.y});
    }
    if (event.axis == sf::Joystick::Axis::Y) {
      this->l_strength =
        this->deadzone<float>({this->l_strength.x, stick_pos});
    }
    // Control aiming direction
    if (event.axis == sf::Joystick::Axis::U) {
      this->r_strength =
        this->deadzone<float>({stick_pos, this->r_strength.y});
    }
    if (event.axis == sf::Joystick::Axis::V) {
      this->r_strength =
        this->deadzone<float>({this->r_strength.x, stick_pos});
    }

    // Set player and reticle strengths
    this->player().strength = this->l_strength;
    this->player().shoot.strength = this->r_strength;

    // Show stick positions
    auto print_vec = [](auto const& id, auto const& vec) {
      return std::format("{}: {}, {}\n", id, vec.x, vec.y);
    };

    if (l_strength.lengthSquared() > 0) {
      this->update_log(print_vec("L", this->l_strength));
    }

    if (r_strength.lengthSquared() > 0) {
      this->update_log(print_vec("R", this->r_strength));
    }
  }

  // All remaining events
  void SFMLApp::handle(auto const&)
  {
    // this->update_log("Unhandled Event\n");
  }

  namespace internal
  {
    sf::Texture& body_texture()
    {
      static sf::Texture t;
      load_texture(t, "resources/player.png");
      return t;
    }

    sf::Texture& reticle_texture()
    {
      static sf::Texture t;
      load_texture(t, "resources/reticle.png");
      return t;
    }
  }  // namespace internal

}  //namespace kalika
