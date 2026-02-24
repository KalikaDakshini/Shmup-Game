#ifndef EVENT_H
#define EVENT_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <functional>
#include <queue>
#include <variant>

namespace kalika
{
  namespace internal
  {
    template<typename SubType, typename Variant>
    inline constexpr bool contains_v = false;

    template<typename SubType, typename... Types>
    inline constexpr bool contains_v<SubType, std::variant<Types...>> =
      (std::is_same_v<SubType, Types> || ...);
  }  //namespace internal

  /**
   * @brief Event class
   */
  struct GameEvent {
    /**
     * @brief Move the player
     */
    struct MoveEvent {
      sf::Vector2f l_strength;
      sf::Vector2f r_strength;
    };

    /**
     * @brief Event representing player firing bullets
     */
    struct FireEvent {
      // Phase
      sf::Vector2f position;
      sf::Vector2f velocity;
      // Sprite data
      std::reference_wrapper<sf::Texture> texture;
      float size;
      // Game data
      float lifetime;
    };

    /**
     * @brief Event representing enemies spawning
     */
    struct SpawnEvent {
      // Phase
      sf::Vector2f position;
      sf::Vector2f velocity;
      // Sprite data
      float size = 90.F;
      std::reference_wrapper<sf::Texture> texture;
      // Lifetime data
      float health = 10.F;
      // Animation data
      bool animate = true;
      size_t frame_count = 2UL;
      size_t interval = 10UL;
    };

    /**
     * @brief Release an Object from the pool
     */
    struct ReleaseEvent {
      size_t idx;
    };

    /**
     * @brief Templated constructor
     */
    template<typename EventType>
    explicit GameEvent(EventType const& event) : data_(event)
    {}

    /**
     * @brief Check if event is of given type
     */
    template<typename EventType> [[nodiscard]] bool is() const
    {
      static_assert(is_subtype<EventType>, "Invalid type passed to Event");
      return std::holds_alternative<EventType>(this->data_);
    }

    /**
     * @brief Return event value if it is of given type
     */
    template<typename EventType> [[nodiscard]] EventType* is() const
    {
      static_assert(is_subtype<EventType>, "Invalid type passed to Event");
      std::get_if<EventType>(this->data_);
    }

    /**
     * @brief Apply a visitor
     */
    template<typename Visitor> void visit(Visitor&& v)
    {
      std::visit(std::forward<Visitor>(v), this->data_);
    }

  private:
    std::variant<FireEvent, SpawnEvent, ReleaseEvent, MoveEvent> data_;

    template<typename SubType>
    static constexpr bool is_subtype =
      internal::contains_v<SubType, decltype(data_)>;
  };

  using EventBus = std::queue<GameEvent>;
}  //namespace kalika

#endif
