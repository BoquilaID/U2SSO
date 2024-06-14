# app/routes.py
from flask import render_template, request, jsonify
from app import app
from app.user import register_user

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/register', methods=['POST'])
def register():
    user_data = request.json
    public_key = user_data.get('publicKeyPem')
    print(f"Received public key: {public_key}")
    result = register_user(user_data)
    return jsonify(result)
