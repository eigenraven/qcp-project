from typing import List

import json

_GATES_PATH = "../src/qgate.json"

@dataclass
class Operation:
    """
    An type of circuit operation.
    """
    name: str
    arg_count: int

    kind: ["gate", "meta"]
    qual_name: str
    description: str
    mathjax: List[str]

OPERATIONS = {}  # op.name -> op

with open(_GATES_PATH) as f:
    print(f)
