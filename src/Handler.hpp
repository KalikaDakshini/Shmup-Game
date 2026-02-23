#ifndef HANDLER_H
#define HANDLER_H

#include <Event/GameEvent.hpp>
#include <World.hpp>

namespace kalika::Handler
{
  // Spawn Bullets
  void handle(GameEvent::FireEvent event, World& w);

  // Spawn Enemies
  void handle(GameEvent::SpawnEvent event, World& w);

  // Release Objects
  void handle(GameEvent::ReleaseEvent event, World& w);
}  //namespace kalika::Handler

#endif
