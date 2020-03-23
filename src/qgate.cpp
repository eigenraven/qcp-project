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
QGate CSWAP(3,{	1,0,0,0,0,0,0,0,
				0,1,0,0,0,0,0,0,
				0,0,1,0,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,0,1,0,0,0,
				0,0,0,0,0,0,1,0,
				0,0,0,0,0,1,0,0,
				0,0,0,0,0,0,0,1});

QGate CCNOT(3,{	1,0,0,0,0,0,0,0,
				0,1,0,0,0,0,0,0,
				0,0,1,0,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,0,1,0,0,0,
				0,0,0,0,0,1,0,0,
				0,0,0,0,0,0,0,1,
				0,0,0,0,0,0,1,0});

std::vector<QGate *> uGates;

QGate* U1(double lambda) {return U3(0,0,lambda);}
QGate* U2(double phi, double lambda) {return U3(M_PI*0.5,phi,lambda);}
QGate* U3(double theta, double phi, double lambda) {
	uGates.push_back(new QGate(1,{
cos(theta*0.5)*complex{cos(0.5*(phi+lambda)),-sin(0.5*(phi+lambda))},
-sin(theta*0.5)*complex{cos(0.5*(phi-lambda)),-sin(0.5*(phi-lambda))},
sin(theta*0.5)*complex{cos(0.5*(phi-lambda)),sin(0.5*(phi-lambda))},
cos(theta*0.5)*complex{cos(0.5*(phi+lambda)),sin(0.5*(phi+lambda))}}));
	return uGates[uGates.size()-1];
}

void deleteGates() {
  for(QGate* q : uGates)delete q;
}

QGate* CCU1(double lambda) {return CCU3(0,0,lambda);}
QGate* CCU2(double phi, double lambda) {return CCU3(M_PI*0.5,phi,lambda);}
QGate* CCU3(double theta, double phi, double lambda) {
	uGates.push_back(new QGate(3,{1,0,0,0,0,0,0,0,
					0,1,0,0,0,0,0,0,
					0,0,1,0,0,0,0,0,
					0,0,0,1,0,0,0,0,
					0,0,0,0,1,0,0,0,
					0,0,0,0,0,1,0,0,
					0,0,0,0,0,0,
cos(theta*0.5)*complex{cos(0.5*(phi+lambda)),-sin(0.5*(phi+lambda))},
-sin(theta*0.5)*complex{cos(0.5*(phi-lambda)),-sin(0.5*(phi-lambda))},
					0,0,0,0,0,0,
sin(theta*0.5)*complex{cos(0.5*(phi-lambda)),sin(0.5*(phi-lambda))},
cos(theta*0.5)*complex{cos(0.5*(phi+lambda)),sin(0.5*(phi+lambda))}}));
	return uGates[uGates.size()-1];
}


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
  } else if (gate == "cswap") {
    return &CSWAP;
  } else if (gate == "ccnot" || gate == "toffoli") {
    return &CCNOT;
  } else
    return std::nullopt;
}
} // namespace qc
