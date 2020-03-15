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

## Building and running

### Installing dependencies

C++ & Python 3.7 required. API runs on web browser.
```
brew install cmake
python -m pip install flask
brew install doxygen
```

### Building

Requires CMake

```
mkdir build
cd build
cmake ..
make
```

### Running the commandline driver
```
./qcsim
```

### Running the HTTP driver
```
./http_qcsim
```
Then navigate to <http://localhost:12345/version>

### Running the front-end

Requires Flask

```
# tested on Python 3.8; should only require 3.7
cd frontend
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
