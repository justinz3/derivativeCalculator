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
	a = clean_up(a);
	
	output_expression(a, &fout);
}