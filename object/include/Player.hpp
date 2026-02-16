#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <filesystem>
#include <memory>
#include <tuple>
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
    // Fire functionality
    template<typename Mode>
    std::vector<ObjInfo<Bullet>>
    fire_impl(Player const& player, WorldContext const& ctx, float dt)
    {
      static Mode mode;
      return mode.fire(player, ctx, dt);
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
    // FireMode selector
    using FireFn = std::vector<ObjInfo<Bullet>> (*)(
      Player const& player, WorldContext const& ctx, float dt
    );
    FireFn fire_mode = nullptr;

    // Constructor
    Player(PlayerInfo info);

    /**
     * @brief Return the forward direction
     */
    sf::Vector2f const& forward() const { return this->mov.up; }

    /**
     * @brief Return the right direction
     */
    sf::Vector2f const& right() const { return this->mov.right; }

    /**
     * @brief Move and orient the ship in the corresponding direction
     */
    void move(WorldContext const& ctx, float dt);

    /**
     * @brief Set the firing  mode
     */
    // Set Firing mode
    template<typename Mode> void set_mode()
    {
      this->fire_mode = &internal::fire_impl<Mode>;
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
    float cooldown = 5;
    float lifetime = 1.0f;
    int32_t last_stamp = 0;
    bool in_cooldown = false;
    float velocity = 1000.0F;

  protected:
    static constexpr float norm1 = 0.8944F;
    static constexpr float norm2 = 0.7071F;
  };

  /**
   * @brief Rapid fire mode
   */
  struct RapidFire : FireMode {
    float velocity = 1500.0F;

    std::vector<ObjInfo<Bullet>>
    fire(Player const& p, WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 2;
  };

  struct SpreadFire : FireMode {
    float lifetime = 0.3F;

    std::vector<ObjInfo<Bullet>>
    fire(Player const& p, WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 5;
  };

  struct HomingFire : FireMode {
    float cooldown = 10;
    float lifetime = 3.0F;
    bool toggle = true;

    std::vector<ObjInfo<Bullet>>
    fire(Player const& p, WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 2;
  };

}  //namespace kalika

#endif
