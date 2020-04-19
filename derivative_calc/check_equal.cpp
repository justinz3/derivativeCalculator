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
#include "derivative_calc.cpp"
using namespace std;


int main(void)
{
	// =   <-- because some buttons on my keyboard don't work (for copying)

	struct expression a = read_in_expression();
	struct expression b = read_in_expression();
	sort(a.inside.begin(), a.inside.end());
	sort(b.inside.begin(), b.inside.end());
	
	a = clean_up(a);
	b = clean_up(b);
	
	sort(a.inside.begin(), a.inside.end());
	sort(b.inside.begin(), b.inside.end());
	
	a = clean_up(expand(a));
	b = clean_up(expand(b));
	
	sort(a.inside.begin(), a.inside.end());
	sort(b.inside.begin(), b.inside.end());
	
	// debug << "outputting..." << endl;
	// output_expression(a, &debug);
	// debug << endl;
	// output_expression(b, &debug);
	// debug << endl;
	// debug << "\n\n\n\n\n\n\n\n\n";
	
	// debug << (is_equal(a, b)) << " " << (a.power == b.power) << " " 
	// 	  << (a.coefficient == b.coefficient) << endl;
	// debug << a.power << ", " << b.power << " | " << a.coefficient << ", " << b.coefficient << endl;
	// debug << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	fout << (is_equal(a, b) and a.power == b.power and a.coefficient == b.coefficient) << endl;
}