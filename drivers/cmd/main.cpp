#include <iostream>
#include <qcircuit.hpp>
#include <input/input.hpp>

using namespace qc;

int main(int argc, char** argv) {
	if(argc<2) {
        std::cerr<<"Usage: "<<argv[0]<<" [input file]"<<"\n";
		return 1;
	}
	auto result = runCircuit(argv[1]);
	if(result) {
		for(int i = 0; i < result->size(); i++)std::cout<<"QBit "<<i<<": "<<(*result)[i]<<"\n";
	}
	return 0;
}
