#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <variant>

#include "helpers.hpp"

namespace kalika
{

  namespace internal
  {
    /**
     * @brief Movement mode of an object
     */
    template<typename Derived> struct Behaviour {
      /**
       * @brief Return acceleration of the Object
       */
      [[nodiscard]] sf::Vector2f accel(WorldContext const& ctx) const
      {
        auto& mode = static_cast<Derived&>(*this);
        return mode.accel(ctx);
      }

    private:
      Behaviour() = default;
      friend Derived;
    };
  }  //namespace internal

  /**
   * @brief Straight path
   */
  struct Dasher : internal::Behaviour<Dasher> {
    [[nodiscard]] sf::Vector2f accel(WorldContext const& ctx) const
    {
      (void)ctx;
      return {};
    }

    [[nodiscard]] sf::Vector2f bound_velocity(
      WorldContext const& ctx, sf::Vector2f position, sf::Vector2f velocity
    ) const
    {
      auto [px, py] = position;
      auto [lx, ly] = ctx.world_size.position;
      auto [rx, ry] = ctx.world_size.size;
      auto [vx, vy] = velocity;

      // Flip the velocity when moving towards the boundary
      if ((equal(px, lx, 10.F) && vx < 0.F) ||
          (equal(px, rx, 10.F) && vx > 0.F)) {
        return {-vx, vy};
      }
      else if ((equal(py, ly, 10.F) && vy < 0.F) ||
               (equal(py, ry, 10.F) && vy > 0.F)) {
        return {vx, -vy};
      }
      return {vx, vy};
    }
  };

  /**
   * @brief Hunts down the nearest enemy
   */
  struct Chaser : internal::Behaviour<Chaser> {
    [[nodiscard]] sf::Vector2f accel(WorldContext const& ctx) const
    {
      (void)ctx;
      return {};
    }

    [[nodiscard]] sf::Vector2f bound_velocity(
      WorldContext const& ctx, sf::Vector2f position, sf::Vector2f velocity
    ) const
    {
      (void)ctx;
      (void)position;
      (void)velocity;
      return {};
    }
  };

  /**
   * @brief Chases towards the target
   */
  using BehaviourVariant = std::variant<Dasher, Chaser>;

  template<typename VariantMode> BehaviourVariant get_behaviour()
  {
    static BehaviourVariant variant{VariantMode()};
    return variant;
  }

}  // namespace kalika

#endif
