#include <chrono>
#include <fstream>
#include <input/input.hpp>
#include <iostream>
#include <locale>
#include <string>
#include <qcircuit.hpp>

using namespace qc;

using hrclock = std::chrono::high_resolution_clock;

class ThousandsSeparation : public std::numpunct<char> {
protected:
  virtual char do_thousands_sep() const { return ' '; }
  virtual std::string do_grouping() const { return "\03"; }
};

int main(int argc, char **argv) {
  std::string_view inputFilePath;
  bool states = false;
  bool useSparseFlag = false;
  bool disableGroupingFlag = false;
  double noise1 = 0.0;
  for (int argi = 1; argi < argc; argi++) {
    std::string_view arg = argv[argi];
    if (arg.size() < 1) {
      continue;
    }
    if (arg.front() == '-') {
      if (arg == "--sparse") {
        useSparseFlag = true;
      } else if (arg == "--nogroup") {
        disableGroupingFlag = true;
      } else if (arg == "--states") {
        states = true;
 	  }	else if (arg.rfind("-n",0)==0) {
        noise1=std::atof(arg.substr(2).data());
      } else {
        std::cerr << "Unrecognized option: " << arg << std::endl;
        return 1;
      }
    } else {
      inputFilePath = arg;
    }
  }
  if (argc < 2 || inputFilePath.size() == 0) {
    std::cerr << "Usage: " << argv[0] << " <input file> [-n[NOISE]] [--sparse] [--nogroup] [--states]" << std::endl;
    return 1;
  }
  std::ifstream inputFile{std::string{inputFilePath}};
  if (!inputFile.is_open() || inputFile.fail()) {
    std::cerr << "Couldn't read input file: " << argv[1] << std::endl;
  }
  try {
    auto tStart = hrclock::now();
    auto [circuit, shots, noise2] = parseCircuit(inputFile, useSparseFlag);
    auto tParsed = hrclock::now();
    auto result = circuit->simulate(shots, disableGroupingFlag, std::max(noise1,noise2), states);
    auto tSimulated = hrclock::now();
	std::cout<<circuit->print(result,states);
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
