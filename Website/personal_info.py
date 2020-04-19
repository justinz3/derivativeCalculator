from flask import Flask, render_template, url_for, request, redirect, session
import db
from flask_mail import Mail

def get_name(user_id):
    
    name = db.query_db("user_data.db", "SELECT name FROM personal_info \
        WHERE user_id = ?", (user_id, ))
        
    return name[0][0]
    
def reset_session():
    
    session['username'] = ""
    session['id'] = -1
    session['name'] = ""
    session['result'] = -1
    
    return