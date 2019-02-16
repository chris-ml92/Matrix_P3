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

template<typename T, class LType, typename U, class RType>
std::enable_if_t<matrix_ref<T,LType>::H*matrix_ref<U,RType>::H==0, matrix<typename op_traits<T,U>::sum_type>> 
operator + (const matrix_ref<T,LType>& left, const matrix_ref<U,RType>& right) {
	if (left.get_height()!=right.get_height() || left.get_width()!=right.get_width())
		throw std::domain_error("dimension mismatch in Matrix addition");
	
	const unsigned height=left.get_height();
	const unsigned width=left.get_width();
	matrix<typename op_traits<T,U>::sum_type> result(height, width);

	for (unsigned i=0; i!=height; ++i)
		for (unsigned j=0; j!=width; j++)
			result(i,j) = left(i,j) + right(i,j);
	
	return result;
}

template<typename T, typename U, class RType>
std::enable_if_t<std::is_same<T,typename op_traits<T,U>::sum_type>::value, matrix<T>>
operator + (matrix<T>&& left, const matrix_ref<U,RType>& right) {
	if (left.get_height()!=right.get_height() || left.get_width()!=right.get_width())
		throw std::domain_error("dimension mismatch in Matrix addition");
	
	const unsigned height=left.get_height();
	const unsigned width=left.get_width();
	for (unsigned i=0; i!=height; ++i)
		for (unsigned j=0; j!=width; j++)
			left(i,j) += right(i,j);
	
	return std::move(left);
}
template<typename T, class LType, typename U, class RType>
std::enable_if_t<matrix_ref<T,LType>::H*matrix_ref<U,RType>::H!=0, matrix<typename op_traits<T,U>::sum_type, matrix_ref<T,LType>::H, matrix_ref<T,LType>::W>> 
operator + (const matrix_ref<T,LType>& left, const matrix_ref<U,RType>& right) {
	static_assert(matrix_ref<T,LType>::H==matrix_ref<U,RType>::H && matrix_ref<T,LType>::W==matrix_ref<U,RType>::W, 
			"dimension mismatch in Matrix addition");
	
	const unsigned height=left.get_height();
	const unsigned width=left.get_width();
	matrix<typename op_traits<T,U>::sum_type, matrix_ref<T,LType>::H, matrix_ref<T,LType>::W> result;
	for (unsigned i=0; i!=height; ++i)
		for (unsigned j=0; j!=width; j++)
			result(i,j) = left(i,j) + right(i,j);
	
	return result;
}

template<typename T, unsigned H, unsigned W, typename U, class RType>
std::enable_if_t<matrix_ref<U,RType>::H!=0 && std::is_same<T,typename op_traits<T,U>::sum_type>::value, matrix<T, H, W>> 
operator + (matrix<T,H,W>&& left, const matrix_ref<U,RType>& right) {
	static_assert(H==matrix_ref<U,RType>::H && W==matrix_ref<U,RType>::W, 
			"dimension mismatch in Matrix addition");

	for (unsigned i=0; i!=H; ++i)
		for (unsigned j=0; j!=W; j++)
			left(i,j) += right(i,j);
	
	return std::move(left);
}




template<typename T,unsigned h, unsigned w>
class matrix_product {
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
		for (int i = 0; i < r1; ++i)
			for (int j = 0; j < c2; ++j)
				for (int k = 0; k < c1; ++k)
				{
					m(i, j) = m(i, j) + a(i, k) * b(k, j);
				}
		return m;
	}
	matrix<T> multiplySubSequence(std::vector<matrix_wrap<T>> A, std::vector<std::vector<int>> s, int i, int j) {
		if (i == j) {
			return A[i]; //uses matrix_wrap operator conversion to matrix
		}
		int k = s[i][j];
		matrix<T> X = multiplySubSequence(A, s, i, k);
		matrix<T> Y = multiplySubSequence(A, s, k + 1, j);
		return singleMultiplication(X, Y);
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


	// Added OpenMP
	operator matrix<T>() {
		return resolveChain(matrices);
	}
	
	// Added OpenMP
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
	
	
	matrix_product(matrix_product<T,h,w>&& X) = default;
	
	private:
	
	matrix_product()=default;
	
	template<unsigned w2>
	matrix_product(matrix_product<T,h,w2>&& X) : matrices(std::move(X.matrices)), sizes(std::move(X.sizes)) {}
	
	template<class matrix_type>
	void add(matrix_ref<T,matrix_type> mat) {
		matrices.emplace_back(mat);
		sizes.push_back(mat.get_width());
	}
	
	// Modify RESOLVE to perform asyncronously
	std::vector<matrix_wrap<T>> matrices;
	std::vector<unsigned> sizes;
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
	
#endif // OPERATIONS_H 
