#!/usr/bin/env python3

import sys

qubits = int(sys.argv[1])
n_args = int(sys.argv[2])

print("// ", ' '.join(sys.argv))

print()
print(f"qubits,{qubits}")

# generate input file
for i in range(0, n_args):
    print()
    gate   = sys.argv[4 * i + 3]
    size   = int(sys.argv[4 * i + 4])
    from_q = int(sys.argv[4 * i + 5])
    to_q   = int(sys.argv[4 * i + 6])

    for j in range(from_q, to_q - size + 2): 
        print(gate, end='')
        for k in range(0, size): 
            print(f",{j + k}", end = '')
        print()
