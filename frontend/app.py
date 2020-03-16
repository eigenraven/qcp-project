import json
from pathlib import Path

from flask import Flask, render_template

_GATES_PATH = Path(__file__).parent.parent / "src" / "qgate.json"

OPERATIONS = dict()
with open(_GATES_PATH, encoding="utf8") as f:
    for data in json.load(f):
        assert "id" in data and "arity" in data
        id_ = data['id']

        op = dict(
            symbol=id_.upper(), kind="general", name="No name", description="",
            mathjax={
                "factor": data.pop("factor", ""),
                "matrix": "\\\\".join(data.pop("matrix", ["\\text{Not a gate}"]))
        })
        op.update(data)
        OPERATIONS[id_] = op

app = Flask(__name__, static_url_path='/static')

@app.route("/")
def index():
    return render_template(
        'builder.html',
        gate_items=OPERATIONS,
        gate_data=json.dumps(OPERATIONS)
    )
