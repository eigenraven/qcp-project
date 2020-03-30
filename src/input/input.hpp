/**
 * @file input.hpp
 * @brief Input format parsing into circuit descriptions
 */
#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <qcircuit.hpp>
#include <sstream>
#include <string>
#include <string_view>

namespace qc {

/// All the parameters parsed from the input file/string
struct ParsedCircuit {
  std::unique_ptr<QCircuit> circuit;
  int shots;
  double noise;
  bool noGroup;
  bool states;
  bool phase;
  int benchmark;
};

/// Parses a given input stream into a circuit object
inline ParsedCircuit parseCircuit(std::istream &input) {
  using std::getline;
  using std::string;
  if (input.bad()) {
    throw std::logic_error("Invalid circuit input stream");
  }
  string line;
  string token;

  bool useSparseMatrices = false;
  bool noGroup = false;
  bool states = false;
  bool phase = false;
  int benchmark = 0;
  int qubitCount = 0;
  int shots = 1024;
  double noise = 0.0;

  std::vector<std::pair<QGate *, std::vector<int>>> gates;
  gates.reserve(32);
  std::unique_ptr<QCircuit> circuit;

  while (!input.eof()) {
    getline(input, line);
    if (line.size() < 1) {
      continue;
    }
    line = line.substr(line.find_first_not_of(" \t", 0));
    if (line.find("//", 0) == 0) {
      continue;
    }
    std::stringstream ss(line);
    getline(ss, token, ',');
    if (token == "qubits") {
      getline(ss, token, ',');
      qubitCount = stoi(token);
    } else if (token == "sparse") {
      useSparseMatrices = true;
    } else if (token == "nogroup") {
      noGroup = true;
    } else if (token == "states") {
      states = true;
    } else if (token == "phase") {
      phase = true;
    } else if (token == "benchmark") {
      getline(ss, token, ',');
      benchmark = std::stoi(token);
    } else if (token == "shots") {
      getline(ss, token, ',');
      shots = std::stoi(token);
    } else if (token == "noise") {
      getline(ss, token, ',');
      noise = std::stof(token);
    } else {
      std::transform(token.begin(), token.end(), token.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      std::optional<QGate *> ogate = getGate(token);
      if (ogate) {
        QGate *gate = *ogate;
        std::vector<int> qubits;
        for (int i = 0; i < gate->qubits; i++) {
          getline(ss, token, ',');
          qubits.push_back(stoi(token));
        }
        gates.push_back(std::make_pair(gate, std::move(qubits)));
      } else {
        throw std::logic_error("Error: unknown gate encountered");
      }
    }
  }

  if (qubitCount < 1) {
    throw std::logic_error("Error: Circuit must have >0 qubits");
  }

  if (useSparseMatrices) {
    circuit = QCircuit::make<smatrix>(qubitCount);
  } else {
    circuit = QCircuit::make<dmatrix>(qubitCount);
  }

  for (auto &[gate, qubits] : gates) {
    circuit->multipleGate(gate, std::move(qubits));
  }

  return {std::move(circuit), shots, noise, noGroup, states, phase, benchmark};
}

} // namespace qc
