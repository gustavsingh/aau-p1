from flask import Flask, request, Response

app = Flask(__name__)

@app.route('/')
@app.route('/index')
def index():
    return '<p>This is a an Arduino sensoring server.<br>Access is restricted.</p>'

@app.route('/api/data/get')
def get_data():
    if not request.method == 'GET':
        return Response('<p>400 Bad Request</p>', status=400)

@app.route('/api/data/send')
def send_data():
    if not request.method == 'POST':
        return Response('<p>400 Bad Request</p>', status=400)
    
app.run(port=4444)