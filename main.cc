#include<iostream>

#include"matrix.h"
#include"matrix_wrap.h"
#include"operations.h"
#include<chrono>
#include <mutex>

int main() {
unsigned m, n;
bool verbose = false;
std::cout << "======= NEW TEST SECTION =======\n";
std::cout << std::endl;
const char* menu = R"(Select Test case: 
	[Parallellism]
		1- small operations  [A op B]: A = (32x32), B = (32x32);
		2- medium operations [A op B]: A = (256x256), B = (256x256) 
		3- large operations  [A op B]: A = (1024x1024), B = (1024x1024)
	[Concurrency]
		4- small sized chain   [A op B op C]
		5- medium sized chain  [A op B op C op D op E]
		6- large sized chain   [A op B op C op D op E op F op G op H]
	[Template Sizes + Parallelism]
		7- normal operations [A op B]: A = (128x128), B = (128x128);
		8- large operations  [C op D]: C = (1024x1024), D = (1024x1024);
)";
std::cout << menu << std::endl;

unsigned choiche;
std::cin >> choiche;
std::cout << "Option chosen: " << choiche<< std::endl;
double etime = omp_get_wtime();
// empty case

m = n = 1;
if (choiche == 4 || choiche == 5 || choiche == 6 || choiche == 7) {
	unsigned s;
	std::cout << "Choose size of matrices: ";
	std::cin >> s;
	std::cout << std::endl;
	m = n = s;
}
else if (choiche == 1) { m = n = 32; }
else if (choiche == 2) { m = n = 128; }
else if (choiche == 3) { m = n = 1024; }

matrix<int> A(m, n), B(m,n), C(m,n), D(m,n);


for (int i = 0; i < m; i++) {
	for (int j = 0; j < n; j++) {
		A(i, j) = 1;
		B(i, j) = 2;
		C(i, j) = 3;
		D(i, j) = 4;
	}
}
matrix<int> S, M;

// Sized
matrix<int,128,128> AA, BB, SS, MM;
matrix<int,1024,1024> CC, DD, SSS, MMM;

switch (choiche)
{
case 1:
case 2:
case 3:
	etime = omp_get_wtime();
	S = A + B;
	M = A * B;
	break;
case 4:
	etime = omp_get_wtime();
	S = A + B + A;
	M = A * B * A;
	break;
case 5:

	etime = omp_get_wtime();
	S = A + B + A + B + A;
	M = A * B * A * B * A;
	break;
case 6:
	etime = omp_get_wtime();
	S = A + B + A + B + A + B + A + B;
	M = A * B * A * B * A * B * A * B;
	break;
case 7:
	etime = omp_get_wtime();
	SS = AA + BB;
	MM = AA * BB;
	break;
case 8:
	etime = omp_get_wtime();
	SSS = CC + DD;
	MMM = CC * DD;
	break;
default:
	break;
}
etime = omp_get_wtime() - etime;
std::cout << "Time elapsed: " << etime << " seconds." << std::endl;
return 0;
}
