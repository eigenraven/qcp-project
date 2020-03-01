from typing import List
from dataclasses import dataclass

import json
from pathlib import Path

_GATES_PATH = Path(__file__).parent.parent.parent \
    / "src" / "qgate.json"
print(_GATES_PATH)

@dataclass
class Operation:
    """
    An type of circuit operation.
    """
    name: str
    arity: int

    kind: ["gate", "meta"]
    qual_name: str
    description: str
    mathjax: List[str]

OPERATIONS = {}  # op.name -> op

with open(_GATES_PATH) as f:
    for name, data in json.load(f).items():
        mathjax = "\\\\".join(data.get(
            'mathjax', ['(not a gate)']))
        op = Operation(
            name,
            data['arity'],
            data['kind'],
            data.get('name', 'No name'),
            data.get('description', ''),
            mathjax
        )
        OPERATIONS[name] = op
