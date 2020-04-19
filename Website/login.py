from flask import Flask, render_template, url_for, request, redirect
import db
import hashlib


def is_valid_user(username, password):

    password = hashlib.sha512(password.encode()).hexdigest()
    all_rows = db.query_db("user_data.db", 'SELECT * FROM user_data WHERE username = ?', \
        (username, ))

    if(len(all_rows) == 0):
        return (0, -1);
    # should only be of size one, maybe program error message otherwise
    if(password == all_rows[0][2]):
        return (2, all_rows[0][0])
    return (1, -1)

def add_user(username, password, name, email):
    
    original = password
    password = hashlib.sha512(password.encode()).hexdigest()
    
    db.query_db("user_data.db", "INSERT INTO user_data ('username', 'password') \
        VALUES (?, ?)", (username, password))
    
    user = is_valid_user(username, original)
    user_id = user[1]

    db.query_db("user_data.db", "INSERT INTO personal_info ('user_id', 'name', 'email') \
        VALUES (?, ?, ?)", (user_id, name, email))
    return

def remove_user(user_id):
    
    db.query_db("user_data.db", "DELETE FROM user_data WHERE user_id = ?", (user_id,))
    db.query_db("user_data.db", "DELETE FROM personal_info WHERE user_id = ?", (user_id,))
    db.query_db("user_data.db", "DELETE FROM test_results WHERE user_id = ?", (user_id,))
    
    return

def email_taken(email):
    
    emails = db.query_db("user_data.db", "SELECT * FROM personal_info WHERE email = ?", \
        (email,))
        
    return len(emails) > 0
    
def update_password(user_id, password):
    
    password = hashlib.sha512(password.encode()).hexdigest()
    
    db.query_db("user_data.db", "UPDATE user_data SET password = ? WHERE user_id = ?", \
            (password, user_id))
    
    return

def get_login(user_id):
    
    user_data = db.query_db("user_data.db", "SELECT username, password FROM user_data \
        WHERE user_id = ?", (user_id,))
        
    return user_data[0] # there should only be one value with that user_id

def get_user_from_email(email):
    
    person = db.query_db("user_data.db", "SELECT user_id FROM personal_info \
        WHERE email = ?", (email,))
        
    return person