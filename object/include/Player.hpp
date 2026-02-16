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

#include "Bullet.hpp"
#include "helpers.hpp"

namespace kalika
{
  // Forward declarations
  struct FireMode;
  struct Player;

  namespace internal
  {
    template<typename FiringMode>
    std::vector<ObjInfo<Bullet>>
    fire_impl(WorldContext const& ctx, float dt)
    {
      static FiringMode mode;
      return mode.fire(ctx, dt);
    }
  }  //namespace internal

  // Player Info
  struct PlayerInfo {
    sf::Vector2f position;
    sf::Vector2f vel_dir;
    float vel_scale;
    float radius;
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
      float radius;
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
    std::vector<ObjInfo<Bullet>>
    fire(WorldContext const& ctx, float dt) const;

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
      std::vector<ObjInfo<Bullet>> (*)(WorldContext const& ctx, float dt);
    FireFn fire_mode;

    // ====== Helper Functions ======= //
    // Update frame data
    void update_frame(WorldContext const& ctx, float dt);

    // Bind displacement to stay within the bounds
    void bind(sf::FloatRect bounds, sf::Vector2f& disp);

    // Smoothen inputs by quantizing them
    static sf::Vector2f smoothen(sf::Vector2f vec, int factor = 5U);

    // Texture Cache
    static sf::Texture& body_texture()
    {
      static sf::Texture t;
      (void)t.loadFromFile("resources/player.png");
      return t;
    }

    static sf::Texture& reticle_texture()
    {
      static sf::Texture t;
      (void)t.loadFromFile("resources/reticle.png");
      return t;
    }
  };

  /**
   * @brief Class describing Firing mode
   */
  struct FireMode {
    // Max distance between bullets
    size_t cooldown = 10;
    // Speed magnitue of a bullet
    float velocity = 750.0F;
    // Lifetime of a bullet
    float lifetime = 1.0F;
    // Flag to check if a new bullet is to be spawned
    bool spawn = true;
    size_t elapsed = 0;

  protected:
    static constexpr float norm1 = 0.8944F;
    static constexpr float norm2 = 0.7071F;
  };

  /**
   * @brief Rapid fire mode
   */
  struct RapidFire : FireMode {
    float velocity = 1000.0F;
    size_t cooldown = 5;

    /**
     * @brief Generate bullet info from player's gun
     */
    std::vector<ObjInfo<Bullet>> fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 2;
  };

  struct SpreadFire : FireMode {
    size_t cooldown = 7;
    float lifetime = 0.4F;

    /**
     * @brief Generate bullet info from player's gun
     */
    std::vector<ObjInfo<Bullet>> fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 5;
  };

  struct HomingFire : FireMode {
    float cooldown = 10;
    float lifetime = 3.0F;
    bool toggle = true;

    /**
     * @brief Generate bullet info from player's gun
     */
    std::vector<ObjInfo<Bullet>> fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 2;
  };

}  //namespace kalika

#endif
