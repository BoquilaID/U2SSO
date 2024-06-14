# app/user.py
from app import db
from app.models import User

def register_user(user_data):
    """
    Function to register a new user.
    :param user_data: Dictionary containing user information.
    :return: Dictionary with registration result.
    """
    website_public_key = user_data.get('website_public_key')
    child_public_key = user_data.get('child_public_key')
    index_value = user_data.get('index_value')

    new_user = User(website_public_key=website_public_key, child_public_key=child_public_key, index_value=index_value)
    db.session.add(new_user)
    db.session.commit()

    return {
        'status': 'success',
        'message': 'User registered successfully.'
    }
