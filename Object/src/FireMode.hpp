#ifndef FIRE_MODE_H
#define FIRE_MODE_H

#include <Object/helpers.hpp>

namespace kalika
{
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
     * @brief Generate bullet info from player's gun
     */
    void fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 2;
  };

  struct SpreadFire : FireMode {
    SpreadFire() { this->lifetime = 0.4F; }

    /**
     * @brief Generate bullet info from player's gun
     */
    void fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 5;
  };

  struct ChaserFire : FireMode {
    ChaserFire()
    {
      this->distance = 200.F;
      this->lifetime = 3.0F;
    }

    /**
     * @brief Generate bullet info from player's gun
     */
    void fire(WorldContext const& ctx, float dt);

  private:
    static constexpr size_t count = 1;
    bool toggle_ = true;
  };

}  //namespace kalika

#endif
