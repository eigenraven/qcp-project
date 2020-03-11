#pragma once

#include <qcircuit.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>

namespace qc {

using namespace std;

optional<vector<double>> runCircuit(char* filename) {
	ifstream file(filename);
	if(file.is_open()) {
		string line;
		string token;
		int shots = 1024;
		QCircuit* circuit;
		while(!file.eof()) {
			getline(file,line);
			stringstream ss(line);
			getline(ss,token,',');
			if(token=="qubits") {
				getline(ss,token,',');
				circuit=new QCircuit(stoi(token));
			} else if(token=="shots") {
				getline(ss,token,',');
				shots=stoi(token);
			} else {
				transform(token.begin(),token.end(),token.begin(),[](unsigned char c){return tolower(c);});
				optional<QGate> gate = getGate(token);
				if(gate) {
					if(circuit) {
						vector<int> qubits;
						for(int i = 0; i < gate->qubits; i++) {
							getline(ss,token,',');
							qubits.push_back(stoi(token));
						}
						circuit->multipleGate(*gate,qubits);
					}
					else cerr<<"Error: Circuit must initialized before applying gates\n";
				}
			}
		}
		file.close();
		if(circuit) {
			return circuit->simulate(shots);
		}
		else cerr<<"Error: Circuit must have >0 qubits\n";
	}
	else cerr<<"Error: "<<strerror(errno)<<"\n";
	return {};
}

}
