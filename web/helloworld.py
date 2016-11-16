from flask import Flask
app = Flask(__name__)

@app.route('/')
def index():
	return 'index page'
	#return 'Hello, World!'
	
@app.route('/mjpeg')
def stream():
	return 'this is the video stream'