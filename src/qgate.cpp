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
QGate Y(1,{	0,  -1_i,
			1_i, 0});
QGate Z(1,{	1, 0,
			0,-1});
QGate H(1,{	1/sqrt(2), 1/sqrt(2),
			1/sqrt(2),-1/sqrt(2)});
QGate S(1,{	1,0,
			0,1_i});
QGate T(1,{	1,0,
			0,complex{1/sqrt(2),1/sqrt(2)}});
QGate Tinv(1,{	1,0,
				0,complex{1/sqrt(2),-1/sqrt(2)}});
QGate V(1,{	complex{0.5, 0.5},complex{0.5,-0.5},
			complex{0.5,-0.5},complex{0.5, 0.5}});
QGate Vinv(1,{	complex{0.5,-0.5},complex{0.5, 0.5},
				complex{0.5, 0.5},complex{0.5,-0.5}});


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

std::optional<QGate*> getGate(std::string gate) {
  if (gate == "id" || gate == "nop") {
    return &ID;
  } else if (gate == "x") {
    return &X;
  } else if (gate == "y") {
    return &Y;
  } else if (gate == "z") {
    return &Z;
  } else if (gate == "h" || gate=="hadamard") {
    return &H;
  } else if (gate == "s") {
    return &S;
  } else if (gate == "t") {
    return &T;
  } else if (gate == "tinv") {
    return &Tinv;
  } else if (gate == "v") {
    return &V;
  } else if (gate == "vinv") {
    return &Vinv;
  } else if (gate == "cnot") {
    return &CNOT;
  } else if (gate == "cy") {
    return &CY;
  } else if (gate == "cz") {
    return &CZ;
  } else if (gate == "swap") {
    return &SWAP;
  } else if (gate == "ccnot" || gate == "toffoli") {
    return &CCNOT;
  } else
    return std::nullopt;
}
} // namespace qc
