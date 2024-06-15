from flask import render_template, request, jsonify
from app import app
from app.user import register_user

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/register', methods=['POST'])
def register():
    try:
        user_data = request.json
        print(f"Received JSON data: {user_data}")  # Debugging statement
        user_data['website_public_key'] = user_data.get('publicKeyPem')
        user_data['child_public_key'] = "dummy_child_public_key"
        user_data['index_value'] = 1
        result = register_user(user_data)
        print(f"Registration result: {result}")
        return jsonify(result)
    except Exception as e:
        print(f"Error: {str(e)}")
        return jsonify({'status': 'error', 'message': str(e)}), 400

@app.route('/get_website_name', methods=['GET'])
def get_website_name():
    # Extract the website name from the Host header
    website_name = request.headers.get('Host', 'unknown')
    return jsonify({'website_name': website_name})
