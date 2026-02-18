#ifndef OBJBASE_H
#define OBJBASE_H

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <filesystem>
#include <variant>

#include <format>
#include <iostream>

#include "Behaviour.hpp"
#include "helpers.hpp"

namespace kalika
{

  // Information needed to construct an object
  struct ObjInfo {
    // Behaviour data
    BehaviourVariant behaviour;
    // Sprite data
    float size = 72.F;
    sf::Texture& texture;
    sf::Vector2f position = {};
    sf::Vector2f velocity = {};
    // Lifetime data
    float lifetime = 1.0F;
    float health = 10.F;
    // Animation data
    bool animate = false;
    size_t frame_count = 1UL;
    size_t interval = 2UL;
  };

  namespace internal
  {
    /**
     * @brief CRTP Base for other game objects
     */
    template<typename Derived> struct ObjBase {
      sf::Sprite sprite;
      Movable mov;

      /**
       * @brief Update the kinetic data
       */
      void move(WorldContext const& wld_ctx, float dt);

      /**
       * @brief Rebuild an object using info
       */
      void rebuild(ObjInfo const& info);

      /**
       * @brief Check if object3 is still alive
       */
      bool is_alive() const { return this->alive_; }

      // Factory function for creating an Object
      static Derived create(ObjInfo const& info)
      {
        return Derived::create(info);
      }

    protected:
      // Lifetime variables
      bool alive_ = true;

      // Animation variables
      size_t fx, fy;
      bool animate_ = true;
      unsigned int frame_count_ = 2;
      unsigned interval_ = 6;

      // Behaviour variables
      BehaviourVariant behaviour_;

      // Constructor
      explicit ObjBase(ObjInfo const& info);

      // ====== Helper functions ====== //
      void update_frame();

      // Animate sprites
      void animate(WorldContext const& ctx);

      // Check if the derived object is alive
      void set_alive(WorldContext const& ctx)
      {
        auto& derived = static_cast<Derived&>(*this);
        derived.set_alive(ctx);
      }

      // Set the behaviour of the object
      void set_behaviour(BehaviourVariant const& behaviour)
      {
        this->behaviour_ = behaviour;
      };

      // Bind the object to the window. Returns true if reached bounds
      bool bound(sf::FloatRect bounds)
      {
        return bounds.contains(this->sprite.getPosition());
      }

    private:
      template<typename T>
      void set_frame(sf::Vector2<T> start, sf::Vector2<T> end)
      {
        auto m_start = sf::Vector2<int>(start);
        auto m_end = sf::Vector2<int>(end);
        this->sprite.setTextureRect({m_start, m_end});
      }
    };

    // Base consstructor
    template<typename Derived>
    ObjBase<Derived>::ObjBase(ObjInfo const& info) :
      sprite(info.texture),
      animate_(info.animate),
      frame_count_(info.frame_count),
      interval_(info.interval),
      behaviour_(std::move(info.behaviour))
    {
      // Render data
      auto [px, py] = sf::Vector2f(this->sprite.getTexture().getSize());
      this->fx = px / frame_count_;
      this->fy = py;
      this->set_frame<size_t>({0UL, 0UL}, {fx, fy});
      this->sprite.setOrigin(sf::Vector2f(fx / 2.F, fy / 2.F));

      // Scale the texture
      float target_y = info.size;
      auto scl = target_y / py;

      this->sprite.scale({scl, scl});

      // Position data
      this->sprite.setPosition(info.position);

      // Orientation data
      this->mov.setVelocity(info.velocity);
      this->update_frame();
    }

    // Move the derived object by a frame
    template<typename Derived>
    void ObjBase<Derived>::move(WorldContext const& ctx, float dt)
    {
      // Swap frames for animations
      if (animate_) {
        this->animate(ctx);
      }

      // Update co-ordinate frame
      this->update_frame();

      // Update kinetic data
      auto accel = std::visit(
        [&ctx](auto const& var) { return var.accel(ctx); },
        this->behaviour_
      );
      this->mov.setVelocity(this->mov.velocity() + (accel * dt));
      auto disp = this->mov.velocity() * dt;
      this->sprite.move(disp);

      // Rotate the Object
      this->sprite.setRotation(
        this->mov.up.angle() + sf::radians(M_PIf / 2)
      );

      this->set_alive(ctx);
    }

    // Rebuild the derived object using info
    template<typename Derived>
    void ObjBase<Derived>::rebuild(ObjInfo const& info)
    {
      // Toggle alive status
      this->alive_ = true;

      // Set parameters
      this->set_behaviour(info.behaviour);
      this->mov.setVelocity(info.velocity);
      this->sprite.setPosition(info.position);

      // Update frame of reference
      this->update_frame();
    }

    // Updates co-ordinate data
    template<typename Derived> void ObjBase<Derived>::update_frame()
    {
      if (!equal(this->mov.up.lengthSquared(), 1.0F)) {
        this->mov.up = this->mov.up.normalized();
      }
      this->mov.right = this->mov.up.perpendicular().normalized();
    }

    template<typename Derived>
    void ObjBase<Derived>::animate(WorldContext const& ctx)
    {
      // Get the frame number
      auto frame_num = (ctx.frame_count / this->interval_) % frame_count_;
      auto xs = (frame_num == 0) ? 0UL : fx * frame_num - 1;
      this->set_frame(sf::Vector2<int>(xs, 0UL), sf::Vector2<int>(fx, fy));
    }
  }  //namespace internal
}  //namespace kalika

#endif
