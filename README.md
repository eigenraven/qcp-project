# qcp-project
Quantum Computing Project 2019/20 group 2

## Directory structure

 - benchmarks: source for the performance benchmarks
 - demo: example quantum circuits for the simulator
 - docs: HTML documentation for the C++ API
 - drivers: source of the command-line and HTTP interfaces to the simulator
 - external: source of the external C++ libraries we have used in this project
 - frontend: the web GUI for the simulator (requires the HTTP backend to be running in the background)
 - src: source for the simulator and linear algebra library
 - tests: unit tests for the linear algebra library

### Quantum circuit protocol and demo programs

Two demo programs are included, and may be loaded by the simulator.

- _entangle.in_: Simple entanglement demo
- _grovers.in_: 3-qubit Grover's Algorithm

There are a few more sample files in that folder, used for performance evaluation of the simulator.

If you want to define your own file, the front-end is an extremely simple and easy-to-use method to export `.in` files. However, they follow a simple file protocol defined in the [protocol readme].

[protocol readme]: PROTOCOL.md

## Building and running

### Installing dependencies

CMake 3.10 and a C++17-compliant compiler are required to build the simulator, and a decently modern browser to use the front-end.

If you have macOS, this is simple: install [Homebrew] and run:
```
brew install cmake
```

[Homebrew]: https://brew.sh

All instructions below assume you are in the `build` folder:

```
mkdir build
cd build
```

### Building

```
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

The build types are as follows:


| Build type       | Optimised performance? | Generates debug symbols?
| ---------------- | ---------------------- | ------------------------
| `Release`        | Yes                    | No
| `Debug`          | No                     | Yes
| `RelWithDebInfo` | Yes                    | Yes

### Simulating via command line

If you have already defined a file by the format above, simply run it via command-line:

```
./qcsim <input file> [--arg1] [--arg2]
```

The arguments can be any valid headers as defined in the [protocol readme], e.g. `--sparse` or `--noise,0.3`. They take precedence over the input file.

To run one of the demos above:
```
./qcsim ../demo/[grovers.in | entangle.in]
```

### Exposing the HTTP API

The simulator also may expose a HTTP API to interact; by default this will be at <http://localhost:12345/version>.
It accepts POST requests to `/simulate` with a single parameter, `circuit`, which is set to a circuit description (as defined by the [protocol readme].)

Start the API server with:

```
./http_qcsim &
```

### Running the front-end

Simply launch the HTTP API, and open `frontend/qbuilder.html` in your favourite browser.

The front-end allows you to build, save and load a quantum circuit with an easy-to-use editor. It instantly shows you results, even as you build a circuit, so you can see the exact inner workings.

## Miscellaneous tasks

### Re-compiling changes to the front-end

The front-end is "compiled" from `frontend/template.html` and `src/qgate.json` for your convenience. If you change them, recompile with:

```
python3 -m pip install jinja2
# every time you change things, run:
python3 frontend/compile/compile.py
```

### Running unit tests

```
make test
```

### Building HTML docs

Doxygen is used for HTML source documentation generation.

```
brew install doxygen
```

Then simply run:

```
make docs
```

They will be located at `PROJECT_DIR/build/docs/html/index.html`
