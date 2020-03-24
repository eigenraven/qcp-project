import json
import os
from pathlib import Path

from jinja2 import Template

BASE_PATH = os.path.split(__file__)[0]
def open2(path, mode="r"):
    return open(
        os.path.abspath(os.path.join(BASE_PATH, path)),
        mode, encoding="utf8")

OPERATIONS = dict()
with open2("../../src/qgate.json") as f:
    for data in json.load(f):
        assert "id" in data
        id = data["id"]
        op = dict(
            control_arities=[0],
            arity=1,
            symbol=id.upper(),
            kind="general",
            name="No name",
            description="",
            mathjax={
                "factor": data.pop("factor", ""),
                "matrix": "\\\\".join(data.pop("matrix", ["\\text{Not a gate}"]))
        })
        op.update(data)
        op['control_modes'] = ", ".join('c' * i for i in op['control_arities'][1:])
        OPERATIONS[id] = op

with open2("template.html") as f:
    template = Template(f.read(-1))

export = template.render(
    gate_items=OPERATIONS,
    gate_data=json.dumps(OPERATIONS)
).replace("\t", "").replace("  ", "")

with open2("../qbuilder.html", "w") as f:
    f.write(export)
