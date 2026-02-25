#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <filesystem>
#include <functional>
#include <memory>
#include <type_traits>
#include <variant>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>

#include <Object/ObjBase.hpp>
#include <Object/helpers.hpp>

namespace kalika
{
  namespace internal
  {
    // Texture cache
    sf::Texture& bullet_texture();
  }  //namespace internal

  // Player Info
  struct PlayerInfo {
    // Phase of the player
    sf::Vector2f position;
    sf::Vector2f velocity;
    // Facing direction of the player
    sf::Vector2f dir;

    // Texture object
    sf::Texture& player_tex;
    sf::Texture& reticle_tex;

    // Size of the object
    float size = 72.F;

    // Reticle information
    float radius;
    float responsiveness;
  };

  /**
   * @brief Class describing Firing mode
   */
  struct FireMode {
    // Max distance between bullets
    float distance = 100.F;
    float velocity = 750.0F;
    float fire_interval;
    // Lifetime of a bullet in seconds
    float lifetime = 1.0F;
    // Flags to check if a new bullet is to be spawned
    bool spawn = true;
    float elapsed = 0.F;

  protected:
    /**
     * @brief Sets spawn to true based on fire rate
     */
    bool spawn_check(float dt)
    {
      this->spawn = false;
      this->elapsed += dt;

      if (this->elapsed >= this->fire_interval) {
        this->elapsed -= this->fire_interval;
        this->spawn = true;
      }

      return this->spawn;
    }

    // Constructor
    FireMode() { this->fire_interval = distance / velocity; }
  };

  /**
   * @brief Rapid fire mode
   */
  struct RapidFire : FireMode {
    // Constructor
    RapidFire()
    {
      this->velocity = 1000.0F;
      this->distance = 50.F;
    }

    /**
     * @brief Trigger event for Rapid fire
     */
    void fire(GameContext const& ctx, float dt, EventBus* bus);

  private:
    static constexpr size_t count = 2;
  };

  /**
   * @brief Spread fire mode
   */
  struct SpreadFire : FireMode {
    SpreadFire() { this->lifetime = 0.4F; }

    /**
     * @brief Trigger fire events for Spreadfire
     */
    void fire(GameContext const& ctx, float dt, EventBus* bus);

  private:
    static constexpr size_t count = 5;
  };

  /**
   * @brief Chaser fire mode
   */
  struct ChaserFire : FireMode {
    ChaserFire()
    {
      this->velocity = 500.F;
      this->distance = 200.F;
      this->lifetime = 3.0F;
    }

    /**
     * @brief Trigger fire for Chaser fire
     */
    void fire(GameContext const& ctx, float dt, EventBus* bus);

  private:
    static constexpr size_t count = 1;
    bool toggle_ = true;
  };

  /**
   * @brief Player class
   */
  struct Player : public internal::ObjBase {
    /**
     * @brief Reticle information
     */
    struct Reticle {
      sf::Sprite sprite;
      sf::Vector2f strength;
      // Distance to ship
      float radius;
      // Movement responsiveness
      float resp;

      // State variables
      bool active_ = false;
      sf::Vector2f cur_offset = {};

      // Timer functionality to hide reticle
      float const duration = 3.0F;
      float elapsed = duration;

      // Constructer
      Reticle(sf::Texture& texture) : sprite(texture) {}

      bool update_timer(float dt)
      {
        this->elapsed -= dt;
        return (this->elapsed > 0.0F);
      }

      void reset_timer() { this->elapsed = this->duration; }
    };

    // Reticle object associated with player
    Reticle shoot;

    // Strength of L-stick push
    sf::Vector2f strength;

    // Constructor
    Player(PlayerInfo info, EventBus* bus);

    /**
     * @brief Return the sprite associated with the reticle
     */
    sf::Sprite const& reticle_sprite() const { return this->shoot.sprite; }

    /**
     * @brief Move and orient the ship in the corresponding direction
     */
    void update(GameContext const& ctx, float dt);

    void set_strength(sf::Vector2f l_strength, sf::Vector2f r_strength)
    {
      this->strength = l_strength;
      this->shoot.strength = r_strength;
    }

    /**
     * @brief Fire bullets depending on firemode
     */
    void fire(GameContext const& ctx, float dt)
    {
      std::visit(
        [&](auto& arg) { arg.fire(ctx, dt, this->bus_); },
        fire_modes_[this->mode_id_]
      );
    }

    /**
     * @brief Set the fire mode
     */
    void set_mode(size_t idx) { this->mode_id_ = idx; }

    // Remove the move method for player. Use update instead
    void move(
      GameContext const& ctx, internal::Movable const& target, float dt
    ) = delete;

  private:
    // Magnitue of velocity
    float vel_;

    // Set if reticle should be active
    bool active_ = false;

    // Choose a firing mode
    using FireType = std::variant<RapidFire, SpreadFire, ChaserFire>;
    static constexpr size_t NUM_MODES = std::variant_size_v<FireType>;
    inline static std::array<FireType, NUM_MODES> fire_modes_{
      RapidFire(), SpreadFire(), ChaserFire()
    };
    size_t mode_id_ = 0;

    // ====== Helper Functions ======= //
    // Update frame data
    void update_body(GameContext const& ctx, float dt);
    void update_reticle(float dt);

    // Bind displacement to stay within the bounds
    sf::Vector2f bind(sf::FloatRect bounds, sf::Vector2f disp) const;
  };

}  //namespace kalika

#endif
