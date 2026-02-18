#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>

#include "ObjBase.hpp"
#include "helpers.hpp"

namespace kalika
{
  // Forward declarations
  struct Player;

  namespace internal
  {
    // Texture cache for objects
    sf::Texture& body_texture();

    sf::Texture& reticle_texture();

    sf::Texture& bullet_texture();

    // Base CRTP template for firing modes
    template<typename FiringMode>
    std::vector<ObjInfo> fire_impl(WorldContext const& ctx, float dt)
    {
      static FiringMode mode;
      return mode.fire(ctx, dt);
    }
  }  //namespace internal

  constexpr float bul_size = 16.F;
  constexpr float pl_size = 72.F;

  // Player Info
  struct PlayerInfo {
    sf::Vector2f position;
    sf::Vector2f vel_dir;
    float vel_scale;
    float radius;
    float responsiveness;
    sf::Vector2f dir;
  };

  /**
   * @brief Player class
   */
  struct Player {
    /**
     * @brief Reticle information
     */
    struct Reticle {
      sf::Vector2f strength;
      float resp;
      float radius;

      // State variables
      bool active_ = false;
      sf::Vector2f cur_offset = {};

      // Timer functionality
      float const duration = 3.0F;
      float elapsed = duration;

      bool update_timer(float dt)
      {
        this->elapsed -= dt;
        return (this->elapsed > 0.0F);
      }

      void reset_timer() { this->elapsed = this->duration; }
    };

    sf::Sprite body;
    sf::Sprite reticle;
    Reticle shoot;
    internal::Movable mov;
    // Constructor
    Player(PlayerInfo info);

    /**
     * @brief Return the forward direction
     */
    sf::Vector2f const position() const
    {
      return this->body.getPosition();
    }

    /**
     * @brief Return the right component of the velocity vector
     */
    sf::Vector2f right_vel() const
    {
      return this->mov.velocity().dot(this->right()) * this->right();
    }

    /**
     * @brief Return the forward direction
     */
    sf::Vector2f const forward() const { return this->mov.up; }

    /**
     * @brief Return the right direction
     */
    sf::Vector2f const right() const { return this->mov.right; }

    /**
     * @brief Move and orient the ship in the corresponding direction
     */
    void move(WorldContext const& ctx, float dt);

    /**
     * @brief Set the firing  mode
     */
    // Set Firing mode
    template<typename FiringMode> void set_mode()
    {
      this->fire_mode = internal::fire_impl<FiringMode>;
    }

    /**
     * @brief Fire bullets at a specified rate
     */
    std::vector<ObjInfo> fire(WorldContext const& ctx, float dt) const;

    /**
     * @brief Create a player object
     *
     * @param position Initial position
     * @param velocity Starting velocity
     * @param radius Base range of the reticle
     */
    static Player create(PlayerInfo info);

  private:
    using FireFn =
      std::vector<ObjInfo> (*)(WorldContext const& ctx, float dt);
    FireFn fire_mode;

    // ====== Helper Functions ======= //
    // Update frame data
    void update_body(WorldContext const& ctx, float dt);
    void update_reticle(float dt);

    // Bind displacement to stay within the bounds
    void bind(sf::FloatRect bounds, sf::Vector2f& disp);

    // Check if the reticle should be active
    bool is_active() const { return this->shoot.active_; }

    bool& is_active() { return this->shoot.active_; }
  };

  /**
   * @brief Class describing Firing mode
   */
  template<typename Mode> struct FireMode {
    // Max distance between bullets
    float distance = 100.F;
    // Speed magnitue of a bullet
    float velocity = 750.0F;
    // Lifetime of a bullet in seconds
    float lifetime = 1.0F;
    // Flags to check if a new bullet is to be spawned
    bool spawn = true;
    float elapsed = 0.F;

  protected:
    constexpr float fire_interval()
    {
      return this->distance / this->velocity;
    }

    /**
     * @brief Sets spawn to true based on fire rate
     */
    bool spawn_check(float dt)
    {
      auto& mode = static_cast<Mode&>(*this);
      mode.spawn = false;
      mode.elapsed += dt;

      if (mode.elapsed >= mode.fire_interval()) {
        mode.elapsed -= mode.fire_interval();
        mode.spawn = true;
      }

      return mode.spawn;
    }

  private:
    FireMode() = default;
    friend Mode;
  };

  /**
   * @brief Rapid fire mode
   */
  struct RapidFire : FireMode<RapidFire> {
    float velocity = 1000.0F;
    float distance = 50.F;

    /**
     * @brief Generate bullet info from player's gun
     */
    std::vector<ObjInfo> fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 2;
  };

  struct SpreadFire : FireMode<SpreadFire> {
    float lifetime = 0.4F;

    /**
     * @brief Generate bullet info from player's gun
     */
    std::vector<ObjInfo> fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 5;
  };

  struct ChaserFire : FireMode<ChaserFire> {
    float distance = 200.F;
    float lifetime = 3.0F;
    bool toggle = true;

    /**
     * @brief Generate bullet info from player's gun
     */
    std::vector<ObjInfo> fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 1;
  };

}  //namespace kalika

#endif
