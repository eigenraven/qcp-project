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
              << " <input file> [--noise,0.5] [--sparse] [--nogroup] "
                 "[--states] [--bloch] [--benchmark,64]"
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
    if (parsed.benchmark <= 0) {
      auto result = parsed.circuit->simulate(parsed.shots, parsed.noGroup,
                                             parsed.noise, parsed.states, parsed.bloch);
      auto tSimulated = hrclock::now();
      std::cout << parsed.circuit->print(result, parsed.states, parsed.bloch);
      int64_t nsParse =
          std::chrono::duration_cast<std::chrono::nanoseconds>(tParsed - tStart)
              .count();
      int64_t nsSimulation =
          std::chrono::duration_cast<std::chrono::nanoseconds>(tSimulated -
                                                               tParsed)
              .count();
      std::cout.imbue(
          std::locale(std::locale::classic(), new ThousandsSeparation));
      std::cout << "Parse time: " << nsParse << "ns" << std::endl;
      std::cout << "Simulation time: " << nsSimulation << "ns" << std::endl;
      auto minDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             hrclock::duration(1))
                             .count();
      std::cout << "Timer max resolution: " << minDuration << "ns" << std::endl;
    } else { // benchmark mode
      std::vector<int64_t> runTimes;
      runTimes.reserve(static_cast<size_t>(parsed.benchmark));
      for (int iter = 0; iter < parsed.benchmark; iter++) {
        auto tIterStart = hrclock::now();
        parsed.circuit->reset();
        parsed.circuit->simulate(parsed.shots, parsed.noGroup, parsed.noise,
                                 parsed.states, parsed.bloch);
        auto tIterEnd = hrclock::now();
        int64_t nsIter = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             tIterEnd - tIterStart)
                             .count();
        runTimes.push_back(nsIter);
      }
      int64_t avgTime = std::accumulate(runTimes.begin(), runTimes.end(), 0LL) /
                        parsed.benchmark;
      int64_t varTime = 0;
      for (auto time : runTimes) {
        varTime += (time - avgTime) * (time - avgTime);
      }
      varTime /= parsed.benchmark * parsed.benchmark;
      int64_t sdTime =
          static_cast<int64_t>(std::sqrt(static_cast<double>(varTime)));
      std::cout << "avg," << avgTime << ",sd," << sdTime << std::endl;
    }
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
