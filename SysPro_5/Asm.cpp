/*
*	5) (c* b + 23) / (a / 2 - 4 * b - 1);
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

double CppFunc(double& a, double& b, double& c)
{
	double denominator = (a / 2) - (4 * b) - 1;
	if (denominator == 0)	//Check equality of the denominator to zero
	{
		cout << "Division by zero was detected in CppFunc. Return value LONG_MAX" << endl;
		return LONG_MAX;
	}
	return (c * b + 23) / denominator;
}

double AsmFunc(double a, double b, double c)
{
	bool er_zero = 0;
	const double c23 = 23, c2 = 2, c4 = 4, c1 = 1;
	double res;
	//(c * b + 23) / (a / 2 - 4 * b - 1);
	__asm
	{
		finit;	coprocessor initialization command
		fld qword ptr[c];	st(0) = c
		fld qword ptr[b];	st(0) = b, st(1) = c
		fmulp st(1), st;	st(0) = c * b
		fadd qword ptr[c23];	st(0) = c * b + 23
		fld qword ptr[a];	st(0) = a, st(1) = c * b + 23
		fdiv qword ptr[c2];	st(0) = a / 2, st(1) = c * b + 23
		fld qword ptr[b];	st(0) = b, st(1) = a / 2, st(2) = c * b + 23
		fmul qword ptr[c4];	st(0) = b * 4, st(1) = a / 2, st(2) = c * b + 23
		fsubp st(1), st;	st(0) = a / 2 - 4 * b, st(1) = c * b + 23
		fsub qword ptr[c1];	st(0) = a / 2 - 4 * b - 1, st(1) = c * b + 23
		fldz;	 st(0) = 0, st(1) = a / 2 - 4 * b - 1, st(2) = c * b + 23
		fucomp;				st(0) ? st(1)
		;					st(0) = a / 2 - 4 * b - 1, st(1) = c * b + 23
		fnstsw ax
		sahf
		jz  error_0;		if a / 2 - 4 * b - 1 = 0->error_0
		fdivp st(1), st;	st(0) = (c * b + 23) / (a / 2 - 4 * b - 1)
		jmp exitAsm
		
		error_0:
			mov er_zero, 1;	er_zero = 1;

		exitAsm:
			fst res;		res = (c * b + 23) / (a / 2 - 4 * b - 1)
	}
	if (er_zero)
	{
		cout << "Division by zero was detected in AsmFunc. Return value LONG_MAX" << endl;
		return LONG_MAX;
	}
	else return res;
}

void readValue(double& a, double& b, double& c, istream & fin) {
	cout << "Enter three integers: ";
	if (!(fin >> a >> b >> c)) {
		cout << "Incorrect data entered. Close the program." << endl;
		exit(1);
	}
	cout << a << ' ' << b << ' ' << c << '\n';
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
		double a = 0, b = 0, c = 0;
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
				readValue(a, b, c, fin);
				fin.close();

			} while (errorFile);
		}
		else {
			readValue(a, b, c, cin);
		}
		ofstream fout("output.txt");
		double cppRes = CppFunc(a, b, c);
		double asmRes = AsmFunc(a, b, c);

		writeResult(cppRes, asmRes, cout);
		writeResult(cppRes, asmRes, fout);
		fout.close();
	}
}