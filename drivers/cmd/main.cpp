#include <iostream>
#include <qcircuit.hpp>

using namespace qc;

int main(int argc, char** argv) {
	// A Quantum Circuit with 4 QBits
	QCircuit circuit(8);
	
	// Initializes A Simple Bell State, now with Toffoli/CCNOT gate!
	circuit.h(2);
	circuit.x(7);
	circuit.ccnot(2,7,0);
	
	std::vector result = circuit.simulate(1024);
	for(int i = 0; i < result.size(); i++)std::cout<<"QBit "<<i<<": "<<result[i]<<"\n";
	std::cout << "Done" << std::endl;
	return 0;
}
