#include <iostream>
#include <qcircuit.hpp>

using namespace qc;

void oracle(std::shared_ptr<QCircuit> circuit, int search) {
	if(1-(search>>2&1))circuit->x(0);
	if(1-(search>>1&1))circuit->x(1);
	if(1-(search&1))circuit->x(2);

	circuit->h(2);
	circuit->ccnot(0,1,2);
	circuit->h(2);

	if(1-(search>>2&1))circuit->x(0);
	if(1-(search>>1&1))circuit->x(1);
	if(1-(search&1))circuit->x(2);
}

void grovers(std::shared_ptr<QCircuit> circuit) {
  circuit->applyAll(&H);
  circuit->applyAll(&X);
  circuit->h(2);
  circuit->ccnot(0,1,2);
  circuit->h(2);
  circuit->applyAll(&X);
  circuit->applyAll(&H);
}

int main(int argc, char **argv) {
  int search = 6;

  std::shared_ptr<QCircuit> circuit = QCircuit::make<smatrix>(3);
  circuit->applyAll(&H);
  for(int i = 0; i < 2; i++) {
	oracle(circuit,search);
	grovers(circuit);
  }
  auto result = circuit->simulate(1024,false,0,true);
  std::cout<<circuit->print(result,true);
  return 0; 
}
