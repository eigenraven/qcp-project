#include <qgate.hpp>

/*
 * These gates are identical to those in gates.json;
 * don't modify one without the other.
 */

namespace qc {

QGate ID(1,{1,0,
			0,1});
QGate X(1,{	0,1,
			1,0});
QGate Y(1,{	0,-1i,
			1i,0});
QGate Z(1,{	1,0,
			0,-1});
QGate H(1,{	1/sqrt(2),1/sqrt(2),
			1/sqrt(2),-1/sqrt(2)});
QGate CNOT(2,{	1,0,0,0,
				0,1,0,0,
				0,0,0,1,
				0,0,1,0});
QGate CY(2,{1,0,0,0,
			0,1,0,0,
			0,0,0,-1i,
			0,0,1i,0});
QGate CZ(2,{1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,-1});
QGate SWAP(2,{	1,0,0,0,
				0,0,1,0,
				0,1,0,0,
				0,0,0,1});
QGate CCNOT(3,{	1,0,0,0,0,0,0,0,
				0,1,0,0,0,0,0,0,
				0,0,1,0,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,0,1,0,0,0,
				0,0,0,0,0,1,0,0,
				0,0,0,0,0,0,0,1,
				0,0,0,0,0,0,1,0});
}
