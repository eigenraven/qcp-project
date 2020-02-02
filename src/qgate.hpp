#pragma once

#include "numeric/numeric.hpp"

namespace qc {

class qgate {
	dmatrix gate;
	inline qgate(int qbits, const complex (&cdata)[int])
	: gate(1<<qbits,1<<qbits,cdata) {
		//verify bounds
	}
};

qgate hadamard(1,{1,1,1,-1});
}
