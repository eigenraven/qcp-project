# qcp-project
Quantum Computing Project 2019/20 group 2

## Protocol used

The frontend and backend communicate using a simple format specifically designed for ease of use rather than formality.

Each line is either empty, a comment (starts with `//`), or a command defined by the regex `[A-Za-z]+(,(0\.)?\d+)*`, or, informally, `name[,arg1,arg2...]`. Specifically:

  - Operations may be letters only, though names are case insensitive.
  - Each instruction MUST have the amount of arguments specified.
  - A comma exists before an argument, and MUST NOT contain whitespace. A single trailing comma MAY (but SHOULD NOT) be added.
  - All arguments are either positive integers (0 to infinity) or floating points (0 to 1).

The circuit MUST begin with the instruction:

  - **Mandatory header**: `qubits`. One argument: the number of qubits.

### Headers and gates

The following headers may be applied; some change state, whereas some change simulation behaviour. Headers MAY be defined any where but SHOULD be defined before gates.

  - **Optional header**: `shots`. One argument, default `1024`: the number of times the circuit is evaluated.
  - **Optional header (float)**: `noise`. One argument, default `0`: the probability per operation per qubit of decay to |0>. Significantly decreases performance if `0`.
  - **Optional header**: `sparse`. Switches the simulator over to sparse matrix types (faster for most cases).
  - **Optional header**: `nogroup`. Applies the gates one-by-one instead of grouping non-overlapping ones into fewer kronecker products
  - **Optional header**: `states`. Makes the simulator output the final probabilities of each qubit state instead of evaluating them with random sampling.

Gates MUST be defined in execution order. (Some gates have multiple aliases.)Information on their use in circuitry can be found in the front-end.

Unless specified, gate arguments are integers, starting from 0, representing the qubit index(es) they act on.

  - **Unary gates**: `id`/`nop`, `x`/`not`, `y`, `z`, `h`/`hadamard` `s`, `t`, `tinv`/`tdg`, `v`, `vinv`/`vdg`
  - **Binary gates**: `swap` (order unimportant)
  
For controlled gates, the target qubit is the final argument.

  - **Binary controlled gates**: `cx`/`cnot`, `cy`, `cz`
  - **Ternary controlled gates**: `cswap`, `ccx`/`ccnot`/`toffoli`

### Simulation output

At the moment, the simulation will definably measure the result of each simulation to a classical bit. Unless `shots` is set to 1, the output therefore represents a probability over the amount of simulations – that is to say, there is no flag set to return a predicted result. 

The result from the simulation may format the output in varying formats. For *q* qubits, we will receive the following:

 - If `states` is set, *2^q* lines are returned corresponding to the individual probabilities of each state: |0⋯00>, |0⋯01>, |0⋯10>, and so on.
 - If `states` is **not** set, *q* lines are returned corresponding to the overall probability of each qubit being measured as zero or one. This is useful for individual tests, but will not capture entanglement of qubits. 


### Demo programs
Two demo programs are written and located in `PROJECT_DIR/demo`

- _entangle.in_: Simple entanglement demo
- _grovers.in_: 3-qubit Grover's Algorithm

There are a few more sample files in that folder, used for performance evaluation of the simulator.

## Building and running

### Installing dependencies

CMake 3.10 and a C++17-compliant compiler are required to build the simulator, and a decently modern browser to use the front-end.
Doxygen is used for HTML source documentation generation.

Example installation instructions with Homebrew:
```
brew install cmake
brew install doxygen
```

### Building

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

Build type can be changed to Release (best performance) or Debug (slow, generates debugger symbols) or RelWithDebInfo (optimized like Release, but has symbols for e.g. profiling).

Below instructions assume you are in the `src` directory for instructions.

### Simulating via command line

If you have already defined a file by the format above, simply run it via command-line:

```
./qcsim <input file> [--arg1] [--arg2]
```

The arguments can be any valid commands from the protocol described above, e.g. `--sparse` or `--noise,0.3`. They take precedence over the commands from the input file.

To run one of the demos above:
```
./qcsim ../demo/[grovers.in | entangle.in]
```

### Exposing the HTTP API

The simulator also may expose a HTTP API to interact; by default this will be at <http://localhost:12345/version>.
It accepts POST requests to `/simulate` with a single parameter, `circuit`, which is set to a circuit description as described in the protocol section.


Start the API server with:

```
./http_qcsim &
```

### Running the front-end

The front-end allows you to build, save and load a quantum circuit with an editor and send it off to the simulator, rather than tangle with the file format.

While it requires the HTTP API to be launched, it is compiled to a static HTML file located at `frontend/qbuilder.html`; simply launch this to run.

If you make any changes to `src/qgate.json` or `frontend/template.html`, compile them by running:

```
python3 -m pip install jinja2
python3 frontend/compile/compile.py
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
