#include <chrono>
#include <fstream>
#include <input/input.hpp>
#include <iostream>
#include <qcircuit.hpp>

using namespace qc;

using hrclock = std::chrono::high_resolution_clock;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [input file]" << std::endl;
    return 1;
  }
  std::ifstream inputFile{argv[1]};
  if (!inputFile.is_open() || inputFile.fail()) {
    std::cerr << "Couldn't read input file: " << argv[1] << std::endl;
  }
  try {
    auto tStart = hrclock::now();
    auto [circuit, shots] = parseCircuit(inputFile);
    auto tParsed = hrclock::now();
    auto result = circuit->simulate(shots);
    auto tSimulated = hrclock::now();
    for (int i = 0; i < result.size(); i++)
      std::cout << "QBit " << i << ": " << result[i] << "\n";
    int64_t nsParse =
        std::chrono::duration_cast<std::chrono::nanoseconds>(tParsed - tStart)
            .count();
    int64_t nsSimulation = std::chrono::duration_cast<std::chrono::nanoseconds>(
                               tSimulated - tParsed)
                               .count();
    std::cout << "Parse time: " << nsParse << "ns" << std::endl;
    std::cout << "Simulation time (all runs): " << nsSimulation << "ns"
              << std::endl;
    std::cout << "Simulation time (average per run): "
              << nsSimulation / static_cast<int64_t>(shots) << "ns"
              << std::endl;
    auto minDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                           hrclock::duration(1))
                           .count();
    std::cout << "Timer max resolution: " << minDuration << "ns" << std::endl;
  } catch (std::exception e) {
    std::cerr << "Caught error: " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
