/*		{3a/(1-b) - 1,	if a > b
*	X = {-25,	if a = b
*		{(b*b - 5)/a,	if a < b
*/
#include <iostream>
#include <fstream>
using namespace std;

long CppFunc(long& a, long& b)
{
	if (a > b) {
		if (b == 1) {
			cout << "Division by 0 was found. Return value LONG_MAX" << endl;
			return LONG_MAX;
		}
		else {
			return 3 * a / (1 - b) - 1;
		}
	}
	else if (a < b) {
		if (a == 0) {
			cout << "Division by 0 was found. Return value LONG_MAX" << endl;
			return LONG_MAX;
		}
		else {
			return (b * b - 5) / a;
		}
	}
	else return -25;
}

long AsmFunc(long a, long b)
{
	int er_zero = 0, er_of = 0;
	long res;
	/*		{3a/(1-b) - 1,	if a > b
	*	X = {-25,	if a = b
	*		{(b*b - 5)/a,	if a < b
	*/
	__asm {
		mov	eax, a; < eax > == a
		mov	ebx, b; < ebx > == b
		cmp	eax, ebx; сравнение a и b
		jg	l_bigger; переход если a > b
		jl	l_smaller; переход если a < b
		mov	eax, -25; < eax > == -25
		jmp	write_res; переход на конец программы
		l_bigger :
			mov ecx, 1;	<ecx> == 1
			sub ecx, ebx;	<ecx> == 1 - b
			jz err_zero;	if the denominator == 0->err_zero
			jo err_over;	at overflow->err_over
			mov ebx, 3;		<ebx> == 3
			imul ebx;	<edx:eax> == 3 * a
			jo err_over;	at overflow->err_over
			cdq
			idiv ecx;	<eax> == 3 * a / (1 - b)
			dec eax;	<eax> == 3 * a / (1 - b) - 1
			jo err_over;	at overflow->err_over
			jmp write_res

		l_smaller :
			or eax, eax; compare a and 0
			jz err_zero;	if the denominator == 0->err_zero
			mov ecx, eax;	<ecx> = a
			mov eax, b;	<eax> == b
			imul eax;	<edx:eax> == b * b
			jo err_over;	at overflow->err_over
			add	eax, -5;	<eax> = b * b Ц 5
			jo err_over;	at overflow->err_over
			cdq
			idiv ecx;	< eax> = (b * b Ц 5) / a
			jmp write_res
		err_zero :
			mov er_zero, 1;		division by 0 found
			jmp write_res

		err_over :
			mov er_of, 1;		overflow detected
			jmp write_res

		write_res :
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

void readValue(long& a, long& b, istream & fin) {
	cout << "Enter three integers: ";
	if (!(fin >> a >> b)) {
		cout << "Incorrect data entered. Close the program." << endl;
		exit(1);
	}
	cout << a << ' ' << b << ' ' << '\n';
}

void writeResult(long& cppRes, long& asmRes, ostream & fout) {
	fout << "A function in C++: " << cppRes << endl;
	fout << "A function in assembler: " << asmRes << endl;
}

int main()
{
	cout << "Read from file? [f-file | else - the console]\n";
	char read;
	long a = 0, b = 0, c = 0;
	read = cin.get();
	while (cin.peek() != '\n' && cin.peek() != EOF) {
		cin.get();
	}
	if (read == 'f' || read == 'F') {
		ifstream fin("input.txt");
		readValue(a, b, fin);
		fin.close();
	}
	else {
		readValue(a, b, cin);
	}
	ofstream fout("output.txt");
	long cppRes = CppFunc(a, b);
	long asmRes = AsmFunc(a, b);

	writeResult(cppRes, asmRes, cout);
	writeResult(cppRes, asmRes, fout);
	fout.close();
	cout << "The program is completed" << endl;
	return 0;
}