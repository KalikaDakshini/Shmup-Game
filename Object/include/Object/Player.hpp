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

#include <Object/ObjBase.hpp>
#include <Object/helpers.hpp>

namespace kalika
{
  constexpr float bul_size = 16.F;

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
    void update(WorldContext const& ctx, float dt);

  private:
    // Magnitue of velocity
    float vel_;

    // Set if reticle should be active
    bool active_ = false;

    // ====== Helper Functions ======= //
    // Update frame data
    void update_body(WorldContext const& ctx, float dt);
    void update_reticle(float dt);

    // Bind displacement to stay within the bounds
    sf::Vector2f bind(sf::FloatRect bounds, sf::Vector2f disp) const;
  };
}  //namespace kalika

#endif
