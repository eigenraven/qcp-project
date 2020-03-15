# qcp-project
Quantum Computing Project 2019/20 group 2

## Protocols used

The frontend and backend communicate using a simple `.csv` format representing a strict, linear subset of QASM. It is comprised purely and only of lines representing an operation with the format:

    name,arg_1,arg_2...

The line fits the general regex `[a-z]+(,(0\.)?\d+)+`. Specifically:
  - Operations are lower-case letters `[a-z]+` only. Each instruction has at least one argument.
  - Arguments are prepended with a comma.
  - An error MUST be raised if the number of arguments does not match the instruction's arity, as provided below.
  - All arguments are either positive integers (0 to infinity) or floating points (0 to 1).

The following instructions are used. They are described in the front-end (data provided in `src/qgate.json` and implemented in `src/qgate.cpp`)

  - **Mandatory header**: `qubits`. One argument: the number of qubits.
  - **Optional header**: `sparse`. One argument, default `0`; if 1, a sparse matrix is used.
  - **Optional header**: `shots`. One argument, default `1024`: the number of times the circuit is evaluated.
  - **Optional header (float)**: `noise`. One argument, default `0`: the probability per operation per qubit of decay to |0>. Significantly decreases performance if `0`.
  - **Unary gates**: `id` (aka `nop`), `x`, `y`, `z`, `h`, (aka `hadamard`) `s`, `t`, `tinv`, `v`, `vinv`
  - **Binary gates**: `cnot`, `cy`, `cz`, `swap`
  - **Ternary gates**: `ccnot` (aka `toffoli`)
  - Operators: none yet implemented

We assume that at the very end all quantum registers are measured. The resulting vector returned from the backend contains the fraction of times the result was returned.

## Building and running

### Building

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

```
# tested on Python 3.8; should only require 3.7
python -m pip install flask
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
