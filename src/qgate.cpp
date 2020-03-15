#include <qgate.hpp>

/*
 * These gates are also specified in two other files;
 * ensure you keep them all up-to-date:
   * src/qgate.json
   * README.md;
 */

namespace qc {
// clang-format off
QGate ID(1,{1,0,
			0,1});
QGate X(1,{	0,1,
			1,0});
QGate Y(1,{	0,-1_i,
			1_i,0});
QGate Z(1,{	1,0,
			0,-1});
QGate H(1,{	1/sqrt(2),1/sqrt(2),
			1/sqrt(2),-1/sqrt(2)});
QGate S(1,{	1,0,
			0,1_i});
QGate T(1,{	1,0,
			0,complex{1/sqrt(2),1/sqrt(2)}});
QGate Tinv(1,{	1,0,
				0,complex{1/sqrt(2),-1/sqrt(2)}});
QGate V(1,{	complex{0.5,0.5},complex{0.5,-0.5},
			complex{0.5,-0.5},complex{0.5,0.5}});
QGate Vinv(1,{	complex{0.5,-0.5},complex{0.5,0.5},
				complex{0.5,0.5},complex{0.5,-0.5}});


QGate CNOT(2,{	1,0,0,0,
				0,1,0,0,
				0,0,0,1,
				0,0,1,0});
QGate CY(2,{1,0,0,0,
			0,1,0,0,
			0,0,0,-1_i,
			0,0,1_i,0});
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
// clang-format on

std::optional<QGate*> getGate(std::string gateID) {
  switch(gateID){
	  case "id":
	  case "nop": 	return &ID;

	  case "x": 	return &X;
	  case "y": 	return &Y;
	  case "z":		return &Z;
	  case "h":		return &H;
	  case "s": 	return &S;
	  case "t": 	return &T;
	  case "tinv":  return &Tinv;
	  case "v":     return &Vinv;
	  case "vinv":  return &Vinv;

	  case "cnot":  return &CNOT;
	  case "cy": 	return &CY;
	  case "cz":	return &CZ;
	  case "swap":	return &SWAP;
	  case "ccnot":	return &CCNOT;
  }
  return std::nullopt
}
} // namespace qc
