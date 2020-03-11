# qcp-project
Quantum Computing Project 2019/20 group 2

## Protocols used

While the front-end implements QASM, the backend uses a simple `.csv` format representing a subset. Each line represents an operation with the format:

    name,arg_1,arg_2,...

All arguments are integers. For gates, these arguments are the zero-indexed qubit indexes. An error MUST be raised if the number of 

The following instructions are used. Their names are case-insensitive.

  - Mandatory header: `qubits`. Takes a single argument, the number of qubits.
  - Optional header (default 1024): `shots`. Takes a single argument, the number of shots.
  - Unary gates: `id/nop`, `x`, `y`, `z`, `h`
  - Binary gates: `cnot`, `cy`, `cz`, `swap`
  - Ternary gates: `ccnot/toffoli`
  - Operators: none yet implemented

We assume that at the very end all quantum registers are measured. The resulting vector returned contains the fraction of times the result was returned.

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
And open your web browser to <http://localhost:12345/version>

### Running unit tests
```
make test
```

### Building HTML docs (Requires Doxygen)
```
make docs
```
They will be located at `PROJECT_DIR/build/docs/html/index.html`
