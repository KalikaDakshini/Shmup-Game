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
  struct FireMode;

  /**
   * @brief Player class
   */
  struct Player {
  private:
    sf::Texture body_tex_;
    sf::Texture reticle_tex_;
    std::unique_ptr<FireMode> mode_;

    /**
     * @brief Reticle information
     */
    struct Reticle {
      sf::Vector2f strength;
      float radius;
    };

    // ====== Helper Functions ======= //
    // Update frame data
    void update_frame(WorldContext const& ctx, float dt);
    // Bind displacement to stay within the bounds
    void bind(sf::FloatRect bounds, sf::Vector2f& disp);
    // Smoothen inputs by quantizing them
    static sf::Vector2f smoothen(sf::Vector2f vec, int factor = 5U);

  public:
    sf::Sprite body;
    sf::Sprite reticle;
    Reticle shoot;
    internal::Movable mov;

    // Constructor
    Player(sf::Vector2f position, float velocity, sf::Vector2f dir);

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
    void set_mode(std::unique_ptr<FireMode> mode);

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
    static Player
    create(sf::Vector2f position, float velocity, float radius);
  };

  struct FireMode {
    float cooldown = 5;
    // TODO(kalika): Implement Decay
    float decay = 100.0F;
    int32_t last_stamp = 0;
    bool in_cooldown = false;

    virtual ~FireMode() {};

    virtual std::vector<ObjInfo<Bullet>>
    fire(Player const& p, WorldContext const& ctx, float dt) = 0;
  };

  /**
   * @brief Rapid fire mode
   */
  struct RapidFire : FireMode {
    float velocity = 750.0F;

    std::vector<ObjInfo<Bullet>> fire(
      Player const& p, WorldContext const& ctx, float dt
    ) override final;
  };

  // struct HomingFire : FireMode {
  //   std::vector<ObjInfo<Bullet>> fire(float dt)  override final;
  // };

  // struct SpreadFire : FireMode {
  //   std::vector<ObjInfo<Bullet>> fire(float dt)  override final;
  // };

}  //namespace kalika

#endif
