#include <iostream>
#include <qcircuit.hpp>

using namespace qc;

int main(int argc, char** argv) {
	// A Quantum Circuit with 4 QBits
	QCircuit circuit(8);
	
	// Initializes A Simple Bell State
	circuit.h(7);
	circuit.cnot(7,0);
	
	std::vector result = circuit.simulate(1024);
	for(int i = 0; i < result.size(); i++)std::cout<<"QBit "<<i<<": "<<result[i]<<"\n";
	std::cout << "Done" << std::endl;
	return 0;
}
