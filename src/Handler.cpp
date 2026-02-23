#include <Handler.hpp>

namespace kalika::Handler
{
  void handle(GameEvent::FireEvent event, World& w)
  {
    w.spawn_bullet(
      event.position,
      event.velocity,
      event.texture,
      event.size,
      event.lifetime,
      get_behaviour<Dasher>(),
      w.bus
    );
  }

  void handle(GameEvent::SpawnEvent, World&)
  {}

  void handle(GameEvent::ReleaseEvent event, World& w)
  {
    w.despawn_bullet(event.id);
  }
}  //namespace kalika::Handler
