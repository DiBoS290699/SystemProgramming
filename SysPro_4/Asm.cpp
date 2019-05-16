/*
* Find the number of positive elements of the array A={a[i]}, which 
* satisfy the condition: b <= a[i] <= d.
*/

#include <iostream>
using namespace std;

long CppFunc(long* mas, long n, long b, long d)
{
	int count = 0;
	for (int i = 0; i < n; ++i) {
		long a = *(mas + i);
		if (!(a <= 0 || a < b || a > d)) {
			count++;
		}
	}
	return count;
}

long AsmFunc(long* mas, long n, long b, long d)
{
	int count = 0;

	__asm {
		xor esi, esi;	prepare the index register in the array
		xor edi, edi;	a count of the number of elements
		mov	ebx, mas;	ebx indicates the beginning of the array
		mov	ecx, n;		loop counter for all array elements
		jecxz exitAsm;	end if array length 0
		begin_loop:
			mov	eax, [ebx + esi * 4]; define the current element
			xor	edx, edx;	preparing a comparison with 0
			cmp	eax, edx;	comparison a[i] and 0
			jle	end_loop;	if less than or equal, then complete the cycle
			mov	edx, d;		подготовка сравнени€ с d
			cmp	eax, edx;	сравнение a[i] и d
			jg	end_loop;	if more, then complete the cycle
			mov	edx, b;		preparation of comparison with b
			cmp	eax, edx;	сравнение a[i] и b
			jl	end_loop;	if less, then complete the cycle
			inc	edi;		element satisfies the condition,
			; increase counter
		end_loop :
			inc	esi;		go to the next element
			loop begin_loop; repeat the cycle for all elements array's
		exitAsm : 
			mov	eax, edi; return the number of elements
			mov count, eax;	write the result to the variable count
	}
	return count;
}

void writeResult(long& cppRes, long& asmRes, ostream & fout) {
	fout << "A function in C++: " << cppRes << endl;
	fout << "A function in assembler: " << asmRes << endl;
}


int main()
{
	long n, b, d, temp;;
	cout << "Enter the number of elements: ";
	cin >> n;
	cout << "Enter array element 1: ";
	cin >> temp;
	long* mas = new long[n];
	mas[0] = temp;
	for (int i = 1; i < n; ++i) {
		cout << "Enter array element " << i+1 << ": ";
		cin >> *(mas + i);
	}
	cout << "Enter b: ";
	cin >> b;
	cout << "Enter d: ";
	cin >> d;
	long cppRes = CppFunc(mas, n, b, d);
	long asmRes = AsmFunc(mas, n, b, d);

	cout << "The Array of numbers: ";

	for (int i = 0; i < n; ++i) {
		cout << *(mas + i) << ' ';
	}
	cout << endl;
	writeResult(cppRes, asmRes, cout);
	cout << "The program is completed" << endl;
	delete[] mas;
	return 0;
}