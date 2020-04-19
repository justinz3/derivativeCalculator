from flask import Flask, render_template, url_for, request, redirect, session
import db, personal_info

def get_test_result(request):

    count = [0, 0, 0] # the number of times 1, 2, 3 appear respectively

    choices = ["choice", "dropdown", "choice2"]

    for choice in choices:
        response = request.form.get(choice)
        if response == None:
            continue

        temp = int(response) - 1
        count[temp] += 1

    choices = ["10", "11", "12"]
    for choice in choices:
        response = request.form.get(choice)
        if response == None:
            continue

        temp = int(response) - 1
        count[temp] += 1

    largest = 0
    output = ""
    for i in range(len(count)):
        output += str(count[i])
        if count[largest] < count[i]:
            largest = i
    print (output)

    gibberish = request.form.get("text")
    
    return (largest + 1, gibberish)
    
def update_test_results(test_result, text, user_id):

    all_rows = db.query_db("user_data.db", 'SELECT * FROM test_results \
        WHERE user_id = ?', (user_id, ))
    
    # should only be one unique id
    if len(all_rows) == 0:
        # first time, make new
        db.query_db("user_data.db", "INSERT INTO test_results \
            ('user_id', 'test_result', 'gibberish') VALUES (?, ?, ?)", \
            (user_id, test_result, text))
    else:
        db.query_db("user_data.db", "UPDATE test_results \
            SET user_id = ?, test_result = ?, gibberish = ? WHERE user_id = ?", \
            (user_id, test_result, text, user_id))
    
    return

def get_others(user_id, test_result):
    
    all_rows = db.query_db("user_data.db", 'SELECT user_id FROM test_results \
        WHERE test_result = ? EXCEPT SELECT user_id FROM test_results \
        WHERE user_id = ?', (test_result, user_id))
    
    formatted_data = [("hi", "hi", "hi")]
    
    for i in range(len(all_rows)):
        tempc = personal_info.get_name(all_rows[i][0])
        
        tempa = db.query_db("user_data.db", "SELECT username \
            FROM user_data WHERE user_id = ?", (all_rows[i][0],))
        
        tempb = db.query_db("user_data.db", "SELECT gibberish \
            FROM test_results WHERE user_id = ?", (all_rows[i][0],))
        
        formatted_data.append((tempc[0], tempa[0][0], tempb[0][0]))
    
    formatted_data.pop(0)
    return formatted_data
    
def get_result(user_id):
    all_rows = db.query_db("user_data.db", 'SELECT test_result \
        FROM test_results WHERE user_id = ?', (user_id, ))
        
    if len(all_rows) == 0:
        return -1
    
    return all_rows[0][0]