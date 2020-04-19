from flask import Flask, render_template, url_for, request, redirect, session
import sqlite3

def query_db(db, query, variables):
    
    conn = sqlite3.connect(str(db))
    c = conn.cursor()

    c.execute(str(query), tuple(variables))
    all_rows = c.fetchall()

    conn.commit()
    conn.close()
    
    return all_rows