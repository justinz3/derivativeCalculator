
from flask import Flask, render_template, url_for, request, redirect
import sqlite3

usernames = []
passwords = []
first_time = True
num_users = 0
user_data_url = "user_data/user_data.txt"
num_users_url = "user_data/num_users.txt"

conn = sqlite3.connect("user_data.db")
c = conn.cursor()

def is_valid_user(username, password):

    c.execute('SELECT * FROM {tn} WHERE {cn}= username' .\
        format(tn = "user_data", cn="username"))
    all_rows = c.fetchall()

    # all_rows should be of size 1
    if len(all_rows) == 0:
        return 0; # not found

    if all_rows[0][2] == password:
        return 2

    return 1

    # if(first_time):
    #     load_user_data()

    # print (username + " " + password)
    # print(len(usernames))
    # for i in range(len(usernames)):
    #     print(usernames[i] + " " + passwords[i])

    # for i in range(len(usernames)):
    #     if username == usernames[i] and password == passwords[i]:
    #         return 2
    #     elif username == usernames[i]:
    #         return 1
    # return 0

def add_user(username, password):

    # I don't feel like programming an error message page right now
    if(is_valid_user(username, password) > 0):
        return

    # number = open(url_for('num_users') + "/num_users.html", "r")
    number = open(num_users_url, "r")
    num_users = int(number.readline())
    number.close()

    # print (num_users)

    usernames.append(username)
    passwords.append(password)

    # print(user_data_url)

    # user_data = open(url_for('user_data') + "/user_data.html", "a")
    user_data = open(user_data_url, "a")

    user_data.write(username + " : " + password + "\n")
    user_data.close()

    num_users += 1
    # number = open(url_for('num_users') + "/num_users.html", "w")
    number = open(num_users_url, "w")
    number.write(str(num_users))
    number.close()
    return

def load_user_data():
    # user_data = open(url_for('user_data') + "/user_data.html", "r")
    number = open(num_users_url, "r")
    num_users = int(number.readline())
    number.close()

    user_data = open(user_data_url, "r")

    for i in range(num_users):
        line = user_data.readline()
        line = line[:len(line) - 1] # cut off the endl
        array = line.split(" : ")

        usernames.append(array[0])
        passwords.append(array[1])

        # print(array[0] + " : " + array[1])
    user_data.close()

    first_time = False
    return
