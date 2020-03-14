#include <qgate.hpp>

/*
 * These gates are identical to those in gates.json;
 * don't modify one without the other.
 */

namespace qc {

// disable formatting for the matrices
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
  }
  if (gate == "x") {
    return &X;
  } else if (gate == "y") {
    return &Y;
  } else if (gate == "z") {
    return &Z;
  } else if (gate == "h") {
    return &H;
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
