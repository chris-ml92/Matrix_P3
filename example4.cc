#include<iostream>

#include"matrix.h"
#include"matrix_wrap.h"
#include"operations.h"
#include<chrono>
#include <mutex>

std::mutex g_display_mutex;

matrix<int> sum(matrix<int> x, matrix<int> y) {
	std::thread::id this_id = std::this_thread::get_id();
	g_display_mutex.lock();
	std::cout << "thread " << this_id << " sleeping...\n";
	g_display_mutex.unlock();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return x+y;
	
}

int main() {
/*
matrix<int> A(4,4);
for (int i=0; i!=4; ++i)
	for(int j=0; j!=4; ++j)
	  A(i,j) = (i+1)*10+j+1;
	  
	  
for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << A(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;


auto B=A.transpose();
for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << B(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;

matrix<int> C = A+B+A;
for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << C(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;

matrix<double> D(4,4);
for (int i=0; i!=4; ++i)
	for(int j=0; j!=4; ++j)
	  D(i,j) = 1;

std::cout << "=======\n";
A+B+D;
std::cout << "=======\n";
D+A+B;
std::cout << "=======\n";


matrix<int,4,4> A2;
for (int i=0; i!=4; ++i)
	for(int j=0; j!=4; ++j)
	  A2(i,j) = (i+1)*10+j+1;
	  
	  
for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << A2(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;


auto B2=A2.transpose();
for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << B2(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;

matrix<int,4,4> C2 = A2+B2+A2;
for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << C2(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;


matrix<double,4,4> D2;
for (int i=0; i!=4; ++i)
	for(int j=0; j!=4; ++j)
	  D(i,j) = 1;

std::cout << "=======\n";
A2+B2+D2;
std::cout << "=======\n";
D2+A2+B2;
std::cout << "=======\n";


A+B2;
A2+B;

std::cout << "=======\n";

matrix<int> M = (A+B)*C;

for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << M(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;



std::cout << "=======\n";

matrix<int,4,4> M2 = A2*B2*C2;

for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << M2(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;


matrix<int,4,4> ZA=A;
//matrix<int,4,5> Z2=A2;

std::cout << "=======\n";

matrix<int,4,4> M3 = A*B*C;

for (int i=0; i!=4; ++i) {
	for(int j=0; j!=4; ++j)
		std::cout << M3(i,j) << ' ';
	std::cout << '\n';
}
std::cout << std::endl;
*/
// ----------------------------------------------->
std::cout << "======= NEW TEST SECTION =======\n";

unsigned const m = 4;
unsigned const n = 4;
bool verbose = false;

double etime = omp_get_wtime();

matrix<int> MM(m,n);
for (int i=0; i!=m; ++i)
	for(int j=0; j!=n; ++j)
	  MM(i,j) = (i+1)*(j+1);

matrix<int> MMMM(n,m);
for (int i=0; i!=n; ++i)
	for(int j=0; j!=m; ++j)
	  MMMM(i,j) = (i+1)*(j+1);

matrix<int> B(n, m);
for (int i = 0; i != n; ++i)
for (int j = 0; j != m; ++j)
B(i, j) = (j + 1);

matrix<int> A(n, m);
for (int i = 0; i != n; ++i)
for (int j = 0; j != m; ++j)
A(i, j) = (i + 1);

matrix<int> C(n, m);
for (int i = 0; i != n; ++i)
for (int j = 0; j != m; ++j)
C(i, j) = (i + 1)*(j + 1);

matrix<int,m,n> ZZ = MM*MMMM*MM;

/*if (verbose) {
	for (int i=0; i!=m; ++i) {
		for(int j=0; j!=n; ++j)
			std::cout << ZZ(i,j) << ' ';
		std::cout << '\n';
	}
}*/

std::cout << std::endl;
etime = omp_get_wtime() - etime;
std::cout << "Time elapsed: " << etime << " seconds." << std::endl;

for (int i = 0; i != m; ++i) {
	for (int j = 0; j != n; ++j)
		std::cout << A(i, j) << ' ';
	std::cout << '\n';
}std::cout << '\n';
for (int i = 0; i != m; ++i) {
	for (int j = 0; j != n; ++j)
		std::cout << B(i, j) << ' ';
	std::cout << '\n';
}std::cout << '\n';

for (int i = 0; i != m; ++i) {
	for (int j = 0; j != n; ++j)
		std::cout << C(i, j) << ' ';
	std::cout << '\n';
}

matrix<int> SS = ( A + B ) + C + A  ;
for (int i = 0; i != m; ++i) {
	for (int j = 0; j != n; ++j)
		std::cout << SS(i, j) << ' ';
	std::cout << '\n';
}
matrix<int> AMM = A * B * C;
for (int i = 0; i != m; ++i) {
	for (int j = 0; j != n; ++j)
		std::cout << AMM(i, j) << ' ';
	std::cout << '\n';
}
// Testing Async
/*
auto x = std::async(sum, MM, MM);
auto y = std::async(sum, A, B);
std::this_thread::sleep_for(std::chrono::milliseconds(50));
matrix<int> C = (x.get())*(y.get());*/

return 0;
}
