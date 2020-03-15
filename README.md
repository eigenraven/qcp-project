# qcp-project
Quantum Computing Project 2019/20 group 2

## Protocols used

The frontend and backend communicate using a simple format specifically designed for ease of use rather than formality.

Each line is defined by ther regex `[A-Za-z]+(,(0\.)?\d+)+`, or, informally, `name,arg1[,arg2...]`. Specifically:

  - Operations may be letters only, though names are case insensitive.
  - Each instruction has at least one argument, and MUST have the amount of arguments specified.
  - A comma exists before each argument, and MUST NOT contain whitespace. A single trailing comma MAY (but SHOULD NOT) be added.
  - All arguments are either positive integers (0 to infinity) or floating points (0 to 1).

The circuit MUST begin with the instruction:

  - **Mandatory header**: one of `qubits` OR `sparsequbits`. One argument: the number of qubits.

The following headers may be applied. Headers MAY be defined any where but SHOULD be defined before gates.

  - **Optional header**: `shots`. One argument, default `1024`: the number of times the circuit is evaluated.
  - **Optional header (float)**: `noise`. One argument, default `0`: the probability per operation per qubit of decay to |0>. Significantly decreases performance if `0`.

Gates MUST be defined in execution order; they are described in `src/qgate.json` and `src/qgate.cpp`. Their arguments all are integers, starting from 0, representing the qubit index(es) they act on. 

  - **Unary gates**: `id` (aka `nop`), `x`, `y`, `z`, `h`, (aka `hadamard`) `s`, `t`, `tinv`, `v`, `vinv`
  - **Binary gates**: `cnot`, `cy`, `cz`, `swap`
  - **Ternary gates**: `ccnot` (aka `toffoli`)
  - **Operators**: none yet implemented

We assume that at the very end all quantum registers are measured. The resulting vector returned from the backend contains the fraction of times the result was returned.

### Demo programs
Two demo programs are written and located in `PROJECT_DIR/demo`

- _entangle.in_: Simple entanglement demo
- _grovers.in_: 3-qubit Grover's Algorithm

## Building and running

### Installing dependencies

CMake 3.10 is required to build the simulator, and Python 3.7 and a decently modern browser to use the basic front-end.
```
brew install cmake
brew install doxygen
python -m pip install flask
```

### Building

```
mkdir build
cd build
cmake ..
make
```

Below instructions assume you are in the `src` directory for instructions.

### Simulating via command line

If you have already defined a file by the format above, simply run it via command-line:

```
./qcsim <input file> [-n[NOISE]] [--sparse] [--nogroup] [--states]
```

To run one of the demos above:
```
./qcsim ../demo/[grovers.in | entangle.in]
```

### Exposing the HTTP API

The simulator also may expose a HTTP API to interact; by default this will be at <http://localhost:12345/version>. Run it with:

```
./http_qcsim
```

### Running the front-end

The front-end allows you to build, save and load a quantum circuit with an editor and send it off to the simulator, rather than tangle with the file format.

```
./http_qcsim
cd ../frontend
python -m flask run
```

### Running unit tests

```
make test
```

### Building HTML docs

Requires Doxygen.

```
make docs
```

They will be located at `PROJECT_DIR/build/docs/html/index.html`

