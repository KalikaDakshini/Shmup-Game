#ifndef EVENT_H
#define EVENT_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <deque>

#include <variant>

namespace kalika
{
  namespace Object
  {
    struct Behaviour;
  }

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
     * @brief Event representing player firing bullets
     */
    struct FireEvent {
      // Behaviour data
      Object::Behaviour* behaviour;
      // Sprite data
      float size = 10.F;
      sf::Texture& texture;
      sf::Vector2f position;
      sf::Vector2f velocity;
      // Lifetime data
      float lifetime = 1.0F;
    };

    /**
     * @brief Event representing enemies spawning
     */
    struct SpawnEvent {
      // Behaviour data
      Object::Behaviour* behaviour;
      // Sprite data
      float size = 90.F;
      sf::Texture& texture;
      sf::Vector2f position;
      sf::Vector2f velocity;
      // Lifetime data
      float health = 10.F;
      // Animation data
      bool animate = false;
      size_t frame_count = 1UL;
      size_t interval = 2UL;
    };

    /**
     * @brief Release an Object from the pool
     */
    template<typename Object> struct ReleaseEvent {
      Object& obj;
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
      static_assert(
        is_subtype<EventType> && "Invalid type passed to Event"
      );
      return std::holds_alternative<EventType>(this->data_);
    }

    /**
     * @brief Return event value if it is of given type
     */
    template<typename EventType> [[nodiscard]] EventType* is() const
    {
      static_assert(
        is_subtype<EventType> && "Invalid type passed to Event"
      );
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
    std::variant<FireEvent, SpawnEvent> data_;

    template<typename SubType>
    static constexpr bool is_subtype =
      internal::contains_v<SubType, decltype(data_)>;
  };

  using EventBus = std::deque<GameEvent>;
}  //namespace kalika

#endif
