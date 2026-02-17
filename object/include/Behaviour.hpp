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
  struct Straight : internal::Behaviour<Straight> {
    sf::Vector2f accel(WorldContext const& ctx) const
    {
      (void)ctx;
      return {};
    }
  };

  /**
   * @brief Hunts down the nearest enemy
   */
  struct Chaser : internal::Behaviour<Chaser> {
    sf::Vector2f accel(WorldContext const& ctx) const
    {
      (void)ctx;
      return {};
    }
  };

  /**
   * @brief Chases towards the target
   */
  using BehaviourVariant = std::variant<Straight, Chaser>;

  template<typename VariantMode> BehaviourVariant get_behaviour()
  {
    static BehaviourVariant variant{VariantMode()};
    return variant;
  }

}  // namespace kalika

#endif
