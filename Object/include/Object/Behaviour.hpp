#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string_view>
#include <variant>

#include <Object/helpers.hpp>

namespace kalika
{

  namespace internal
  {
    /**
     * @brief Movement mode of an object
     */
    struct BehaviourBase {
      /**
       * @brief Check if the object is hitting the bounds.
       */
      [[nodiscard]] bool at_xbound(
        GameContext const& ctx,
        sf::Vector2f position,
        sf::Vector2f velocity
      ) const
      {
        auto [px, py] = position;
        auto [lx, ly] = ctx.world_size.position;
        auto [rx, ry] = ctx.world_size.size;
        auto [vx, vy] = velocity;

        // Flip the velocity when moving towards the boundary
        if ((equal(px, lx, 10.F) && vx < 0.F) ||
            (equal(px, rx, 10.F) && vx > 0.F)) {
          return true;
        }
        return false;
      }

      [[nodiscard]] bool at_ybound(
        GameContext const& ctx,
        sf::Vector2f position,
        sf::Vector2f velocity
      ) const
      {
        auto [px, py] = position;
        auto [lx, ly] = ctx.world_size.position;
        auto [rx, ry] = ctx.world_size.size;
        auto [vx, vy] = velocity;

        // Flip the velocity when moving towards the boundary
        if ((equal(py, ly, 10.F) && vy < 0.F) ||
            (equal(py, ry, 10.F) && vy > 0.F)) {
          return true;
        }
        return false;
      }

      BehaviourBase() = default;
    };
  }  //namespace internal

  /**
   * @brief Straight path
   */
  struct Dasher : internal::BehaviourBase {
    inline static constexpr auto abs_vel = 500.F;
    inline static constexpr auto frame_count = 2UL;
    inline static constexpr auto interval = 30UL;

    /**
     * @brief Return the acceleration of the chaser object
     */
    [[nodiscard]] sf::Vector2f
    accel(internal::Movable const&, internal::Movable const&) const
    {
      return {};
    }

    /**
     * @brief Return the velocity at the bounds
     */
    [[nodiscard]] sf::Vector2f bound_velocity(
      GameContext const& ctx, sf::Vector2f position, sf::Vector2f velocity
    ) const
    {
      auto [vx, vy] = velocity;

      // Flip the velocity when moving towards the boundary
      if (at_xbound(ctx, position, velocity)) {
        return {-vx, vy};
      }
      else if (at_ybound(ctx, position, velocity)) {
        return {vx, -vy};
      }

      return velocity;
    }
  };

  /**
   * @brief Hunts down the nearest enemy
   */
  struct Chaser : public internal::BehaviourBase {
    inline static constexpr auto abs_vel = 100.F;
    inline static constexpr auto frame_count = 4UL;
    inline static constexpr auto interval = 10UL;
    inline static constexpr auto homing_factor = 500.F;

    /**
     * @brief Return the acceleration of the object
     */
    [[nodiscard]] sf::Vector2f accel(
      internal::Movable const& self, internal::Movable const& target
    ) const
    {
      auto const dist_vec = target.pos - self.pos;
      auto const dist_length = dist_vec.length();
      auto const dot_prod = dist_vec.dot(self.up);
      auto const angle_update = homing_factor *
                                sqrtf(dist_length - dot_prod) /
                                powf(dist_length, 1.5F);

      // Update the bullet's velocity & position
      auto const dir = (dist_vec.dot(self.right) > 0) ? 1.F : -1.F;
      return dir * self.vel.perpendicular() * angle_update;
    }

    /**
     * @brief Bind the velocity at the boundaries
     */
    [[nodiscard]] sf::Vector2f bound_velocity(
      GameContext const& ctx, sf::Vector2f position, sf::Vector2f velocity
    ) const
    {
      auto [vx, vy] = velocity;
      // Negate the veloct
      if (this->at_xbound(ctx, position, velocity)) {
        return {0.F, vy};
      }
      else if (this->at_ybound(ctx, position, velocity)) {
        return {vx, 0.F};
      }
      return velocity;
    }
  };

  /**
   * @brief Chases towards the target
   */
  using Behaviour = std::variant<Dasher*, Chaser*>;

  template<typename BType> Behaviour get_behaviour()
  {
    static BType behaviour;
    return &behaviour;
  }

}  // namespace kalika

#endif
