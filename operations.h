#ifndef OPERATIONS_H
#define OPERATIONS_H

#include<type_traits>
#include<list>
#include<omp.h>

#include"matrix.h"

template<typename T, typename U>
struct op_traits {
	typedef decltype(T() + U()) sum_type;
	typedef decltype(T() * U()) prod_type;
};

/*#####################################################################################*/
									/*MATRIX SUM*/
/*#####################################################################################*/

template<typename T,unsigned h, unsigned w>
class matrix_sum {
	public:

	matrix<T> singleAddiction(matrix<T>& a, matrix<T>& b) {
		const unsigned height = a.get_height();
		const unsigned width = a.get_width();

		matrix<T> m(height, width);

		#pragma omp parallel
		{
			int i, j;
			#pragma omp for
			for (i = 0; i < height; ++i) {
				for (j = 0; j < width; j++) {
					m(i, j) = a(i, j) + b(i, j);
				}
			}
		}
		return m;

	}


	matrix<T> subAddiction(std::vector<matrix_wrap<T>> A, int i, int j) {
		if (i == j) {
			/*std::thread::id this_id = std::this_thread::get_id();
			std::cout << "thread " << this_id << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));*/
			return A[i];
		}

		int m = (i + j) / 2;
		std::future<matrix<T>> X = std::async([&] {return subAddiction(A, i, m); });
		std::future<matrix<T>> Y = std::async([&] {return subAddiction(A, m+1, j); });
		matrix<T> r1 = X.get();
		matrix<T> r2 = Y.get();
		return singleAddiction(r1, r2);
	}

	matrix<T> addMatrices(std::vector<matrix_wrap<T>> list) {

		unsigned ah = list.front().get_height();
		unsigned aw = list.front().get_width();
		matrix<T> result(ah, aw);
		unsigned n = list.size();
		return subAddiction(list,0,n-1);
	}

	static constexpr unsigned H=h;
	static constexpr unsigned W=w;

	// Base
	operator matrix<T>() {
		return addMatrices(matrices);
	}
	
	// Templated sizes
	template<unsigned h2, unsigned w2>
	operator matrix<T,h2,w2>() {
		return addMatrices(matrices);
	}
	
	// Sizes of matrices, they are all equal in sums
	unsigned get_height() const { return matrices.front().get_height(); }
	unsigned get_width() const { return matrices.front().get_width(); }
	
	/*OUR VERSION*/
	template<typename U, class LType, class RType>
	friend matrix_sum<U,matrix_ref<U,LType>::H, matrix_ref<U, LType>::W>
		operator + (const matrix_ref<U, LType>& left, const matrix_ref<U, RType>& right);

	template<typename U, class RType, unsigned h2, unsigned w2>
	friend matrix_sum<U,h2,w2>
		operator + (matrix_sum<U,h2,w2>&& left, const matrix_ref<U, RType>& right);


		/*PROF'S VERSION*/
	/*template<typename U, class LType,typename P, class RType>
	friend std::enable_if_t<matrix_ref<U, LType>::H*matrix_ref<P, RType>::H == 0, matrix<typename op_traits<U, P>::sum_type>>
		operator + (const matrix_ref<U, LType>& left, const matrix_ref<P, RType>& right);

	template<typename U, typename P, class RType>
	friend std::enable_if_t<std::is_same<U, typename op_traits<U, P>::sum_type>::value, matrix<U>>
		operator + (matrix<U>&& left, const matrix_ref<P, RType>& right);
	
	template<typename U, class LType, typename P, class RType>
	friend std::enable_if_t<matrix_ref<U, LType>::H*matrix_ref<P, RType>::H != 0, matrix<typename op_traits<U, P>::sum_type, matrix_ref<U, LType>::H, matrix_ref<U, LType>::W>>
		operator + (const matrix_ref<U, LType>& left, const matrix_ref<P, RType>& right);

	template<typename U, unsigned H, unsigned W, typename P, class RType>
	friend std::enable_if_t<matrix_ref<P, RType>::H != 0 && std::is_same<U, typename op_traits<U, P>::sum_type>::value, matrix<U, H, W>>
		operator + (matrix<U, H, W>&& left, const matrix_ref<P, RType>& right);
	*/
	
	matrix_sum(matrix_sum<T,h,w>&& X) = default;
	
	private:
	matrix_sum() = default; 
	
	template<unsigned w2>
	matrix_sum(matrix_sum<T,h,w2>&& X) : matrices(std::move(X.matrices)) {}
	
	template<class matrix_type>
	void add(matrix_ref<T,matrix_type> mat) {
		matrices.emplace_back(mat);
	}
	
	// Elenco matrici da sommare
	std::vector<matrix_wrap<T>> matrices;
};

/*OUR VERSION WITH MATRIX_SUM*/
template<typename T, class LType, class RType>
matrix_sum<T, matrix_ref<T, LType>::H, matrix_ref<T, LType>::W>
operator + (const matrix_ref<T, LType>& left, const matrix_ref<T, RType>& right) {
	if (left.get_height() != right.get_height() || left.get_width() != right.get_width())
		throw std::domain_error("dimension mismatch in Matrix addition");

	matrix_sum<T, matrix_ref<T, LType>::H, matrix_ref<T, RType>::W> result;
	result.add(left);
	result.add(right);
	return result;
}

template<typename T, class RType, unsigned h, unsigned w>
matrix_sum<T, h, w>
operator + (matrix_sum<T, h, w>&& left, const matrix_ref<T, RType>& right) {

	if (left.get_height() != right.get_height() || left.get_width() != right.get_width())
		throw std::domain_error("dimension mismatch in Matrix addition");

	matrix_sum<T, h, w > result(std::move(left));
	result.add(right);
	return result;
}

/*VERSIONE PROF*/
/*template<typename T, class LType, typename U, class RType>
std::enable_if_t<matrix_ref<T, LType>::H*matrix_ref<U, RType>::H == 0, matrix<typename op_traits<T, U>::sum_type>>
operator + (const matrix_ref<T, LType>& left, const matrix_ref<U, RType>& right) {
	if (left.get_height()!=right.get_height() || left.get_width()!=right.get_width())
		throw std::domain_error("dimension mismatch in Matrix addition");
	matrix_sum<T, matrix_ref<T, LType>::H, matrix_ref<U, RType>::W> result;
	result.add(left);
	result.add(right);
	return result;
}

template<typename T, typename U, class RType>
std::enable_if_t<std::is_same<T, typename op_traits<T, U>::sum_type>::value, matrix<T>>
operator + (matrix<T>&& left, const matrix_ref<U, RType>& right) {
	if (left.get_height() != right.get_height() || left.get_width() != right.get_width())
		throw std::domain_error("dimension mismatch in Matrix addition");
	//matrix_sum<T, matrix_ref<U,RType>::H, matrix_ref<U, RType>::W> result(std::move(left));
	matrix_sum<T, matrix_ref<U, RType>::W, matrix_ref<U, RType>::W> result;
	result.add(left);
	result.add(right);
	return std::move(result);
}

template<typename T, class LType, typename U, class RType>
std::enable_if_t<matrix_ref<T, LType>::H*matrix_ref<U, RType>::H != 0, matrix<typename op_traits<T, U>::sum_type, matrix_ref<T, LType>::H, matrix_ref<T, LType>::W>>
operator + (const matrix_ref<T, LType>& left, const matrix_ref<U, RType>& right) {
	static_assert(matrix_ref<T, LType>::H == matrix_ref<U, RType>::H && matrix_ref<T, LType>::W == matrix_ref<U, RType>::W,
		"dimension mismatch in Matrix addition");
	matrix_sum<T, matrix_ref<T, LType>::H, matrix_ref<U, RType>::W> result;
	result.add(left);
	result.add(right);
	return result;
}

template<typename T, unsigned H, unsigned W, typename U, class RType>
std::enable_if_t<matrix_ref<U, RType>::H != 0 && std::is_same<T, typename op_traits<T, U>::sum_type>::value, matrix<T, H, W>>
operator + (matrix<T, H, W>&& left, const matrix_ref<U, RType>& right) {
	static_assert(H == matrix_ref<U, RType>::H && W == matrix_ref<U, RType>::W,
		"dimension mismatch in Matrix addition");
	matrix_sum<T, H, W> result(std::move(left));
	result.add(right);
	return result;
}*/

/*#####################################################################################*/
									/*MATRIX PRODUCT*/
/*#####################################################################################*/

template<typename T,unsigned h, unsigned w>
class matrix_product {
	template<typename T1, unsigned h1, unsigned w1>
	friend class matrix_sum;
	
	private:
	
	matrix<T> singleMultiplication(matrix<T>& a, matrix<T>& b) {
		int r1 = a.get_height();
		int r2 = b.get_height();
		int c1 = a.get_width();
		int c2 = b.get_width();

		///////////////////////////////////////////////////////
		assert(c1 == r2);
		///////////////////////////////////////////////////////

		matrix<T> m(r1, c2);

		#pragma omp parallel
    	{
        	int i, j, k;
			#pragma omp for
			for (i = 0; i < r1; ++i){
				for (j = 0; j < c2; ++j){
					m(i, j) = 0;
					for (k = 0; k < c1; ++k){
						m(i, j) += a(i, k) * b(k, j);
					}
				}
			}
		}		

		return m;
	}
	
	matrix<T> multiplySubSequence(std::vector<matrix_wrap<T>> A, std::vector<std::vector<int>> s, int i, int j) {
		if (i == j) {
			return A[i]; //uses matrix_wrap operator conversion to matrix
		}
		int k = s[i][j];
		std::future<matrix<T>> X = std::async([&] { return multiplySubSequence(A, s, i, k); });
		std::future<matrix<T>> Y = std::async([&] { return multiplySubSequence(A, s, k + 1, j); });
		matrix<T> r1 = X.get();
		matrix<T> r2 = Y.get();
		return singleMultiplication(r1, r2);
	}

	std::vector<int> extractDims(std::vector<matrix_wrap<T>> list) {
		int n = list.size();
		std::vector<int> res;
		res.resize(n + 1, 0);
		res[0] = list[0].get_height();
		for (int i = 1; i < n; i++) {
			res[i] = list[i].get_height();
		}
		res[n] = list[n - 1].get_width();
		return res;
	}

	matrix<T> resolveChain(std::vector<matrix_wrap<T>> list) {
		unsigned ah = list.front().get_height();
		unsigned aw = list.back().get_width();
		matrix<T> result(ah, aw);
		unsigned n = list.size();
		std::vector<std::vector<int>> m, s;
		std::vector<int> p;
		m.resize(n, std::vector<int>(n, 0));
		s.resize(n, std::vector<int>(n, 0));
		p = extractDims(list);
		for (unsigned L = 2; L < n; L++) {
			for (unsigned i = 1; i < n - L + 1; i++) {
				unsigned j = i + L - 1;
				m[i][j] = std::numeric_limits<int>::max();
				for (unsigned k = i; k <= j - 1; k++) {
					unsigned q = m[i][k] + m[k + 1][j] + p[i - 1 + 1] * p[k] * p[j];
					if (q < m[i][j]) {
						m[i][j] = q;
						s[i][j] = k;
					}
				}
			}
		}
		return multiplySubSequence(list, s, 0, n - 1);
	}

	public:
	
	static constexpr unsigned H=h;
	static constexpr unsigned W=w;

	operator matrix<T>() {
		return resolveChain(matrices);
	}
	
	template<unsigned h2, unsigned w2>
	operator matrix<T,h2,w2>() {
		return resolveChain(matrices);
	}
	
	unsigned get_height() const { return matrices.front().get_height(); }
	unsigned get_width() const { return matrices.back().get_width(); }
	
	template<typename U, class LType, class RType>
	friend matrix_product<U, matrix_ref<U,LType>::H, matrix_ref<U,RType>::W> 
	operator * (const matrix_ref<U,LType>& lhs, const matrix_ref<U,RType>& rhs);
	
	template<typename U, unsigned h2, unsigned w2, class RType>
	friend matrix_product<U,h2,matrix_ref<U,RType>::W> 
	operator * (matrix_product<U,h2,w2>&& lhs, const matrix_ref<U,RType>& rhs);
	
	template<typename U, unsigned lh, unsigned lw, unsigned rh, unsigned rw>
	friend matrix_product<U, lh, rw>
	operator * (matrix_sum<U,lh,lw> lhs, matrix_sum<U,rh,rw> rhs);

	matrix_product(matrix_product<T,h,w>&& X) = default;
	
	private:
	
	matrix_product()=default;
	
	template<unsigned w2>
	matrix_product(matrix_product<T,h,w2>&& X) : matrices(std::move(X.matrices)) {}
	
	template<class matrix_type>
	void add(matrix_ref<T,matrix_type> mat) {
		matrices.emplace_back(mat);
	}
	
	std::vector<matrix_wrap<T>> matrices;
};	
	
template<typename T, class LType, class RType>
matrix_product<T, matrix_ref<T,LType>::H, matrix_ref<T,RType>::W> 
operator * (const matrix_ref<T,LType>& lhs, const matrix_ref<T,RType>& rhs) {
	static_assert(matrix_ref<T,LType>::W*matrix_ref<T,LType>::H==0 || matrix_ref<T,LType>::W==matrix_ref<T,LType>::H,
			"dimension mismatch in Matrix multiplication");
	if (lhs.get_width()!=rhs.get_height())
		throw std::domain_error("dimension mismatch in Matrix multiplication");
	matrix_product<T, matrix_ref<T,LType>::H, matrix_ref<T,RType>::W> result;
	result.add(lhs);
	result.add(rhs);
	return result;
}

template<typename T, unsigned h, unsigned w, class RType>
matrix_product<T,h,matrix_ref<T,RType>::W> 
operator * (matrix_product<T,h,w>&& lhs, const matrix_ref<T,RType>& rhs) {
	static_assert(w*matrix_ref<T,RType>::H==0 || w==matrix_ref<T,RType>::H, 
			"dimension mismatch in Matrix multiplication");
	if (lhs.get_width()!=rhs.get_height())
		throw std::domain_error("dimension mismatch in Matrix multiplication");
	matrix_product<T,h,matrix_ref<T,RType>::W> result(std::move(lhs));
	result.add(rhs);
	return result;
}

template<typename U, unsigned lh, unsigned lw, unsigned rh, unsigned rw>
matrix_product<U, lh, rw>
operator * (matrix_sum<U,lh,lw> lhs, matrix_sum<U,rh,rw> rhs) {

	if (lhs.get_width()!=rhs.get_height())
		throw std::domain_error("dimension mismatch in Matrix multiplication");
	
	matrix_product<U, lh, rw> result;
	std::future<matrix<U>> X = std::async([&] { return (matrix<U>)lhs; });
	std::future<matrix<U>> Y = std::async([&] { return (matrix<U>)rhs; });
	matrix<U> x = X.get();
	matrix<U> y = Y.get();
	result.add(x);
	result.add(y);
	return result;
}


#endif // OPERATIONS_H 
