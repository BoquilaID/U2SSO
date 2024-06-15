# init_db.py
import os
from app import app, db
from app.models import User

def init_db():
    db_path = os.path.join(os.path.abspath(os.path.dirname(__file__)), 'app.db')
    
    # Remove existing database file if it exists
    if os.path.exists(db_path):
        os.remove(db_path)
        print("Existing database file removed.")
    
    # Create new database tables
    with app.app_context():
        db.create_all()
        print("Database tables created.")

if __name__ == "__main__":
    init_db()
