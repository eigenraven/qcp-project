from typing import Dict
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
    mathjax: Dict[str, str]

OPERATIONS = {}  # op.name -> op

with open(_GATES_PATH) as f:
    for name, data in json.load(f).items():
        mathjax = {
            'matrix': '\\\\'.join(data.get('matrix', ['\\text{Not a gate}'])),
            'factor': data.get('factor', '')
        }
        op = Operation(
            name,
            data['arity'],
            data['kind'],
            data.get('name', 'No name'),
            data.get('description', ''),
            mathjax
        )
        OPERATIONS[name] = op
