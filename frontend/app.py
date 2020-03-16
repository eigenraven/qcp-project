import json
from pathlib import Path

from flask import Flask, render_template

_GATES_PATH = Path(__file__).parent.parent / "src" / "qgate.json"

OPERATIONS = {}  # op.name -> op
with open(_GATES_PATH, encoding="utf8") as f:
    for id, data in json.load(f).items():
        op = dict(
            id=id, symbol=id.upper(), name="No name", description="",
            mathjax={
                "factor": data.pop("factor", ""),
                "matrix": "\\\\".join(data.pop("matrix", ["\\text{Not a gate}"]))
        })
        op.update(data)
        OPERATIONS[id] = op

app = Flask(__name__, static_url_path='/static')

@app.route("/")
def index():
    return render_template(
        'builder.html',
        ops=OPERATIONS
    )
