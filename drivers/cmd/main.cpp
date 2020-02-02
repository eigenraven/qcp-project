#include <iostream>
#include <qcsim.hpp>

using namespace qc;

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  simulate(1);
  std::cout << "Done" << std::endl;
  return 0;
}
