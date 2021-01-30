from flask import Flask, jsonify, request
import json

app = Flask(__name__)

data = {'light_state':'off'}

@app.route('/', methods=['GET', 'POST'])
def home():
    global data
    if request.method == 'GET':
        print(data)
        return jsonify(data)
    if request.method == 'POST':
        data_req = request.data
        json_data = json.loads(data_req)
        print(json_data)
        data = json_data.copy()
        return jsonify(data)

app.run('0.0.0.0', port = 5000)
