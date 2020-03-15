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

struct ParsedCircuit {
  std::unique_ptr<QCircuit> circuit;
  int shots;
  double noise;
};

inline ParsedCircuit parseCircuit(std::istream &input,
                                  bool useSparseMatrices = false) {
  using std::getline;
  using std::string;
  if (input.bad()) {
    throw std::logic_error("Invalid circuit input stream");
  }
  string line;
  string token;
  int shots = 1024;
  double noise = 0.0;
  std::unique_ptr<QCircuit> circuit;
  while (!input.eof()) {
    getline(input, line);
    std::stringstream ss(line);
    getline(ss, token, ',');
    if (token == "qubits") {
      getline(ss, token, ',');
      if (useSparseMatrices) {
        circuit = QCircuit::make<smatrix>(stoi(token));
      } else {
        circuit = QCircuit::make<dmatrix>(stoi(token));
      }
    } else if (token == "sparsequbits") {
      getline(ss, token, ',');
      circuit = QCircuit::make<smatrix>(stoi(token));
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
        if (!circuit) {
          throw std::logic_error(
              "Error: Circuit must initialized before applying gates");
        }
        std::vector<int> qubits;
        for (int i = 0; i < gate->qubits; i++) {
          getline(ss, token, ',');
          qubits.push_back(stoi(token));
        }
        circuit->multipleGate(gate, qubits);
      }
    }
  }
  if (!circuit) {
    throw std::logic_error("Error: Circuit must have >0 qubits");
  }
  return {std::move(circuit), shots, noise};
}

} // namespace qc
