/*		   { 3a/(1-b) - 1, if a > b
*	5) X = { -25, if a = b
*		   { (b*b-5)/a, if a < b
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

double CppFunc(double& a, double& b)
{
	double denominator;
	if (a > b) {
		denominator = 1 - b;
		if (denominator == 0)	//Check equality of the denominator to zero
		{
			cout << "Division by zero was detected in CppFunc (a > b). Return value LONG_MAX" << endl;
			return LONG_MAX;
		}
		return 3 * a / denominator - 1;
	}
	else if (a < b) {
		if (a == 0)	//Check equality of the denominator to zero
		{
			cout << "Division by zero was detected in CppFunc (a < b). Return value LONG_MAX" << endl;
			return LONG_MAX;
		}
		return (b * b - 5) / a;
	}
	else return -25;
}

double AsmFunc(double a, double b)
{
	bool er_zero = 0;
	const int cNeg25 = -25, c5 = 5;
	const double c3 = 3;
	double res;
	__asm {
		;									st0			st1			st2			 st3
		finit;								coprocessor initialization command
		fld qword ptr[a];					a
		fld qword ptr[b];					b			a
		fcom st(1); compare a and b

		fnstsw ax; save the flags register of the coprocessor
		sahf;

		ja b_bigger; jump if b is greater than
		jb a_bigger; jump if a is greater than
		; если равны
		fild cNeg25;						-25			b			a
		jmp endcalc

	a_bigger :	fld1;						1			b			a
				fcompp; comparison b to 1
				;							a
				fnstsw ax; save the flags register of the coprocessor
				sahf;
				je error; jump if b = 1
				fld1;						1			a
				fsub qword ptr[b];			1 - b		a
				fld qword ptr[c3];			3			1 - b		a
				fmulp st(2), st;			1 - b		3a
				fdivp st(1), st;			3a / (1 - b)
				fld1;						1			3a / (1 - b)
				fsubp st(1), st;			3a / (1 - b) - 1
				jmp endcalc
				

	b_bigger :	fldz;						0			b			a
				fcomp st(2); comparison a to 0
				;							b			a
				fnstsw ax; save the flags register of the coprocessor
				sahf;
				je error; jump if a = 0
				fld qword ptr[b];			b			b			a
				fmulp st(1), st;			b * b		a
				fisub qword ptr[c5];		b * b - 5	a
				fdiv st, st(1);		(b * b - 5) / a		a
				jmp endcalc

	error :		mov er_zero, 1

	endcalc :	fstp res;	res = st(0)
	}
	if (er_zero)
	{
		cout << "Division by zero was detected in AsmFunc. Return value LONG_MAX" << endl;
		return LONG_MAX;
	}
	else return res;
}

void readValue(double& a, double& b, istream & fin) {
	cout << "Enter two integers: ";
	if (!(fin >> a >> b)) {
		cout << "Incorrect data entered. Close the program." << endl;
		exit(1);
	}
	cout << a << ' ' << b << '\n';
}

void writeResult(double& cppRes, double& asmRes, ostream & fout) {
	fout << "A function in C++: " << cppRes << endl;
	fout << "A function in assembler: " << asmRes << endl;
}

void cinClean() {
	while (cin.peek() != '\n' && cin.peek() != EOF) {
		cin.get();
	}
}

int main()
{
	while (true) {
		cout << "Read from file? [f-file | q-quit | else - the console]\n";
		char read;
		double a = 0, b = 0;
		cin >> read;
		cinClean();
		string fileName;
		if (read == 'q' || read == 'Q') {
			cout << "The program is completed" << endl;
			return 0;
		}
		else if (read == 'f' || read == 'F') {
			bool errorFile = false;
			cout << "Enter the name of the file: ";
			do {
				getline(cin, fileName);
				if (fileName == "") {
					errorFile = true;
					continue;
				}
				fileName += ".txt";
				ifstream fin(fileName);
				if (!fin || fin.peek() == '\0') {
					errorFile = true;
					cout << "Enter the name of the file: ";
					continue;
				}
				errorFile = false;
				readValue(a, b, fin);
				fin.close();

			} while (errorFile);
		}
		else {
			readValue(a, b, cin);
		}
		ofstream fout("output.txt");
		double cppRes = CppFunc(a, b);
		double asmRes = AsmFunc(a, b);

		writeResult(cppRes, asmRes, cout);
		writeResult(cppRes, asmRes, fout);
		fout.close();
	}
}