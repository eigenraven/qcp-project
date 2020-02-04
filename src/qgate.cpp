#include <qgate.hpp>

namespace qc {

QGate ID(1,{1,0,0,1});
QGate X(1,{0,1,1,0});
QGate Y(1,{0,-1i,1i,0});
QGate Z(1,{1,0,0,-1});
QGate H(1,{1/sqrt(2),1/sqrt(2),1/sqrt(2),-1/sqrt(2)});
QGate CNOT(2,{1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0});
QGate SWAP(2,{1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1});
}
