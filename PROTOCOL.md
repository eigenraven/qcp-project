# qcp-project "qCSV" protocol

The frontend and backend communicate using a simple format specifically designed for ease of use rather than formality.

Each line is either empty, a comment (starts with `//`), or a command defined by the regex `[A-Za-z]+(,(0\.)?\d+)*`, or, informally, `name[,arg1,arg2...]`. Specifically:

  - Operations may be letters only, though names are case insensitive.
  - Each instruction MUST have the amount of arguments specified.
  - A comma exists before an argument, and MUST NOT contain whitespace. A single trailing comma MAY (but SHOULD NOT) be added.
  - All arguments are either positive integers (0 to infinity) or floating points (0 to 1).

The circuit MUST begin with the instruction:

  - **Mandatory header**: `qubits`. One argument: the number of qubits.

## Headers defining output mode

Whether in command-line or HTTP output the simulation will return a list of floating-point numbers. Depending on if the headers `phase` and `states` are provided, the simulation may show the internal state with qubit phase, or measure the result to classical bits and return rational numbers (where `shots` is the denominator) corresponding to the probability of qubits or states.
 
The behaviour is as follows, where *n* is the amount of `qubits` and *x* is the amount of `shots`:

| header      | lines returned | times simulated | returns...                  |  of...
|:----------- | --------------:| ---------------:| --------------------------- | -------------------------
| `phase`     |         *2×2ⁿ* |             *1* | internal state (Re, Im)     | states \|0⋯00>, \|0⋯01>, ⋯
| `states`    |           *2ⁿ* |             *x* | expected prob. ∝ √(Re²+Im²) | states \|0⋯00>, \|0⋯01>, ⋯
| \[default\] |            *n* |             *x* | expected overall prob.      | individual qubits (no entanglement)

### Optional performance headers

These headers, if present, will change the internal mechanisms, usually to be more performant.

  - **Optional header**: `sparse`. Switches the simulator over to sparse matrix types (faster for most cases).
  - **Optional header**: `nogroup`. Applies the gates one-by-one instead of grouping non-overlapping ones into fewer kronecker products
  
### Optional simulation headers

These headers have a default value. They are only considered when physically measuring the circuit: if `phase` is set, these will not apply.

  - **Optional header**: `shots`. One argument, default `1024`: the number of times the circuit is evaluated.
  - **Optional header (float)**: `noise`. One argument, default `0`: the probability per operation per qubit of decay to |0>. Significantly decreases performance if `0`.
  
## Gates

Gates MUST be defined in execution order. Information on their use in circuitry can be found in the front-end, and are defined in `src/gates.json`.

All gate arguments are integers, starting from 0, representing the qubit index(es) they act on. Some gates have multiple aliases.

  - **Unary gates**: `id`/`nop`, `x`/`not`, `y`, `z`, `h`/`hadamard` `s`, `t`, `tinv`/`tdg`, `v`, `vinv`/`vdg`
  - **Binary gates**: `swap` (order unimportant)
  
For controlled gates, the target qubit is the final argument.

  - **Controlled unary gates**: `cx`/`cnot`, `cy`, `cz`
  - **Controlled binary gates**: `cswap`
  - **Doubly-controlled unary gates**: `ccx`/`ccnot`/`toffoli`
