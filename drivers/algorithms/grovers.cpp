#include <iostream>
#include <qcircuit.hpp>

using namespace qc;

void oracle(std::shared_ptr<QCircuit> circuit) {
	circuit->x(0);
	circuit->x(1);

	circuit->h(2);
	circuit->ccnot(0,1,2);
	circuit->h(2);

	circuit->x(0);
	circuit->x(1);
}

void grovers(std::shared_ptr<QCircuit> circuit) {
  circuit->applyAll(H);
  circuit->applyAll(X);
  circuit->h(2);
  circuit->ccnot(0,1,2);
  circuit->h(2);
  circuit->applyAll(X);
  circuit->applyAll(H);
}

int main(int argc, char **argv) {
  std::shared_ptr<QCircuit> circuit = QCircuit::make<smatrix>(3);
  circuit->applyAll(H);
  for(int i = 0; i < 2; i++) {
	oracle(circuit);
	grovers(circuit);
  }
  auto result = circuit->simulate(1024);
  for (int i = 0; i < result.size(); i++)
    std::cout << "QBit " << i << ": " << result[i] << "\n";
  return 0; 
}
