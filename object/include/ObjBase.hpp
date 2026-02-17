#ifndef OBJBASE_H
#define OBJBASE_H

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <filesystem>
#include <variant>

#include "Behaviour.hpp"
#include "helpers.hpp"

namespace kalika
{

  // Information needed to construct an object
  struct ObjInfo {
    BehaviourVariant behaviour;
    sf::Texture& texture;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
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
      static Derived create(ObjInfo const& info);

    protected:
      bool alive_ = true;
      BehaviourVariant behaviour_;

      // Constructor
      explicit ObjBase(ObjInfo const& info);

      // ====== Helper functions ====== //
      void update_frame();

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
    };

    // Base consstructor
    template<typename Derived>
    ObjBase<Derived>::ObjBase(ObjInfo const& info) :
      sprite(info.texture), behaviour_(std::move(info.behaviour))
    {
      // Position data
      this->sprite.setPosition(info.position);
      this->sprite.scale({2.5F, 2.5F});

      this->sprite.setOrigin(
        sf::Vector2<float>(this->sprite.getTexture().getSize() / 2U)
      );

      // Orientation data
      this->mov.setVelocity(info.velocity);
      this->update_frame();
    }

    // Move the derived object by a frame
    template<typename Derived>
    void ObjBase<Derived>::move(WorldContext const& wld_ctx, float dt)
    {
      // Update co-ordinate frame
      this->update_frame();

      // Update kinetic data
      auto accel = std::visit(
        [&wld_ctx](auto const& var) { return var.accel(wld_ctx); },
        this->behaviour_
      );
      this->mov.setVelocity(this->mov.velocity() + (accel * dt));
      auto disp = this->mov.velocity() * dt;
      this->sprite.move(disp);

      // Rotate the Object
      this->sprite.setRotation(
        this->mov.up.angle() + sf::radians(M_PIf / 2)
      );

      this->set_alive(wld_ctx);
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
  }  //namespace internal

  // Create a new object from info
  template<typename Derived>
  Derived internal::ObjBase<Derived>::create(ObjInfo const& info)
  {
    return {info};
  }
}  //namespace kalika

#endif
