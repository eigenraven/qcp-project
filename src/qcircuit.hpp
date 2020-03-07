#include <vector>
#include <qreg.hpp>
#include <iostream>

namespace qc {

class QCircuit {
	QRegister qreg;
	std::vector<std::pair<int,QGate>> gates;

	public:
	QCircuit(int qbits)
	: qreg(qbits) {}

	void nop(int qbit) {
		gates.push_back(std::make_pair(qbit,ID));
	}

	void h(int qbit) {
		gates.push_back(std::make_pair(qbit,H));
	}

	void x(int qbit) {
		gates.push_back(std::make_pair(qbit,X));
	}

	void y(int qbit) {
		gates.push_back(std::make_pair(qbit,Y));
	}
	
	void z(int qbit) {
		gates.push_back(std::make_pair(qbit,Z));
	}

	void cnot(int qbit1, int qbit2) {
		doubleGate(CNOT,qbit1,qbit2);
	}
	
	void swap(int qbit1, int qbit2) {
		doubleGate(SWAP,qbit1,qbit2);
	}

	void ccnot(int qbit1, int qbit2, int qbit3) {
		multipleGate(CCNOT,{qbit1,qbit2,qbit3});
	}

	// So far only gates that operate on single or multiple consecutive QBits have been implemented
	// So this emulates a quantum computer with the QBits connected linearly
	// i.e. Each Qbit is only directly connected to its nearest neighbours
	void doubleGate(QGate gate, int qbit1, int qbit2) {
		if(gate.qbits!=2)throw std::invalid_argument("This gate does not operate on two qbits");
		if(qbit1==qbit2)throw std::invalid_argument("This gate must operate on different qbits");
		int lowest = std::min(qbit1,qbit2);
		int highest = std::max(qbit1,qbit2);
		for(int i = highest-1; i > lowest; i--) {
			swapNext(i);
		}
		if(qbit2>qbit1) {
			gates.push_back(std::make_pair(lowest,gate));
		} else {
			swapNext(lowest);
			gates.push_back(std::make_pair(lowest,gate));
			swapNext(lowest);
		}
		for(int i = lowest+1; i < highest; i++) {
			swapNext(i);
		}
	}

	void multipleGate(QGate gate, std::vector<int> qbits) {
		if(gate.qbits!=qbits.size())throw std::invalid_argument("This gate does not operate on the correct number of qbits");
		int lowest = qbits[0];
    std::vector<int> overlaps{qreg.nqbits, -1};
		for(int i = 0; i < qbits.size(); i++) {
			overlaps[qbits[i]]=qbits[i];
			lowest = std::min(lowest,qbits[i]);
			for(int j = i+1; j < qbits.size(); j++) {
				if(qbits[i]==qbits[j])throw std::invalid_argument("This gate must operate on different qbits");
			}
		}
		for(int i = 0; i < qbits.size(); i++) {
			if(overlaps[qbits[i]]!=lowest+i) {
				swap(overlaps[qbits[i]],lowest+i);
				overlaps[lowest+i]=qbits[i];
			}
		}
		gates.push_back(std::make_pair(lowest,gate));
		for(int i = qbits.size()-1; i >= 0; i--) {
			if(overlaps[qbits[i]]!=lowest+i) {
				swap(overlaps[qbits[i]],lowest+i);
				overlaps[lowest+i]=qbits[i];
			}
		}
	}

	void swapNext(int qbit) {
		gates.push_back(std::make_pair(qbit,SWAP));
	}

	std::vector<double> simulate(int shots) {
		for(std::pair<int,QGate> gate : gates)qreg.applyOperator(gate.second,gate.first);
		return qreg.measureMultiple(shots);
	}
};
}
