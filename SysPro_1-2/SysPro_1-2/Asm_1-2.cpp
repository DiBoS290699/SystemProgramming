/*
*	5) (c* b + 23) / (a / 2 - 4 * b - 1);
*/

#include <iostream>
#include <fstream>
using namespace std;

long CppFunc(long& a, long& b, long& c)
{
	long denominator = (a / 2) - (4 * b) - 1;
	if (!denominator)	//Check equality of the denominator to zero
	{
		cout << "Division by 0 was found. Return value LONG_MAX" << endl;
		return LONG_MAX;
	}
	return (c * b + 23) / denominator;
}

long AsmFunc(long a, long b, long c)
{
	int er_zero = 0, er_of = 0;
	long res;
	//(c * b + 23) / (a / 2 - 4 * b - 1);
	__asm
	{
		mov eax, a;		<eax> == a
		mov ebx, 2;		<ebx> == 2

		cdq;			<eax> = > <edx:eax>
		idiv ebx;		<eax> = a / 2
		push eax;		on the Stack à / 2

		mov eax, b;		<eax> == b
		mov ecx, 4;		<ecx> == 4

		imul ecx;		<edx:eax> == b * 4
		jo err_over;	at overflow->err_over
		mov ebx, eax;	<ebx> == b * 4
		pop eax;		<eax> == a / 2
		dec eax;		<eax> == a / 2 - 1
		jo err_over;	at overflow->err_over
		sub eax, ebx;	<eax> = a / 2 - b * 4 - 1
		jz err_zero;	if the denominator == 0->err_zero
		jo err_over;	at overflow->err_over

		push eax;		on the Stack a / 2 - b * 4 - 1

		mov eax, c;		<eax> == c
		mov ebx, b;		<ebx> == b

		imul ebx;		<edx:eax> == c * b
		jo err_over;	at overflow->err_over
		add eax, 23;	<eax> == c * b + 23
		jo err_over;	at overflow->err_over

		pop ebx;		<ebx> == a / 2 - b * 4 - 1

		cdq;			<eax> => <edx:eax>
		idiv ebx;		<eax> == (c * b + 23) / (a / 2 - 4 * b - 1)
		jo err_over;	at overflow->err_over
		jmp writeRes

		err_zero : 
			mov er_zero, 1;		division by 0 found
			jmp writeRes

		err_over : 
			mov er_of, 1;		overflow detected
			jmp writeRes

		writeRes :
			mov res, eax
	}
	if (er_zero)
	{
		cout << "Division by 0 was found. Return value LONG_MAX" << endl;
		return LONG_MAX;
	}
	else if (er_of)
	{
		cout << "Overflow detected. Returned 0" << endl;
		return 0;
	}
	else return res;
}

void readValue(long& a, long& b, long& c, istream& fin) {
	cout << "Enter three integers: ";
	if (!(fin >> a >> b >> c)) {
		cout << "Incorrect data entered. Close the program." << endl;
		exit(1);
	}
	cout << a << ' ' << b << ' ' << c << '\n';
}

void writeResult(long& cppRes, long& asmRes, ostream& fout) {
	fout << "A function in C++: " << cppRes << endl;
	fout << "A function in assembler: " << asmRes << endl;
}

int main()
{
	setlocale(LC_ALL, "Rus");
	cout << "Read from file? [f-file | else - the console]\n";
	char read;
	long a = 0, b = 0, c = 0;
	read = cin.get();
	while (cin.peek() != '\n' && cin.peek() != EOF) {
		cin.get();
	}
	if (read == 'f' || read == 'F') {
		ifstream fin("input.txt");
		readValue(a, b, c, fin);
		fin.close();
	}
	else {
		readValue(a, b, c, cin);
	}
	ofstream fout("output.txt");
	long cppRes = CppFunc(a, b, c);
	long asmRes = AsmFunc(a, b, c);
	
	writeResult(cppRes, asmRes, cout);
	writeResult(cppRes, asmRes, fout);
	fout.close();
	cout << "The program is completed" << endl;
	return 0;
}