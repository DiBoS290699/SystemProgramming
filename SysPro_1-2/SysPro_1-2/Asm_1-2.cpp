/*
*	5 вариант:	(c* b + 23) / (a / 2 - 4 * b - 1);
*/

#include <iostream>
#include <fstream>
using namespace std;

long CppFunc(long& a, long& b, long& c)
{
	long denominator = (a / 2) - (4 * b) - 1;
	if (!denominator)	//Проверка равенства знаменателя с нулём
	{
		cout << "Обнаружено деление на 0. Возвращено значение LONG_MAX" << endl;
		return LONG_MAX;
	}
	return (c * b + 23) / denominator;
}

long AsmFunc(long a, long b, long c)
{
	int zero = 0, over = 0;
	long res;
	//(c * b + 23) / (a / 2 - 4 * b - 1);
	__asm
	{
		mov eax, a;		<eax> == a
		mov ebx, 2;		<ebx> == 2

		cdq;	<eax> = > <edx:eax>
		idiv ebx;	<eax> = a / 2
		push eax;	в стеке а / 2

		mov eax, b;		<eax> == b
		mov ecx, 4;		<ecx> == 4

		imul ecx;	<edx:eax> == b * 4
		mov ebx, eax;	<ebx> == b * 4
		pop eax;	<eax> == a / 2
		jo err_over;	при переполнении->err_over
		dec eax;	<eax> == a / 2 - 1
		sub eax, ebx;	<eax> = a / 2 - b * 4 - 1
		jz err_zero;	если знаменатель == 0->err_zero
		jo err_over;	при переполнении->err_over

		push eax;	в стеке a / 2 - b * 4 - 1

		mov eax, c;		<eax> == c
		mov ebx, b;		<ebx> == b

		imul ebx;	<edx:eax> == c * b
		jo err_over;	при переполнении->err_over
		add eax, 23;	<eax> == c * b + 23
		jo err_over;	при переполнении->err_over

		pop ebx;	<ebx> == a / 2 - b * 4 - 1

		cdq;	<eax> = > <edx:eax>
		idiv ebx;	<eax> == (c * b + 23) / (a / 2 - 4 * b - 1)
		jo err_over;	при переполнении->err_over
		jmp writeRes

		err_zero : 
			mov zero, 1;		обнаружено деление на 0
			jmp writeRes

		err_over : 
			mov over, 1;		обнаружено переполнение
			jmp writeRes

		writeRes :
			mov res, eax
	}
	if (zero)
	{
		cout << "Обнаружено деление на 0. Возвращено значение LONG_MAX" << endl;
		return LONG_MAX;
	}
	else if (over)
	{
		cout << "Обнаружено переполнение. Возвращено 0" << endl;
		return 0;
	}
	else return res;
}

void readValue(long& a, long& b, long& c, istream& fin) {
	cout << "Введите три целых числа: ";
	if (!(fin >> a >> b >> c)) {
		cout << "Введены некорректные данные. Закрытие программы." << endl;
		exit(1);
	}
	cout << a << ' ' << b << ' ' << c << '\n';
}

void writeResult(long& cppRes, long& asmRes, ostream& fout) {
	fout << "Функция на С++: " << cppRes << endl;
	fout << "Функция на ассемблере: " << asmRes << endl;
}

int main()
{
	setlocale(LC_ALL, "Rus");
	cout << "Чтение из файла? [f-файл | иначе через консоль]\n";
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
	cout << "Работа программы завершена" << endl;
	return 0;
}