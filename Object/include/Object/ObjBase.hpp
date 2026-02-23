#ifndef OBJBASE_H
#define OBJBASE_H

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <filesystem>
#include <variant>

#include <format>

#include <Event/GameEvent.hpp>
#include <Object/Behaviour.hpp>
#include <Object/helpers.hpp>

namespace kalika::internal
{
  /**
   * @brief Base for objects
   */
  struct ObjBase {
    // Constructor
    ObjBase(
      sf::Vector2f position,
      sf::Vector2f velocity,
      sf::Vector2f dir,
      sf::Texture& sprite_tex,
      float obj_size,
      EventBus* bus
    );

    /**
     * @brief Return the sprite associated with the player
     */
    sf::Sprite const& sprite() const { return this->draw_.sprite; }

    /**
     * @brief Return a modifiable reference to sprite associated with
     * player
     */
    sf::Sprite& sprite() { return this->draw_.sprite; }

    /**
     * @brief Return the position of the ship
     */
    sf::Vector2f position() const { return this->mov_.pos; }

    /**
     * @brief Return the velocity of the ship
     */
    sf::Vector2f velocity() const { return this->mov_.vel; }

    /**
     * @brief Return the forward direction
     */
    sf::Vector2f forward() const { return this->mov_.up; }

    /**
     * @brief Return the right direction
     */
    sf::Vector2f right() const { return this->mov_.right; }

    /**
     * @brief Scale sprite
     */
    void scale(float sprite_size);

    /**
     * @brief Update the kinetic data
     */
    void move(
      WorldContext const& ctx, internal::Movable const& target, float dt
    );

    /**
     * @brief Check if object is still alive
     */
    bool is_alive() const { return this->alive_; }

  protected:
    // Event bus for pushing eventsscale
    EventBus* bus_;

    // Composition variables
    Movable mov_;
    Drawable draw_;

    // Lifetime variables
    bool alive_ = true;

    // Animation variables
    size_t fx, fy;
    bool animate_ = false;
    unsigned int frame_count_ = 2UL;
    unsigned int interval_ = 10UL;

    // Behaviour variable
    Behaviour behaviour_;

    // ====== Helper functions ====== //
    /**
     * @brief Update the co-ordinate system
     */
    void update_frame();

    /**
     * @brief Animates the object
     */
    void animate(WorldContext const& ctx);

    /**
     * @brief Returns true if the object is at the edge of the arena
     */
    bool at_edge(sf::FloatRect bounds)
    {
      return bounds.contains(this->sprite().getPosition());
    }

  private:
    template<typename T>
    void set_frame(sf::Vector2<T> start, sf::Vector2<T> end)
    {
      auto m_start = sf::Vector2<int>(start);
      auto m_end = sf::Vector2<int>(end);
      this->sprite().setTextureRect({m_start, m_end});
    }
  };
}  //namespace kalika::internal

#endif
