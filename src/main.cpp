#include <iostream>

#include "SFMLApp.hpp"

int main()
{
  kalika::SFMLApp app;
  // Run application
  try {
    app.run();
  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
