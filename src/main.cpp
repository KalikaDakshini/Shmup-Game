#include <iostream>

#include <SFMLGame.hpp>

int main()
{
  kalika::SFMLGame game({1600, 1000}, "smol-shmup");
  // Run application
  try {
    game.run();
  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
