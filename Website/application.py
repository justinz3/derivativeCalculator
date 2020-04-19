from flask import Flask, render_template, url_for, request, redirect, session
from flask_mail import Message, Mail
import login, test, personal_info, problem_generator
import os
import hashlib

app = Flask(__name__)
app.secret_key = os.urandom(24)
mail = Mail(app)

@app.route("/", methods={"GET", "POST"})
def index():

    personal_info.reset_session();

    if request.method == "POST":

        username = request.form.get("username")
        password = request.form.get("password")
        result = login.is_valid_user(username, password)

        if result[0] > 1:
            session['username'] = username
            session['id'] = result[1]
            session['name'] = personal_info.get_name(session['id'])
            session['result'] = test.get_result(session['id'])
            session['problem'] = ""
            session['needs_new_problem'] = True
            return render_template(url_for('menu') + "/logged_in.html")
        else:
            return render_template("index.html", \
                error_message = "Invalid Login")

    else:
        return render_template("index.html", error_message = "")
        
@app.route("/menu", methods={"GET"})
def menu():
    session['result'] = test.get_result(session['id'])
    return render_template(url_for('menu') + "/logged_in.html")
    
@app.route("/delete_account", methods={"GET", "POST"})
def delete_account():
    print(request.method)
    if request.method == "POST":
        print("posted!")
        login.remove_user(session['id'])
        personal_info.reset_session();
        return render_template("index.html", error_message = "Account successfully deleted")
    
    else:
        return render_template(url_for('delete_account') + "/delete_account.html")


@app.route("/test", methods={"GET", "POST"})
def personality_test():

    if request.method == "POST":

        result = test.get_test_result(request)
        
        largest = result[0]
        gibberish = result[1]
        
        test.update_test_results(largest, gibberish, session['id'])

        return render_template(url_for('personality_test') + "/result.html", \
            result = largest, gibberish = gibberish)

    else:
        return render_template(url_for('personality_test') + "/personality_test.html")


@app.route('/create_account', methods = {'GET', 'POST'})
def create_account():
    
    personal_info.reset_session();
    
    create_account_url = url_for('create_account') + '/create_account.html'
    error_message = ""

    if request.method == 'POST':

        username = request.form.get("username")
        pass1 = request.form.get("password")
        pass2 = request.form.get("password2")
        name = request.form.get("name")
        email = request.form.get("email")

        if username == "":
            return render_template(create_account_url, \
                error_message = "Must include a username")
                
        if pass1 == "":
            return render_template(create_account_url, \
                error_message = "Must include a password")

        if pass1 != pass2:
            return render_template(create_account_url, \
                error_message = "Passwords don\'t match")
        
        if email == "":
            return render_template(create_account_url, \
                error_message = "Must include an email")
                
        validity = login.is_valid_user(username, pass1) # validity is supposed to be 0
        if validity[0] > 0:
            return render_template(create_account_url, \
                error_message = "Username already taken")
                
        if login.email_taken(email):
            return render_template(create_account_url, \
                error_message = "Email already taken")

        else:
            login.add_user(username, pass1, name, email)

            return render_template(url_for('index') + '/index.html')

    else:
        return render_template(create_account_url, \
            error_message = "")

@app.route("/compatibility", methods={"GET"})
def compatibility():
    
    user_id = session['id']
    result = test.get_result(user_id)
    others = test.get_others(user_id, result)
    
    return render_template(url_for('compatibility') + "/compatibility.html", \
        others = others, result = result)

@app.route("/reset_password", methods={"GET", "POST"})
def reset_password():
    
    print("here!")
    user_id = int(request.args.get('user_id'))
    
    user_id = int(user_id)
    
    reset_password_url = url_for('reset_password') + "/reset_password.html"
    
    if(request.method == "POST"):
        
        pass1 = request.form.get("password")
        pass2 = request.form.get("password2")
                
        if pass1 == "":
            return render_template(reset_password_url, \
                error_message = "Must include a password", user_id = user_id)

        if pass1 != pass2:
            return render_template(reset_password_url, \
                error_message = "Passwords don\'t match", user_id = user_id)

        login.update_password(session['id'], pass1)
        return render_template(url_for('index') + '/index.html', \
            error_message = "Password successfully updated")
            
    return render_template(reset_password_url, \
        user_id = user_id, username = login.get_login(user_id)[0])
        
        
@app.route("/forgot_password", methods = {"GET", "POST"})
def forgot_password():
    
    forgot_password_url = url_for('forgot_password') + "/forgot_password.html"
    
    message = ""
    
    if request.method == "POST":
        
        email = request.form.get("email")
        id_tuple = login.get_user_from_email(email)
        
        if(len(id_tuple) == 0):
            
            return render_template(forgot_password_url, \
                error_message = "no user with that email")
        
        user_id = int(id_tuple[0][0])
        
        with mail.connect() as conn:
            message = "Click here <a href = \
                '{{ url_for('reset_password', user_id = user_id) }}'></a>"
            subject = "Reset Password"
            msg = Message(recipients=[email],
                          body=message,
                          subject=subject)
            conn.send(msg)
        
        message = "Email sent, check your email"
    
    return render_template(forgot_password_url, error_message = message)
    
@app.route("/calculator", methods = {"GET", "POST"})
def calculator():
    
    calculator_url = url_for('calculator') + "/calculator.html"
    result = ""
    expression = ""
    
    if request.method == "POST":
        
        # get the string and give it to the calculator
        
        expression = request.form.get("expression")
        expression = "(" + expression + ")"
        
        temp = open("/tmp/input.txt", "w")
        
        temp.write(str(1))
        temp.write(str(expression))
        
        temp.close()
        
        os.system("cd /home/ubuntu/workspace/Final_Project/derivative_calc ; \
            g++ calc_derivative.cpp -o calc_derivative ; ./calc_derivative")
        
        temp = open("/tmp/output.txt", "r")
        
        result = temp.readline()
        
        expression = expression[1:len(expression) - 1]
    
    return render_template(calculator_url, result = result, expression = expression)
    

@app.route("/practice", methods = {"GET", "POST"})
def practice():
    
    practice_url = url_for('practice') + "/practice.html"
    select_url = url_for('practice') + "/problem_select.html"
    result = ""
    answer = ""
    expression = ""
    problem = session['problem']
    
    if request.method == "POST" and session['needs_new_problem'] == False:
        
        # get the answer for the given problem
        answer = problem
        answer = "(" + answer + ")"
        temp = open("/tmp/input.txt", "w")
        temp.write(str(1))
        temp.write(str(answer))
        temp.close()
        
        print("Problem " + answer)
        
        os.system("cd /home/ubuntu/workspace/Final_Project/derivative_calc ; \
            g++ calc_derivative.cpp -o calc_derivative ; ./calc_derivative")
        
        temp = open("/tmp/output.txt", "r")
        answer = temp.readline()
        temp.close()
        
        
        if answer == "":
            answer = "Sorry, but our calculator experienced an error Please Try Again"
        elif answer[len(answer) - 1] == "\n":
            answer = answer[:len(answer) - 1]
        
        print("Derivative " + answer)
        
        if not(answer == "Expected *, +, - or )" or answer == "Expected * or / but got +" or \
        answer == "Expected * or / but got -" or answer == "Expected + or - but got *" or \
        answer == "Expected + or - but got /" or \
        answer == "Sorry, but our calculator experienced an error\nPlease Try Again"): 
            # if it did not give an error message
        
            # compare answer with the checker
            expression = request.form.get("expression")
            expression = "(" + expression + ")"
            answer = "(" + answer + ")"
            temp = open("/tmp/input.txt", "w")
            temp.write(str(expression) + "\n" + str(answer) + "\n")
            temp.close()
            
            print("Expressions " + answer + ", " + expression)
            
            os.system("cd /home/ubuntu/workspace/Final_Project/derivative_calc ; \
                g++ check_equal.cpp -o check_equal ; ./check_equal")
            
            temp = open("/tmp/output.txt", "r")
            result = temp.readline()
            temp.close()
            
            print("is_equal " + result)
            
            # answer = expression[1:len(answer) - 1] # remove the parentheses
        
        
        if result == "0\n" or result == "1\n": # it didn't give an error message
            
            # answer = expression[1:len(answer) - 1] # remove the parentheses
            while(answer[len(answer) - 1] != ')'):
                answer = answer[:len(answer) - 1]
            answer = answer[:len(answer) - 1]
            
            while(answer[0] != '('):
                answer = answer[1:]
            answer = answer[1:]
            
            if result == "1\n" or answer == expression or \
            answer == expression[1:len(expression) - 1]: 
                # if is_equal, exactly equal, or exactly equal without extra parentheses
                result = "Your answer matches! Please select a new problem"
                session['needs_new_problem'] = True
            else:
                result = "Our comparator didn't think your answer matched, \
                    but that doesn't mean you're wrong!"
                session['needs_new_problem'] = False
            
        expression = expression[1:len(expression) - 1] # remove the parentheses
            
        return render_template(practice_url, problem = problem, result = result, \
            expression = expression, answer = answer)
            
    elif request.method == "POST":
        
        session['needs_new_problem'] = False
        
        problem_type = request.form.get("problem_type")
        
        if not(problem_type is None):
            problem_type = int(problem_type)
            
            session['problem'] = problem_generator.gen_problem(problem_type)
            
            return render_template(practice_url, problem = session['problem'], result = result, \
                expression = expression, answer = answer)
    
    session['needs_new_problem'] = True
    
    return render_template(select_url)
    