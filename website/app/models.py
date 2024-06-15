from app import db

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    website_public_key = db.Column(db.String(500), nullable=False)
    child_public_key = db.Column(db.String(500), nullable=False)
    index_value = db.Column(db.Integer, nullable=False)

    def __repr__(self):
        return f'<User {self.id}>'
