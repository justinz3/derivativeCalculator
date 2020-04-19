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

ofstream debug_calc("debug_calc.txt");

int main(void)
{
	// =   <-- because some buttons on my keyboard don't work (for copying)

	int n;
	fin >> n; // takes in a number of expressions

	for(int i = 0; i < n; i++)
	{
		struct expression test;
		// debug_calc << "reading in..." << endl;
		system("cd /tmp/ ; cp input.txt /home/ubuntu/workspace/Final_Project/derivative_calc/debug_input.txt");

		test = read_in_expression();
		
		// debug_calc << "Before: ";
		// output_expression(test, &debug_calc);
		// debug_calc << endl;
		// output_expression(expand(test), &debug);
		// debug << endl;

		test = clean_up(expand(clean_up(test))); // just to make sure final answer is in a simpler form, 
						 // which starts by making sure the input is in simplest form
		
		debug << "Simplified expression: ";
		output_expression(test, &debug);
		debug << "\n\n\n\n\n\n\n\n";

		struct expression ans = derivative(test);
		
		ans = clean_up(expand(clean_up(ans)));
		
		debug_calc << "After: ";
		output_expression(expand(ans), &debug_calc);
		debug_calc << endl;

		output_expression(ans, &fout);
		fout << endl;
		
		// debug << "\n\n\n\n\n\n\n";
	}
}