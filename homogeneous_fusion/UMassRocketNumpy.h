#ifndef UMASS_MATRIX_MATH
#define UMASS_MATRIX_MATH



#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#endif
#include <string>
#include <vector>
#include <iostream>
using matrix = std::vector<std::vector<double>>;
class UMassRocketNumpy
{
	public:
		//MatrixMath();
		void print(matrix& m);
		matrix transpose(matrix& m);
		matrix matmul(matrix& m1, matrix& m2);
		matrix numToSquareMatrix(double num, matrix& m);
		friend matrix operator+(matrix& a, matrix& b);
		friend matrix operator-(matrix& a, matrix& b);
		friend matrix operator*(matrix& a, matrix& b);
		friend matrix operator/(matrix& a, matrix& b);
};

//extern UMassRocketNumpy UMassRocketNumpy;
#endif