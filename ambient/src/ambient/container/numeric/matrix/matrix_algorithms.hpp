/*
 * Copyright Institute for Theoretical Physics, ETH Zurich 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef AMBIENT_CONTAINER_NUMERIC_MATRIX_ALGORITHMS
#define AMBIENT_CONTAINER_NUMERIC_MATRIX_ALGORITHMS

#include "ambient/container/numeric/matrix/matrix.h"
#include "ambient/container/numeric/matrix/detail/kernels.hpp"
#include "ambient/container/numeric/matrix/diagonal_matrix.hpp"

#define size_type       typename matrix<T,A>::size_type
#define real_type       typename matrix<T,A>::real_type
#define scalar_type     typename matrix<T,A>::scalar_type
#define difference_type typename matrix<T,A>::difference_type

namespace ambient { namespace numeric {

    template<typename T, class A>
    bool is_hermitian(const matrix<T,A>& a)
    {
        if(num_rows(a) != num_cols(a))
            return false;
        for (size_t i=0; i < num_rows(a); ++i)
            for(size_t j=0; j < num_cols(a); ++j){
               if( a(i,j) != ambient::numeric::mkl::helper_complex<T>::conj(a(j,i)) )
                   return false;
            }
        return true;
    }

    template<typename T, class A>
    inline matrix<T,A> conj(const matrix<T,A>& a){
        matrix<T,A> b(a);
        conj_inplace(b);
        return b;
    }

    template<class A>
    inline void conj_inplace(matrix<std::complex<double>, A>& a){
        kernels::template conj_inplace<std::complex<double>,A>(a);
    }

    template<class A>
    inline void conj_inplace(matrix<double, A>& a){ }

    template <typename T, class A>
    inline void touch(const matrix<T,A>& a){
        kernels::template touch<T,A>(a); 
    }

    template <typename T>
    inline void touch(const diagonal_matrix<T>& a){
        kernels::template touch<T,ambient::default_allocator<T> >(a); 
    }

    template <typename T, class A>
    inline void migrate(matrix<T,A>& a){
        kernels::template migrate<T,A>(a); 
    }

    template <typename T>
    inline void migrate(diagonal_matrix<T>& a){
        kernels::template migrate<T,ambient::default_allocator<T> >(a); 
    }

    template <typename T, class A>
    inline void hint(const matrix<T,A>& a){
        kernels::template hint<T,A>(a); 
    }

    template <typename T>
    inline void hint(const diagonal_matrix<T>& a){
        kernels::template hint<T,ambient::default_allocator<T> >(a); 
    }

    template<class MatrixViewA, class MatrixViewB, typename T, class A>
    inline void gemm_fma(const MatrixViewA& a, const MatrixViewB& b, matrix<T,A>& c){
        if(ambient::weak(a) || ambient::weak(b)) return;
        kernels::template gemm_fma<MatrixViewA,MatrixViewB,matrix<T,A>,T>(a, b, c);
    }

    template<class MatrixViewA, class MatrixViewB, typename T, class A>
    inline void gemm(const MatrixViewA& a, const MatrixViewB& b, matrix<T,A>& c){
        if(ambient::weak(a) || ambient::weak(b)) return;
        kernels::template gemm<MatrixViewA,MatrixViewB,matrix<T,A>,T>(a, b, c);
    }

    template<class MatrixViewA, typename T, typename D, class A>
    inline void gemm(const MatrixViewA& a, const diagonal_matrix<D>& b, matrix<T,A>& c){ 
        kernels::template gemm_diagonal_rhs<MatrixViewA,T,D>(a, b, c);
    }

    template<class MatrixViewB, typename T, typename D, class A>
    inline void gemm(const diagonal_matrix<D>& a, const MatrixViewB& b, matrix<T,A>& c){ 
        kernels::template gemm_diagonal_lhs<MatrixViewB,T,D>(a, b, c);
    }

    template<typename T>
    inline void gemm(const diagonal_matrix<T>& a, const diagonal_matrix<T>& b, diagonal_matrix<T>& c){
        kernels::template gemm_diagonal<T>(a, b, c);
    }

    template<typename T, class A>
    inline void scale(matrix<T,A>& a, size_t ai, size_t aj, const diagonal_matrix<T>& alfa, size_t alfai, size_t alfaj){
        kernels::template scale_offset<T>(a, ai, aj, alfa, alfai);
    }

    template<typename T, class A>
    inline void svd(matrix<T,A>& a, matrix<T,A>& u, matrix<T,A>& vt, diagonal_matrix<double>& s){
        kernels::template svd<T>(a, u, vt, s);
    }

    template<typename T, class A>
    inline void inverse(matrix<T,A>& a){
        kernels::template inverse<T>(a);
    }

    template<typename T, class A>
    inline void geev(matrix<T,A>& a, matrix<T,A>& lv, matrix<T,A>& rv, diagonal_matrix<T>& s){
        kernels::template geev<T>(a, lv, rv, s); 
    }

    template<typename T, class A>
    inline void heev(matrix<T,A>& a, matrix<T,A>& evecs, diagonal_matrix<double>& evals){
        assert(num_rows(a) == num_cols(a) && num_rows(evals) == num_rows(a));
        kernels::template heev<T>(a, evals); // destoys U triangle of M
        evecs.swap(a);
    }

    template<int IB, typename T, class A>
    inline void geqrt(matrix<T,A>& a, matrix<T,A>& t){
        kernels::template geqrt<T, int_type<IB> >(a, t);
    }

    template<PLASMA_enum TR, int IB, typename T, class A>
    inline void ormqr(size_t k, const matrix<T,A>& a, const matrix<T,A>& t, matrix<T,A>& c){
        kernels::template ormqr<T,trans_type<TR>, int_type<IB> >(k, a, t, c);
    }

    template<int IB, typename T, class A>
    inline void tsqrt(matrix<T,A>& a1, matrix<T,A>& a2, matrix<T,A>& t){
        kernels::template tsqrt<T, int_type<IB> >(a1, a2, t);
    }

    template<PLASMA_enum TR, int IB, typename T, class A>
    inline void tsmqr(size_t k, matrix<T,A>& a1, matrix<T,A>& a2, const matrix<T,A>& v, const matrix<T,A>& t){
        kernels::template tsmqr<T,trans_type<TR>, int_type<IB> >(k, a1, a2, v, t);
    }

    template<int IB, typename T, class A>
    inline void gelqt(matrix<T,A>& a, matrix<T,A>& t){
        kernels::template gelqt<T, int_type<IB> >(a, t);
    }

    template<PLASMA_enum TR, int IB, typename T, class A>
    inline void ormlq(size_t k, const matrix<T,A>& a, const matrix<T,A>& t, matrix<T,A>& c){
        kernels::template ormlq<T,trans_type<TR>, int_type<IB> >(k, a, t, c);
    }

    template<int IB, typename T, class A>
    inline void tslqt(matrix<T,A>& a1, matrix<T,A>& a2, matrix<T,A>& t){
        kernels::template tslqt<T, int_type<IB> >(a1, a2, t);
    }

    template<PLASMA_enum TR, int IB, typename T, class A>
    inline void tsmlq(size_t k, matrix<T,A>& a1, matrix<T,A>& a2, const matrix<T,A>& v, const matrix<T,A>& t){
        kernels::template tsmlq<T,trans_type<TR>, int_type<IB> >(k, a1, a2, v, t);
    }

    template<typename T, class A> 
    inline void resize(matrix<T,A>& a, size_t m, size_t n){ 
        assert(a.num_rows() != 0 && a.num_cols() != 0);
        if(a.num_rows() == m && a.num_cols() == n) return;
        matrix<T,A> resized(m, n);
        if(!ambient::weak(a))
            kernels::template resize<T,A>(resized, a, std::min(m, a.num_rows()), std::min(n, a.num_cols()));
        a.swap(resized);
    }

    template<typename T, class A> 
    inline scalar_type trace(const matrix<T,A>& a){ 
        scalar_type trace(0.);
        kernels::template trace<T,A>(a, trace);
        return trace;
    }

    template <typename T, class A>
    inline real_type norm_square(const matrix<T,A>& a){ 
        real_type norm(0.); 
        kernels::template scalar_norm<T,A>(a, norm); 
        return norm; 
    }

    template <typename T, class A>
    inline scalar_type overlap(const matrix<T,A>& a, const matrix<T,A>& b){ 
        scalar_type overlap(0.); 
        kernels::template overlap<T>(a, b, overlap); 
        return overlap; 
    }
        
    template<typename T, class A>
    inline void swap(matrix<T,A>& x, matrix<T,A>& y){ 
        x.swap(y);                     
    }

    template<typename T, class A>
    inline void transpose_inplace(matrix<T,A>& a){
        matrix<T,A> t(a.num_cols(), a.num_rows());
        kernels::template transpose_out<T,A>(a, t);
        a.swap(t);
    }

    template<typename T, class A>
    inline transpose_view<matrix<T,A> > transpose(const matrix<T,A>& a){
        return transpose_view<matrix<T,A> >(a);
    }

    template<class Matrix>
    inline void fill_identity(Matrix& a){
        kernels::template init_identity<typename Matrix::value_type>(a);
    }

    template<class Matrix>
    inline void fill_random(Matrix& a){
        kernels::template init_random<typename Matrix::value_type>(a);
        // ambient::sync(); // uncomment for reproduceability
    }

    template<class Matrix>
    inline void fill_random_hermitian(Matrix& a){
        kernels::template init_random_hermitian<typename Matrix::value_type>(a);
    }

    template<class Matrix>
    inline void fill_value(Matrix& a, typename Matrix::value_type value){
        if(value == typename Matrix::value_type()) return; // matrices are 0 by default
        kernels::template init_value<typename Matrix::value_type, typename Matrix::allocator_type>(a, value);
    }

    template <typename T, class A>
    inline void add_inplace(matrix<T,A>& lhs, const matrix<T,A>& rhs){ 
        if(ambient::weak(rhs)) return;
        else if(ambient::weak(lhs)) ambient::merge(rhs, lhs);
        else kernels::template add<T,A>(lhs, rhs); 
    }

    template <typename T, class A>
    inline void sub_inplace(matrix<T,A>& lhs, const matrix<T,A>& rhs){ 
        kernels::template sub<T>(lhs, rhs); 
    }

    template <typename T, class A>
    inline void mul_inplace(matrix<T,A>& a, const scalar_type& rhs) {
        kernels::template scale<T>(a, rhs); 
    }

    template <typename T, class A>
    inline void div_inplace(matrix<T,A>& a, const scalar_type& rhs) { 
        kernels::template scale_inverse<T>(a, rhs); 
    }

    template<typename T, class A>
    inline void copy(const matrix<T,A>& src, matrix<T,A>& dst){
        ambient::merge(src, dst);
    }

    template<typename T, class A>
    inline void copy_rt(const matrix<T,A>& src, matrix<T,A>& dst){ 
        kernels::template copy_rt<T>(src, dst);
    }

    template<typename T, class A>
    inline void copy_lt(const matrix<T,A>& src, matrix<T,A>& dst){ 
        kernels::template copy_lt<T>(src, dst);
    }

    template<class A1, class A2, typename T>
    inline void copy_block(const matrix<T,A1>& src, size_t si, size_t sj, 
                           matrix<T,A2>& dst, size_t di, size_t dj, 
                           size_t m, size_t n)
    {
        if(!ambient::weak(src) || !ambient::weak(dst))
        if(m*n == dst.num_rows()*dst.num_cols()) kernels::template copy_block_unbound<A1,A2,T>(src, si, sj, dst, di, dj, m, n); 
        else kernels::template copy_block<A1,A2,T>(src, si, sj, dst, di, dj, m, n); 
    }

    template<typename T, class A>
    inline void copy_block_s(const matrix<T,A>& src, size_t si, size_t sj, 
                             matrix<T,A>& dst, size_t di, size_t dj, 
                             const matrix<T,A>& alfa, size_t ai, size_t aj,
                             size_t m, size_t n)
    { 
        if(m*n == dst.num_rows()*dst.num_cols()) kernels::template copy_block_s_unbound<T>(src, si, sj, dst, di, dj, alfa, ai, aj, m, n);
        else kernels::template copy_block_s<T>(src, si, sj, dst, di, dj, alfa, ai, aj, m, n);
    }

    template<class A1, class A2, class A3, typename T>
    inline void copy_block_sa(const matrix<T,A1>& src, size_t si, size_t sj, 
                              matrix<T,A2>& dst, size_t di, size_t dj, 
                              const matrix<T,A3>& alfa, size_t ai, size_t aj,
                              size_t m, size_t n, T alfa_scale)
    { 
        kernels::template copy_block_sa<A1,A2,A3,T>(src, si, sj, dst, di, dj, alfa, ai, aj, m, n, alfa_scale);
    }

    template<typename T, class A>
    bool operator == (const matrix<T,A>& a, const matrix<T,A>& b){
        if(num_cols(a) != num_cols(b) || num_rows(a) != num_rows(b)){
            printf("Sizes are different: %lu x %lu against %lu x %lu\n",
                    num_cols(a), num_rows(a), num_cols(b), num_rows(b));
            return false;
        }
        ambient::future<bool> ret(true);
        kernels::template validation<T>(a, b, ret);
        return (bool)ret;
    }

    template<typename T, class A>
    bool operator == (matrix<T,A> a, const transpose_view<matrix<T,A> >& b){
        if(num_cols(a) != num_cols(b) || num_rows(a) != num_rows(b)){
            printf("Sizes are different: %lu x %lu against %lu x %lu\n",
                    num_cols(a), num_rows(a), num_cols(b), num_rows(b));
            return false;
        }
        transpose_inplace(a);
        ambient::future<bool> ret(true);
        kernels::template validation<T>(a, b, ret);
        return (bool)ret;
    }

    template<typename T>
    bool operator == (const diagonal_matrix<T>& a, const diagonal_matrix<T>& b){
        if(num_rows(a) != num_rows(b)){
            printf("Sizes are different: %lu x %lu against %lu x %lu\n",
                    num_cols(a), num_rows(a), num_cols(b), num_rows(b));
            return false;
        }
        ambient::future<bool> ret(true);
        kernels::template validation<T>(a, b, ret);
        return (bool)ret;
    }

    template <typename T, class A>
    std::ostream& operator << (std::ostream& o, const matrix<T,A>& a){
        std::cout.precision(2);
        std::cout.setf(std::ios::fixed, std::ios::floatfield);
        for(size_type i=0; i< a.num_rows(); ++i){
            for(size_type j=0; j < a.num_cols(); ++j){
                std::cout << a(i,j) << " ";
            }
            std::cout << "\n";
        }
        return o;
    }

    template <typename T, class A> 
    inline matrix<T,A> operator + (matrix<T,A> lhs, const matrix<T,A>& rhs){
        return (lhs += rhs);
    }

    template <typename T, class A> 
    inline matrix<T,A> operator - (matrix<T,A> lhs, const matrix<T,A>& rhs){
        return (lhs -= rhs);
    }

    template <typename T, class A>
    inline const matrix<T,A> operator * (matrix<T,A> lhs, const matrix<T,A>& rhs){
        return (lhs *= rhs);
    }

    template<typename T, typename T2, class A> 
    inline const matrix<T,A> operator * (matrix<T,A> lhs, const T2& rhs){
        return (lhs *= rhs);
    }

    template<typename T, typename T2, class A> 
    inline const matrix<T,A> operator * (const T2& lhs, matrix<T,A> rhs){
        return (rhs *= lhs);
    }

    template<typename T, class A> inline size_type num_rows(const matrix<T,A>& a){
        return a.num_rows();
    }

    template<typename T, class A> inline size_type num_cols(const matrix<T,A>& a){
        return a.num_cols();
    }

    template<typename T, class A> inline size_type num_rows(const transpose_view< matrix<T,A> >& a){
        return ambient::get_dim(a).x;
    }

    template<typename T, class A> inline size_type num_cols(const transpose_view< matrix<T,A> >& a){
        return ambient::get_dim(a).y;
    }

} }

#undef size_type
#undef real_type
#undef scalar_type
#undef difference_type
#endif
