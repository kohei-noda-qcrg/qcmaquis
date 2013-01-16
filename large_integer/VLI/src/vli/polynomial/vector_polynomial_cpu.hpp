/*
*Very Large Integer Library, License - Version 1.0 - May 3rd, 2012
*
*Timothee Ewart - University of Geneva, 
*Andreas Hehn - Swiss Federal Institute of technology Zurich.
*
*Permission is hereby granted, free of charge, to any person or organization
*obtaining a copy of the software and accompanying documentation covered by
*this license (the "Software") to use, reproduce, display, distribute,
*execute, and transmit the Software, and to prepare derivative works of the
*Software, and to permit third-parties to whom the Software is furnished to
*do so, all subject to the following:
*
*The copyright notices in the Software and this entire statement, including
*the above license grant, this restriction and the following disclaimer,
*must be included in all copies of the Software, in whole or in part, and
*all derivative works of the Software, unless such copies or derivative
*works are solely in the form of machine-executable object code generated by
*a source language processor.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
*SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
*FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
*ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*DEALINGS IN THE SOFTWARE.
*/

#ifndef VLI_VECTOR_POLYNOME_CPU_H
#define VLI_VECTOR_POLYNOME_CPU_H
//#include "vli/config.hpp"
#include "vli/polynomial/polynomial.hpp"
#include "vli/polynomial/variable.hpp"
//#include "vli/vli.h"
#include <vector>
#include <ostream>
#include <cassert>
#include <boost/swap.hpp>
#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP

#ifdef VLI_COMPILE_GPU
#include "vli/detail/kernels_gpu.h"
#include "vli/detail/gpu/inner_product_gpu_accelerator.hpp"
#endif //VLI_COMPILE_GPU

namespace vli {


/*! \class vector
    \brief  This class models a vector for polynomial

    It helps the preparation of the inner product. As the class derived from std::vector, it inherits the properties of the vector, constructor and so on
 
*/
template<class Polynomial>
class vector : public std::vector<Polynomial> {
  public:
    /**
    \brief  constructor
    \param size, default value 1
    
    Initialiation of the vector for a given size
    */
    vector(std::size_t size = 1)
    :std::vector<Polynomial>(size) {
    }

    //copy and assignemant are done by the std vector
}; //end class

/* \cond I do not need this part in the doc*/
template <class VectorPolynomial>
struct inner_product_result_type {
};

template <class Coeff, class MaxOrder, class Var0, class Var1, class Var2, class Var3>
struct inner_product_result_type< vector<polynomial<Coeff,MaxOrder,Var0,Var1,Var2,Var3> > > {
    typedef typename polynomial_multiply_result_type<polynomial<Coeff,MaxOrder,Var0,Var1,Var2,Var3> >::type type;
};
/* \endcond */


namespace detail {    

/**
    \brief multithread inner product on CPU
    \param v1 vector of polynomial
    \param v2 vector of polynomial
 
    This function performs an inner product on the CPU using OMP multi-threading
*/
template <class Polynomial>
typename inner_product_result_type<vector<Polynomial> >::type inner_product_cpu( 
    vector<Polynomial> const& v1, 
    vector<Polynomial> const& v2){
    
    assert(v1.size() == v2.size());
    std::size_t size_v = v1.size();

    #ifdef _OPENMP
        std::vector<typename inner_product_result_type<vector<Polynomial> >::type > res(omp_get_max_threads()); 
    #else
    typename inner_product_result_type<vector<Polynomial> >::type res;
    #endif

    #pragma omp parallel for schedule(dynamic) 
    for(std::size_t i=0 ; i < size_v ; ++i){
        #ifdef _OPENMP
           res[omp_get_thread_num()] += v1[i]*v2[i];
        #else
           res += v1[i]*v2[i];
        #endif
    }

    #ifdef _OPENMP
        for(int i=1; i < omp_get_max_threads(); ++i)
            res[0]+=res[i];
    #endif 

    #ifdef _OPENMP
        return res[0];
    #else 
        return res;
    #endif
}

/**
    \brief  Safe inner product
    \param v1 vector of polynomial
    \param v2 vector of polynomial
 
    This function performs an inner product on the CPU without any threading
*/
template <class Polynomial>
typename inner_product_result_type<vector<Polynomial> >::type inner_product_plain (
      vector<Polynomial> const& v1
    , vector<Polynomial> const& v2
    ) {
    assert(v1.size() == v2.size());
    std::size_t const v_size = v1.size();
    typename inner_product_result_type<vector<Polynomial> >::type res;
    for(std::size_t i=0; i < v_size; ++i)
        res += v1[i]*v2[i];
    return res;
}

} // end namespace detail


/**
    \brief  Optimize Inner Product
    \param v1 vector of polynomial
    \param v2 vector of polynomial
 
    This function performs an optimized inner product. If the GPU mode is OFF the inner product will be executed in parrallel by OMP threading.
    If the GPU mode is activated an hybrid inner product will be executed on the GPU under hybrid setting rule.
    
    \note By default the  hybrid inner product is executed on the full GPU. A split parameter is available into the cmake setting. It is to the user
    to fix properly.
*/
template <class Polynomial>
inline typename inner_product_result_type<vector<Polynomial> >::type
inner_product(
          vector<Polynomial> const& v1
        , vector<Polynomial> const& v2
        ) {
#ifdef VLI_USE_GPU
    return detail::inner_product_gpu_helper<Polynomial>::inner_product_gpu(v1,v2); // can be pure gpu or hybride cpu omp/gpu
#else 
    return detail::inner_product_cpu(v1,v2);// can be pure serial or omp
#endif
}

/**
    \brief  Stream operator for the vector class
*/
template<class Polynomial>
std::ostream & operator<<(std::ostream & os, vector<Polynomial> const& v) {        
    for(std::size_t i = 0; i < v.size(); ++i)
        os << v[i] << std::endl;
    return os;
}

} // namespace vli

#endif //VLI_VECTOR_POLYNOME_GPU_H
