#include <chrono>
#include <fstream>
#include <input/input.hpp>
#include <iostream>
#include <locale>
#include <qcircuit.hpp>
#include <string>

using namespace qc;

using hrclock = std::chrono::high_resolution_clock;

class ThousandsSeparation : public std::numpunct<char> {
protected:
  virtual char do_thousands_sep() const { return ' '; }
  virtual std::string do_grouping() const { return "\03"; }
};

int main(int argc, char **argv) {
  std::string_view inputFilePath;
  std::stringstream config;
  for (int argi = 1; argi < argc; argi++) {
    std::string_view arg = argv[argi];
    if (arg.size() < 2) {
      continue;
    }
    if (arg.front() == '-') {
      config << arg.substr(arg[1] == '-' ? 2 : 1) << "\n";
    } else {
      inputFilePath = arg;
    }
  }
  if (argc < 2 || inputFilePath.size() == 0) {
    std::cerr << "Usage: " << argv[0]
              << " <input file> [--noise,0.5] [--sparse] [--nogroup] [--states]"
              << std::endl;
    return 1;
  }
  std::ifstream inputFile{std::string{inputFilePath}};
  if (!inputFile.is_open() || inputFile.fail()) {
    std::cerr << "Couldn't read input file: " << argv[1] << std::endl;
  }
  std::string configStr = config.str();
  config.clear();
  config << inputFile.rdbuf() << "\n" << configStr << "\n";
  config << std::flush;
  try {
    auto tStart = hrclock::now();
    auto parsed = parseCircuit(config);
    auto tParsed = hrclock::now();
    auto result = parsed.circuit->simulate(parsed.shots, parsed.noGroup,
                                           parsed.noise, parsed.states);
    auto tSimulated = hrclock::now();
    std::cout << parsed.circuit->print(result, parsed.states);
    int64_t nsParse =
        std::chrono::duration_cast<std::chrono::nanoseconds>(tParsed - tStart)
            .count();
    int64_t nsSimulation = std::chrono::duration_cast<std::chrono::nanoseconds>(
                               tSimulated - tParsed)
                               .count();
    std::cout.imbue(
        std::locale(std::locale::classic(), new ThousandsSeparation));
    std::cout << "Parse time: " << nsParse << "ns" << std::endl;
    std::cout << "Simulation time: " << nsSimulation << "ns" << std::endl;
    auto minDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                           hrclock::duration(1))
                           .count();
    std::cout << "Timer max resolution: " << minDuration << "ns" << std::endl;
  } catch (std::bad_alloc e) {
    std::cerr << "Cannot allocate this much memory" << std::endl;
    return 2;
  } catch (std::logic_error e) {
    std::cerr << "Caught logic error: " << e.what() << std::endl;
    return 2;
  } catch (std::exception e) {
    std::cerr << "Caught error: " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
