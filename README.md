# qcp-project
Quantum Computing Project 2019/20 group 2

## Protocols used

The frontend and backend communicate using a simple `.csv` format representing a strict, linear subset of QASM. It is comprised purely and only of lines representing an operation with the format:

    name,arg_1,arg_2...

The line fits the regex `[a-z]+(,\d+)*`, which is to say: operations are lower-case letters `[a-z]+` only; arguments, if provided, are integers prepended with a comma `,[0-9]+`. For gates, arguments are zero-indexed qubit indexes. An error MUST be raised if the number of arguments does not match the instruction's arity.

The following instructions are used. They are described in the front-end (data provided in `src/qgate.json` and implemented in `src/qgate.cpp`)

  - Mandatory header: `qubits` or `sparsequbits`. Takes a single argument, the number of qubits.
  - Optional header (default 1024): `shots`. Takes a single argument, the number of shots.
  - Unary gates: `id` (aka `nop`), `x`, `y`, `z`, `h`, `s`, `t`, `tinv`, `v`, `vinv`
  - Binary gates: `cnot`, `cy`, `cz`, `swap`
  - Ternary gates: `ccnot`
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
