import random

def gen_problem(user_input):
    random.seed()
    
    exp_low = 1
    exp_high = 4
    
    coeff_low = 1
    coeff_high = 10
    
    problem_type = random.randint(1, 4)
    problem_type = user_input
    
    if problem_type == 1: # just power rule
        coefficient = random.randint(coeff_low, coeff_high)
        power = random.randint(exp_low, exp_high)
        
        return str(coefficient) + "x^" + str(power)
    
    if problem_type == 2: # chain rule
        coefficient1 = random.randint(coeff_low, coeff_high)
        power1 = random.randint(exp_low, exp_high)
        
        coefficient2 = random.randint(coeff_low, coeff_high)
        power2 = random.randint(exp_low, exp_high)
        
        constant = random.randint(-coeff_high, coeff_high)
        
        return str(coefficient2) + "(" + str(coefficient1) + "x^" + str(power1) + \
            " + " + str(constant) + ")^" + str(power2)
    
    if problem_type == 3: # product rule
        
        constant1 = random.randint(-coeff_high, coeff_high)
        constant2 = random.randint(-coeff_high, coeff_high)
        
        power = random.randint(0, 1)
        if power == 0:
            power = -1
        
        return "(x + " + str(constant1) + ") * (x + " + str(constant2) + ")"
    
    if problem_type == 4: # both
        coefficient1 = random.randint(coeff_low, coeff_high)
        power1 = random.randint(exp_low, exp_high)
        
        coefficient2 = random.randint(coeff_low, coeff_high)
        power2 = random.randint(exp_low, exp_high)
        
        constant1 = random.randint(-coeff_high, coeff_high)
        constant2 = random.randint(-coeff_high, coeff_high)
        
        power3 = random.randint(exp_low, exp_high)
        
        return str(coefficient2) + "(" + str(coefficient1) + "(x + " + str(constant1) + ")^" + str(power1) + \
            " * (x + " + str(constant2) + ")^" + str(power2) + ")^" + str(power3)