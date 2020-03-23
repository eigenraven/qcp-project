#include <iostream>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <qcircuit.hpp>
#include <cstdint>

using namespace qc;

// Euclid's Algorithm. At least 2300 years old!
int64_t gcd(int64_t a, int64_t b) {
  if(a==0)return b;
  return gcd(b%a,a);
}

// Extended Euclid's Algorithm
std::tuple<int64_t,int64_t,int64_t> egcd(int64_t a, int64_t b) {
  if(a==0)return {b,0,1};
  auto [g,y,x] = egcd(b%a,a);
  return {g,x-(b/a)*y,y};
}

int64_t modinv(int64_t a, int64_t m) {
  auto [g,x,n]= egcd(a,m);
  if(x<0)x+=m;
  return x%m;
}

class Shors {
  int64_t N;
  int64_t n;
  int64_t a;
  int64_t r;
  std::shared_ptr<QCircuit> circuit;

  std::vector<int64_t> angles(int64_t a) {
	auto s = binary(a,n+1);
	std::vector<int64_t> angles(n+1);
	for(int64_t i = 0; i < n+1; i++) {
	  for(int64_t j = i; j < n+1; j++) {
		if(s[j]=='1')angles[n-i]+=pow(2,i-j);
	  }
	  angles[n-i]*=M_PI;
	}
	return angles;
  }

  void qft(std::vector<int> qubits, bool inverse=false) {
	std::vector<int> qubitRange(qubits.size());
	std::iota(qubitRange.begin(),qubitRange.end(),0);
    if(inverse)std::reverse(qubitRange.begin(),qubitRange.end());
	for(int j : qubitRange) {
	  std::vector<int> neighbours(j-std::max(0,j-(int)qubits.size()+1));
	  std::iota(neighbours.begin(),neighbours.end(),std::max(0,j-(int)qubits.size()+1));
	  if(inverse) {
		std::reverse(neighbours.begin(),neighbours.end());
		circuit->singleGate(U2(0,M_PI),qubits[j]);
	  }
	  for(int k : neighbours) {
		double lam = M_PI/pow(2,j-k);
		if(inverse)lam*=-1;
		circuit->singleGate(U1(lam*0.5),qubits[j]);
		circuit->cnot(qubits[j],qubits[k]);
		circuit->singleGate(U1(-lam*0.5),qubits[k]);
		circuit->cnot(qubits[j],qubits[k]);
		circuit->singleGate(U1(lam*0.5),qubits[k]);
	  }
	  if(!inverse) {
		circuit->singleGate(U2(0,M_PI),qubits[j]);
	  }
	}
  }

  void phiAdd(int qubit, bool inverse=false) {
	auto angle = angles(N);
	for(int i = 0; i < n+1; i++) {
	  QGate* u;
	  if(inverse) {
		u=U1(-angle[i]);
	  } else {
		u=U1(angle[i]);
	  }
	  circuit->singleGate(u,qubit);
	}
  }

  void cphiAdd(int control, int q, bool inverse=false) {
	auto as = angles(N);
	for(int i = 0; i < n+1; i++) {
	  double angle;
	  if(inverse) {
		angle=-as[i]*0.5;
	  } else {
		angle=as[i]*0.5;
	  }
      circuit->singleGate(U1(angle),control);
	  circuit->cnot(control,q+i);
      circuit->singleGate(U1(-angle),q+i);
	  circuit->cnot(control,q+i);
      circuit->singleGate(U1(angle),q+i);
	}
  }

  void ccphiAdd(int control1, int control2, int q, int64_t a, bool inverse=false) {
	auto angle = angles(a);
	for(int i = 0; i < n+1; i++) {
	  QGate* u;
	  if(inverse) {
		u=CCU1(-angle[i]);
	  } else {
		u=CCU1(angle[i]);
	  }
	  circuit->multipleGate(u,{control1,control2,q+i});
	}
  }

  void ccphiAddModN(int control1, int control2, int q, int aux, int64_t a) {
	ccphiAdd(control1,control2,q,a);
	phiAdd(q,true);
	std::vector<int> qs(n+1);
	std::iota(qs.begin(),qs.end(),q);
	std::reverse(qs.begin(),qs.end());
	qft(qs,true);
	circuit->cnot(q+n,aux);
	qft(qs);
	cphiAdd(aux,q);
	ccphiAdd(control1,control2,q,a,true);
	qft(qs,true);
	circuit->y(q+n);
	circuit->cnot(q+n,aux);
	circuit->y(q+n);
	qft(qs);
	ccphiAdd(control1,control2,q,a);	
  }

  void ccphiAddModNInv(int control1, int control2, int q, int aux, int64_t a) {
	ccphiAdd(control1,control2,q,a,true);
	std::vector<int> qs(n+1);
	std::iota(qs.begin(),qs.end(),q);
	std::reverse(qs.begin(),qs.end());
	qft(qs,true);
	circuit->y(q+n);
	circuit->cnot(q+n,aux);
	circuit->y(q+n);
	qft(qs);
	ccphiAdd(control1,control2,q,a,true);
	cphiAdd(aux,q);
	qft(qs,true);
	circuit->cnot(q+n,aux);
	qft(qs);
	phiAdd(q);
	ccphiAdd(control1,control2,q,a,true);
  }

  void cMulModN(int control, int q, int aux, int64_t a) {
	std::vector<int> qs(n+1);
	std::iota(qs.begin(),qs.end(),aux);
	std::reverse(qs.begin(),qs.end());
	qft(qs);
	for(int i = 0; i < n; i++)ccphiAddModN(q+i,control,aux,aux+n+1,(1<<i)*(a%N));
	qft(qs,true);
	for(int i = 0; i < n; i++) {
	  circuit->cswap(control,q+i,aux+i);
	}
	int64_t aInv = modinv(a,N);
	qft(qs);
	for(int i = 0; i < n; i++)ccphiAddModNInv(q+i,control,aux,aux+n+1,(1<<i)*(aInv%N));
	qft(qs,true);
  }

  int64_t quantumPart() {
	n = ceil(log(N)/log(2));
	circuit = QCircuit::make<smatrix>(4*n+2);
	int aux = 3*n;
	for(int i = 0; i < 2*n; i++)circuit->h(i);
	circuit->x(2*n);
	for(int i = 0; i < 2*n; i++) {
	  cMulModN(i,2*n,3*n,pow(a,1<<i));
	}
	std::vector<int> qs(2*n);
	std::iota(qs.begin(),qs.end(),0);
	qft(qs,true);
	for(int i = 0; i < n; i++) {
	  circuit->swap(i,2*n-i-1);
	}
	std::cout<<"* Warning - Quantum Factorization may take a long time*\n";
	auto result = circuit->simulate(1024,false,0,false);
	deleteGates();
	return std::distance(result.begin(),std::max_element(result.begin(),result.end()));
  }

  public:
  inline Shors(int64_t N): N(N) {}

  std::vector<int64_t> solve() {
	// Initial Random Number: a<N
	srand(time(0));
	a = rand()%(N-2)+2;

	// Early exit, if we're lucky
	if(gcd(a,N)!=1)return {gcd(a,N)};

	// This is where the fun begins
	r = quantumPart();

	// Bad luck, trying again
	if(r%2==1||((int64_t)pow(a,r/2))%N==N-1)return solve();

	// Minimum of one non-trivial solution
	std::vector<int64_t> solutions;
	int64_t s1 = gcd(pow(a,r/2)+1,N);
	int64_t s2 = gcd(pow(a,r/2)-1,N);
	if(N%s1==0)solutions.push_back(s1);
	if(s1!=s2&&N%s2==0)solutions.push_back(s2);
	return solutions;
  }
};

int main(int argc, char **argv) {
  int64_t input = 6;
  Shors s = Shors(input);
  auto solutions = s.solve();
  std::cout<<"Factor";
  if(solutions.size()>1)std::cout<<"s";
  std::cout<<" of "<<input<<":\n";
  std::cout<<solutions[0]<<"\n";
  if(solutions.size()>1)std::cout<<solutions[1]<<"\n";
  return 0; 
}
