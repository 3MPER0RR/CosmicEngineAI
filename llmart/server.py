from flask import Flask, request

app = Flask(__name__)

@app.route("/", methods=["POST"])
def test():
    data = request.get_json()
    return {
        "status": "ok",
        "received": data
    }

app.run(host="0.0.0.0", port=8000)
