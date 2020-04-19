#include <string>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <math.h> 
#include <iomanip>
using namespace std;

ifstream fin("/tmp/input.txt");
ofstream fout("/tmp/output.txt");

ofstream debug("debug.txt");

struct expression // all expressions are in the form: coefficient * ((inside) ^ power)
{
	bool type; // true for product, false for sum

	double power;

	double coefficient;

	vector <struct expression> inside;

	// returns if expression evaluates to 0
	bool is_zero() const
	{
		if(coefficient == 0 and power == 0) // check the power just in case
		{
			return true;
		}
		return false;
	}

	// returns if function is a constant
	bool is_constant() const
	{
		if(power == 0)
		{
			return true;
		}
		return false;
	}

	// used by the sort functions, 
	// it just needs to put identical things next to each other
	bool operator< (struct expression other) const
	{
		// on this case, 
		// we want to separate expressions that are not equal from each other
		// but is_equal isn't defined at this point in the code
		struct expression temp = this->copy();
		if(is_equal_2(&(temp), &other))
		{
			// debug << "BAD!" << endl;
			return false;
		}
		
		// debug << "beginning comparisons" << endl;
		if(inside.size() == other.inside.size()) 
		{
			if(power == other.power)
			{
				if(coefficient == other.coefficient)
				{
					if(type != other.type)
					{
						// debug << "comparison complete 1" << endl;
						return type;
					}
					for(int i = 0; i < inside.size(); i++)
					{
						if(!is_equal_2(&(temp.inside[i]), &(other.inside[i])))
						{
							// debug << "comparison complete i:" << i << endl;
							return (inside[i] < other.inside[i]);
						}
					}
					debug << "BAD!!" << endl;
					return false; // I don't think I have any other criteria
				}
				// debug << "comparison complete 2" << endl;
				return coefficient < other.coefficient;
				
			}
			// debug << "comparison complete 3" << endl;
			return power > other.power;
			
		}
		// debug << this->inside.size() << " " << other.inside.size() << endl;
		// debug << (this->inside.size() < other.inside.size()) << endl;
		// debug << "comparison complete 4" << endl;
		return this->inside.size() < other.inside.size();
		
	}
	
	bool is_equal_2(struct expression *a, struct expression *b) const
	{
		// debug << "entering is_equal_2" << endl;
		if((*a).inside.size() == (*b).inside.size()) 
		{
			if((*a).power == (*b).power)
			{
				if((*a).coefficient == (*b).coefficient)
				{
					if((*a).type != (*b).type)
					{
						// debug << "exiting... 1" << endl;
						return false;
					}
					for(int i = 0; i < (*a).inside.size(); i++)
					{
						if(!is_equal_2(&(*a).inside[i], &(*b).inside[i]))
						{
							// debug << "exiting... i: " << i << endl;
							return false;
						}
					}
					// debug << "EQUAL!" << endl;
					return true; // I don't think I have any other criteria
				}
				// debug << "exiting... 2" << endl;
				return false;
			}
			// debug << "exiting... 3" << endl;
			return false;
		}
		// debug << "exiting... 4" << endl;
		return false;
	}
	
	struct expression copy() const
	{
		struct expression temp;
		temp.inside = this->inside;
		temp.coefficient = this->coefficient;
		temp.power = this->power;
		temp.type = this->type;
		return temp;
	}
};

// derivative functions
struct expression product_rule(struct expression in);
struct expression power_rule(struct expression in);
struct expression chain_rule(struct expression in);
struct expression derivative(struct expression in);
struct expression derivative_inside(struct expression in);

// input functions
int read_exp(void);
struct expression read_in_expression(void);

// comparison functions
bool is_equal(struct expression a, struct expression b, bool type);
bool by_power(struct expression a, struct expression b);

// output function(s)
void output_expression(struct expression in, ofstream *out);

// simplification functions
struct expression clean_up(struct expression in);
struct expression distribute(struct expression a, struct expression b);
struct expression multiplfy(struct expression a, struct expression b);
struct expression expand(struct expression in);

// because-I-needed-it function
int exp(int base, int power);

/*int main(void)
{
	// =   <-- because some buttons on my keyboard don't work (for copying)

	int n;
	fin >> n; // takes in a number of expressions

	for(int i = 0; i < n; i++)
	{
		struct expression test;

		test = read_in_expression();
		
		debug << "Read in" << endl;
		
		output_expression(test, &debug);
		debug << endl;
		output_expression(expand(test), &debug);
		debug << endl;

		test = clean_up(test); // just to make sure final answer is in a simpler form, 
						 // which starts by making sure the input is in simplest form
		
		output_expression(expand(test), &debug);
		debug << endl;
		debug << "Simplified expression: ";
		output_expression(test, &debug);
		debug << "\n\n\n\n\n\n\n\n";

		struct expression ans = derivative(test);

		output_expression(ans, &fout);
		fout << endl;
	}
}*/


// ---------------------------------------------------------------------------------------


struct expression product_rule(struct expression in)
{
	struct expression ans;

	if(in.type != true)
	{
		fout << "product rule was given a sum" << endl;
		exit(2);
	}

	ans.type = false; // sum of the products, it's a holder for the result
	ans.coefficient = ans.power = 1; // just defaults
	ans.inside.clear();

	struct expression terms; // this is the stuff multiplied together
	terms.inside = in.inside;
	terms.coefficient = terms.power = 1;
	terms.type = true;
	
	struct expression original = terms;

	// debug << "start of ans.inside: " << endl;
	// applying the generalized product rule
	for(int i = 0; i < terms.inside.size(); i++) 
	{
		struct expression temp;
		temp = terms.inside[i];
		
		// output_expression(temp, &debug);
		// ;

		terms.inside[i] = derivative(temp);
		
		// output_expression(terms.inside[i], &debug);
		//  << endl;

		ans.inside.push_back(terms);

		terms = original;
	}
	// debug << "end of ans.inside" << endl;
	// debug << "ans : ";
	// output_expression(ans, &debug);
	// ;

	return ans;
}

struct expression power_rule(struct expression in)
{
	struct expression ans;
	if(in.power == 0) // it is a constant so just set everything to 0
	{
		ans.power = 0;
		ans.coefficient = 0;
		return ans;
	}
	
	if(in.power == 1)
	{
		ans.power = 0;
		ans.coefficient = in.coefficient;
		// debug << in.power << " " << in.coefficient << endl;
		return ans;
	}

	ans.coefficient = in.coefficient * in.power; // apply the power rule
	ans.power = in.power - 1;
	ans.type = in.type;
	ans.inside = in.inside;
	
	return ans;
}

struct expression chain_rule(struct expression in)
{
	struct expression ans;
	ans.type = true; // product of f'(g(x)) and g'(x), 
	// so we need a new expression to contain them

	ans.inside.push_back(derivative_inside(in));

	in = power_rule(in); // only affects outer layer, f'(g(x))

	ans.inside.push_back(in);

	ans.coefficient = 1;
	ans.power = 1;
	
	// debug << "Ans.inside: ";
	// for(int i = 0; i < ans.inside.size(); i++)
	// {
	// 	output_expression(ans.inside[i], &debug);
	// 	debug << " , ";
	// }
	// ;

	return ans;
}

struct expression derivative(struct expression in)
{
	if(in.inside.size() == 0)
	{
		return power_rule(in);
	}
	else
	{
		return chain_rule(in);
	}
}

struct expression derivative_inside(struct expression in)
{
	struct expression ans;
	if(in.inside.size() == 0)
	{
		ans.power = 0;
		ans.coefficient = 0;
		ans.type = false;
		return ans;
	}
	if(in.type == false) // added together
	{
		ans.type = false; // sum of the individual derivatives
		ans.coefficient = 1;
		ans.power = 1;
		ans.inside.clear();
		for(int i = 0; i < in.inside.size(); i++)
		{
			// debug << "Before derivative: ";
			// output_expression(in.inside[i], &debug);
			// ;
			
			struct expression temp = derivative(in.inside[i]);
			ans.inside.push_back(temp);
			
			// debug << "After derivative: ";
			// output_expression(temp, &debug);
			// ;
		}
		
		// debug << "Ans.inside: ";
		// for(int i = 0; i < ans.inside.size(); i++)
		// {
		// 	output_expression(ans.inside[i], &debug);
		// 	debug << " , ";
		// }
		// ;
		return ans;
	}
	if(in.type == true) // multiplied together
	{
		// product rule...
		return product_rule(in);
	}
}


// ---------------------------------------------------------------------------------------


int read_exp(void) // just a simple helper function
{
	int exponent;
	fin >> exponent;
	return exponent;
}

struct expression read_in_expression(void)
{
	// debug << __LINE__ << endl;
	char in;
	fin >> in;

	int coefficient;

	if((in >= '0' and in <= '9') or in == '-') // it's a number
	{
		fin.putback(in);
		fin >> coefficient;
	}
	else
	{
		coefficient = 1;
		fin.putback(in);
	}
	
	// debug << __LINE__ << " " << coefficient << endl;

	char x;
	fin >> x;

	int exponent;
	struct expression ans;
	
	ans.coefficient = coefficient;

	if(x != 'x' and x != '(')
	{
		fin.putback(x);
		
		exponent = 0; // it's a constant
		ans.power = exponent;
		
		// debug << __LINE__ << " constant" << endl;
		
		return ans;
	}
	if(x == 'x')
	{
		// return the expression with an exponent, do nothing, 
		// the code after will do that
	}
	
	// debug << __LINE__ << endl;
	
	if(x == '(')
	{
		// recursively read more expressions

		struct expression temp;
		temp = read_in_expression();
		// debug << __LINE__ << endl;

		ans.inside.push_back(temp);

		char next;
		do
		{
			fin >> next;

			if(next != '+' and next != '-' and next !=')' and next != '*' and next != '/')
			{
				fout << "Expected *, /, +, - or )" << endl;
				exit(-1);
			}
			else if(next == '+')
			{
				// conflicts with previous operation
				if(ans.type == true and ans.inside.size() > 1)
				{
					fout << "Expected * or / but got +" << endl;
					exit(-1);
				}
				
				temp = read_in_expression();
				// debug << __LINE__ << endl;
				ans.type = false;
				ans.inside.push_back(temp);
			}
			else if(next == '-')
			{
				// conflicts with previous operation
				if(ans.type == true and ans.inside.size() > 1)
				{
					fout << "Expected * or / but got -" << endl;
					exit(-1);
				}
				
				temp = read_in_expression();
				// debug << __LINE__ << endl;
				temp.coefficient *= -1;
				ans.type = false;
				ans.inside.push_back(temp);
			}
			else if(next == '*')
			{
				// conflicts with previous operation
				if(ans.type == false and ans.inside.size() > 1)
				{
					fout << "Expected + or - but got *" << endl;
					exit(-1);
				}

				temp = read_in_expression();
				// debug << __LINE__ << endl;
				ans.type = true;
				ans.inside.push_back(temp);
			}
			else if(next == '/')
			{
				// conflicts with previous operation
				if(ans.type == false and ans.inside.size() > 1)
				{
					fout << "Expected + or - but got /" << endl;
					exit(-1);
				}

				temp = read_in_expression();
				// debug << __LINE__ << endl;
				temp.power *= -1;
				ans.type = true;
				ans.inside.push_back(temp);
			}
			else if(next == ')')
			{
				break;
			}
		}while(next != ')');
	}
	
	// debug << __LINE__ << endl;

	char carat;
	fin >> carat;

	if(carat != '^')
	{
		fin.putback(carat);

		exponent = 1;
	}
	else
	{
		exponent = read_exp();
	}
	ans.power = exponent;
	
	// debug << __LINE__ << " " << exponent << endl;
	// output_expression(ans, &debug);

	return ans;
}


// ---------------------------------------------------------------------------------------


// only checks if stuff inside is equal
bool is_equal(struct expression a, struct expression b) // type works like the struct
{
	// put stuff in order, puts equal stuff together
	sort(a.inside.begin(), a.inside.end());
	sort(b.inside.begin(), b.inside.end());
	
	a = clean_up(expand(a));
	b = clean_up(expand(b));
	
	// put stuff in order, puts equal stuff together
	sort(a.inside.begin(), a.inside.end());
	sort(b.inside.begin(), b.inside.end());
	
	// debug << "printing (" << endl;
	// output_expression(a, &debug);
	// debug << " " << a.inside.size() << endl;
	// output_expression(b, &debug);
	// debug << " " << b.inside.size() << endl;
	// debug << ") insides: " << a.inside.size() << " " << b.inside.size() << endl << endl;
	
	if(a.inside.size() == 0 and b.inside.size() == 0)
	{
		return true;
	}
	if(a.inside.size() != b.inside.size())
	{
		// debug << 2 << endl;
		return false;
	}

	for(int i = 0; i < a.inside.size(); i++)
	{
		if(a.inside[i].coefficient != b.inside[i].coefficient)
		{
			// debug << 3 << " " << i << endl;
			return false;
		}
		if(a.inside[i].power != b.inside[i].power)
		{
			// debug << 4 << " " << i << endl;
			return false;
		}
		if(is_equal(a.inside[i], b.inside[i]) == false)
		{
			// we already know all other parts of it are equal
			// debug << 5 << " " << i << endl;
			return false;
		}
	}
	
	// debug << 6 << endl;
	return true;
}

bool by_power(struct expression a, struct expression b)
{
	if(a.power == b.power)
	{
		return a.coefficient < b.coefficient;
	}
	return a.power > b.power;
}


// ---------------------------------------------------------------------------------------


void output_expression(struct expression in, ofstream *out)
{
	sort(in.inside.begin(), in.inside.end());
	// debug << "sorted" << endl;
	// in = clean_up(expand(in));
	
	// debug << __LINE__ << endl;
	if(in.is_zero()) 
	{
		(*out) << "0";
		return;
	}
	if(in.is_constant())
	{
		(*out) << in.coefficient;
		return;
	}
	
	// debug << __LINE__ << endl;
	
	// debug << "(power: " << in.power << ")";

	if(in.inside.size() == 0)
	{
		if(in.coefficient != 1)
		{
			(*out) << in.coefficient;
		}
		(*out) << "x";
		if(in.power != 1)
		{
			(*out) << "^" << in.power;
		}
		return;
	}

	if(in.coefficient != 1)
	{
		(*out) << in.coefficient;
	}
	
	// debug << __LINE__ << endl;

	// if(in.inside.size() > 1 or (in.inside.size() == 1 and in.coefficient != 1))
		(*out) << "(";

	for(int i = 0; i < in.inside.size(); i++) // recursively print stuff inside
	{
		// debug << "i " << i << endl;
		struct expression temp = in.inside[i];
		output_expression(temp, out);
		if(i < in.inside.size() - 1)
		{
			if(in.type)
			{
				(*out) << " * ";
			}
			else
			{
				(*out) << " + ";
			}
		}
	}
	// if(in.inside.size() > 1 or (in.inside.size() == 1 and in.coefficient != 1))
		(*out) << ")";

	if(in.power != 1)
	{
		(*out) << "^" << in.power;
	}
	
	// debug << __LINE__ << endl;
}


// ---------------------------------------------------------------------------------------


struct expression clean_up(struct expression in)
{
	if(in.is_zero())
	{
		in.inside.clear();
		in.power = 0;
		return in;
	}

	if(in.is_constant()) // it's a constant
	{
		in.inside.clear();
		in.power = 0;
		return in;
	}
		
	if(in.inside.size() == 0)
	{
		return in;
	}
	
	// debug << "< individual cleaning: ";
	// output_expression(in, &debug);
	// ;
	sort(in.inside.begin(), in.inside.end());
	for(int i = 0; i < in.inside.size(); i++)
	{
		in.inside[i] = clean_up(in.inside[i]); // clean up those first
	}
	
	// debug << "> individual cleaning: ";
	// output_expression(in, &debug);
	// ;
	
	if(in.is_zero())
	{
		in.inside.clear();
		in.power = 0;
		return in;
	}

	if(in.is_constant()) // it's a constant
	{
		in.inside.clear();
		in.power = 0;
		return in;
	}
		
	if(in.inside.size() == 0)
	{
		return in;
	}

	// while(in.inside.size() == 1) // bring everything up a level
	// {
	// 	in.type = in.inside[0].type;
	// 	in.coefficient *= exp(in.inside[0].coefficient, in.power);
	// 	in.power *= in.inside[0].power;

	// 	vector <struct expression> temp = in.inside[0].inside;
	// 	in.inside.clear();
	// 	in.inside = temp;
	// }
	
	// debug << "before cleaning: ";
	// output_expression(in, &debug);
	// debug << endl;

	if(in.type == true) // product
	{
		// debug << "\nmultiplying" << endl;
		// debug << "( Before anything: ";
		// for(int i = 0; i < in.inside.size(); i++)
		// {
		// 	debug << (in.inside[i].power == 0 ? "constant " : "not ");
		// 	output_expression(in.inside[i], &debug);
		// 	debug << " ";
		// }
		// debug << endl;
		
		// could also merge stuff that is the same base
		sort(in.inside.begin(), in.inside.end());
		
		// debug << "constants ";
		// clean constants
		for(int i = 0; i < in.inside.size(); i++)
		{
			// debug << in.inside[i].is_constant() << " ";
			if(in.inside[i].is_constant())
			{
				if(in.inside[i].is_zero()) // everything is now 0
				{
					debug << "zero!" << endl;
					in.power = 0;
					in.coefficient = 0;
					in.inside.clear();
					break;
				}
				in.coefficient *= exp(in.inside[i].coefficient, in.power);

				in.inside.erase(in.inside.begin() + i);
				i--;
			}
		}
		// debug << endl;
		
		if(in.inside.size() == 0) // everything was a constant
		{
			in.power = 0;
		}

		for(int i = 1; i < in.inside.size(); i++)
		{
			if(in.inside[i].is_constant() or in.inside[i - 1].is_constant())
			{
				debug << "BAD!!!" << endl;
			}
			
			// debug << "should merge: " << (is_equal(in.inside[i], in.inside[i - 1])) << " " 
			// 	  << (in.inside[i].is_constant() == in.inside[i - 1].is_constant()) << endl;
			
			if(is_equal(in.inside[i], in.inside[i - 1]) and 
				(in.inside[i].is_constant() == in.inside[i - 1].is_constant())) // they have the same base
			{
				// debug << "Before: ";
				// output_expression(in.inside[i], &debug);
				// debug << " * ";
				// output_expression(in.inside[i - 1], &debug);
				// debug << endl;
				
				// debug << in.inside[i].power << " " << in.inside[i - 1].power << endl;
				in.inside[i].power += in.inside[i - 1].power;
				in.inside[i].coefficient *= in.inside[i - 1].coefficient;
				
				// debug << "After: ";
				// output_expression(in.inside[i], &debug);
				// debug << endl;

				in.inside.erase(in.inside.begin() + i - 1);
				i--;
			}
		}

		// debug << ") After removal: ";
		// for(int i = 0; i < in.inside.size(); i++)
		// {
		// 	debug << (in.inside[i].is_constant() ? "constant " : "not ");
		// 	output_expression(in.inside[i], &debug);
		// 	debug << " ";
		// }
		// debug << endl;
	}
	else // sum
	{
		// debug << "\nadding" << endl;
		
		// debug << "( Before anything: ";
		// for(int i = 0; i < in.inside.size(); i++)
		// {
		// 	output_expression(in.inside[i], &debug);
		// 	debug << " ";
		// }
		// debug << endl;
		
		// clean zeroes
		for(int i = 0; i < in.inside.size(); i++)
		{
			if(in.inside[i].is_zero())
			{
				in.inside.erase(in.inside.begin() + i);
				i--;
			}
		}
		
		if(in.inside.size() == 0) // everything evaluated to 0
		{
			in.power = 0;
			in.coefficient = 0;
		}
		
		// debug << "Before sorting: ";
		// for(int i = 0; i < in.inside.size(); i++)
		// {
		// 	output_expression(in.inside[i], &debug);
		// 	debug << " ";
		// }
		// debug << endl;

		// could also merge stuff that is the same base and power
		sort(in.inside.begin(), in.inside.end());
		
		// debug << "After sorting: ";
		// for(int i = 0; i < in.inside.size(); i++)
		// {
		// 	output_expression(in.inside[i], &debug);
		// 	debug << " ";
		// }
		//  << endl;

		for(int i = 1; i < in.inside.size(); i++)
		{
			// they have the same base and power
			if(is_equal(in.inside[i], in.inside[i - 1]) and 
				in.inside[i].power == in.inside[i - 1].power) 
			{
				// debug << "Before: ";
				// output_expression(in.inside[i], &debug);
				// debug << " + ";
				// output_expression(in.inside[i - 1], &debug);
				// ;
				in.inside[i].coefficient += in.inside[i - 1].coefficient;
				// debug << "After: ";
				// output_expression(in.inside[i], &debug);
				// ;

				in.inside.erase(in.inside.begin() + i - 1);
				// debug << " " << in.inside.size() << endl;
				i--;
			}
		}
		
		// debug << ") After removal: ";
		// for(int i = 0; i < in.inside.size(); i++)
		// {
		// 	output_expression(in.inside[i], &debug);
		// 	debug << " ";
		// }
		// debug << endl;

	}
	// debug << "Before: ";
	// output_expression(in, &debug);
	// debug << endl;
	
	while(in.inside.size() == 1) // bring everything up a level
	{
		// output_expression(in, &debug);
		// debug << " " << in.inside.size() << endl;
		in.type = in.inside[0].type;
		in.coefficient *= exp(in.inside[0].coefficient, in.power);
		// debug << __LINE__ << " " << in.power << " " << in.inside[0].power << " " 
			  //<< (in.power * in.inside[0].power) << endl;
		in.power = in.power * in.inside[0].power;
		// debug << __LINE__ << " " << in.inside.size() << endl;
		// debug << __LINE__ << " " << in.inside[0].inside.size() << endl;
	
		vector <struct expression> temp = in.inside[0].inside;
		in.inside.clear();
		in.inside = temp;
	
		// debug << __LINE__ << endl;
	}
	
	// debug << "After:  ";
	// output_expression(in, &debug);
	// debug << endl;
	
	return in;
}

struct expression distribute(struct expression in)
{
	if(in.type) // multiplication
	{
		// something
	}
	else
	{
		// distribute somehow...
		for(int i = 0; i < in.inside.size(); i++)
		{
			in.inside[i] = distribute(in.inside[i]);
		}
	}
	
	return in;
}

struct expression multiply(struct expression a, struct expression b)
{
	// multiply all elements of b by a
	
	a = expand(a);
	b = expand(b);
	
	if(a.inside.size() == 0 or a.inside.size() == 1)
	{
		if(a.inside.size() == 0)
		{
			b = distribute(a, b);
		}
	}
	
	if(a.type) // multiply
	{
		
	}
}

struct expression distribute(struct expression a, struct expression b) // a has to a single term
{
	b = expand(b); // b.power should be 1 now
	if(a.inside.size() != 0 or b.power != 1)
	{
		return b; // it isn't supposed to do distribution like that
	}
	if(b.inside.size() == 0) // they are both x terms
	{
		b.coefficient *= a.coefficient;
		b.power += a.power;
		return b;
	}
	
	if(b.type == false) // added together, so distribute to all terms
	{
		struct expression temp = a;
		temp.coefficient *= b.coefficient;
		for(int i = 0; i < b.inside.size(); i++)
		{
			if(b.inside[i].inside.size() == 0) // they are both x terms
			{
				b.inside[i].coefficient *= temp.coefficient;
				b.inside[i].power += temp.power;
			}
			else
			{
				b.inside[i] = distribute(temp, b.inside[i]);
			}
		}
	}
	else // multiplied together, so distribute to only one term
	{
		struct expression temp = a;
		temp.coefficient *= b.coefficient;
		if(b.inside[0].inside.size() == 0) // they are both x terms
		{
			b.inside[0].coefficient *= temp.coefficient;
			b.inside[0].power += temp.power;
		}
		else
		{
			b.inside[0] = distribute(temp, b.inside[0]);
		}
	}
	
	return b;
	
}

struct expression expand(struct expression in)
{
	return in; // do this until the bug is fixed
	
	// takes an expression with an exponent and expands it to a polynomial
	
	if(in.inside.size() == 0) // it's just x inside,
	{
		return in;
	}
	
	for(int i = 0; i < in.inside.size(); i++)
	{
		in.inside[i] = expand(in.inside[i]);
	}
	
	if(in.power == 1) // if it doesn't need expansion
	{
		return in;
	}
	
	struct expression ans = in;
	// leave the coefficent unchanged
	ans.power = 1;
	struct expression temp = in;
	// leave type unchanged
	temp.coefficient = 1;
	temp.power = 1;
	
	ans.type = true; // multiply
	ans.inside.clear();
	for(int i = 0; i < in.power; i++)
	{
		ans.inside.push_back(temp);
	}
	// debug << ans.inside.size() << " " << in.power << " " << 
	// ans.coefficient << " " << in.coefficient << endl;
	
	return ans;
}

// ---------------------------------------------------------------------------------------


int exp(int base, int power) // naive exponent function
{
	int ans = 1;
	for(int i = 0; i < power; i++)
	{
		ans *= base;
	}
	return ans;
}




