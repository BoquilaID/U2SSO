# app/routes.py
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
        print(f"Received JSON data: {user_data}")
        # For now, let's use dummy values for `child_public_key` and `index_value`
        user_data['website_public_key'] = user_data.get('publicKeyPem')
        user_data['child_public_key'] = "dummy_child_public_key"
        user_data['index_value'] = 1
        result = register_user(user_data)
        return jsonify(result)
    except Exception as e:
        print(f"Error: {str(e)}")
        return jsonify({'status': 'error', 'message': str(e)}), 400
