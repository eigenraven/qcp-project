# qcp-project
Quantum Computing Project 2019/20 group 2

## Protocols used

While the front-end implements QASM, the backend uses a simple `.csv` format representing a subset. Each line represents an operation with the format:

    name,arg_1,arg_2,...

All arguments are integers. For gates, these arguments are the zero-indexed qubit indexes. An error MUST be raised if the number of 

The following instructions are used:

  - Mandatory header: `runs`. Takes a single argument, the number of runs.
  - Mandatory header: `qubits`. Takes a single argument, the number of qubits.
  - Unary gates: `id`, `x`, `y`, `z`, `h`
  - Binary gates: `cnot`, `cy`, `cz`, `swap`
  - Ternary gates: `ccnot`
  - Operators: none yet implemented

We assume that at the very end all quantum registers are measured. The resulting matrix returned contains the integer count of times the result was returned. The exact format for this will be defined later.

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
