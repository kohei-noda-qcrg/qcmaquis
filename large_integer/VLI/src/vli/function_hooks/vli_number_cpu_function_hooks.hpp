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

#ifndef VLI_NUMBER_CPU_FUNCTION_HOOKS_HPP
#define VLI_NUMBER_CPU_FUNCTION_HOOKS_HPP

#include "vli/detail/kernels_cpu.h"

namespace vli 
{
    //forwart declaration
    template <class BaseInt, std::size_t Size>
    class vli_cpu;
    
    //declaration wrapper
    //addition
    template<typename BaseInt, std::size_t Size>
    void add(BaseInt * x, BaseInt const b);
    
    template<typename BaseInt, std::size_t Size>
    void add(BaseInt * x, BaseInt const* y); 

    template<typename BaseInt, std::size_t Size>
    void add_extension(BaseInt * x, BaseInt const* y, BaseInt const* z); 

    //substraction
    template<typename BaseInt, std::size_t Size>
    void sub(BaseInt * x, BaseInt const b);
    
    template<typename BaseInt, std::size_t Size>
    void sub(BaseInt * x, BaseInt const* y); 

    //multiplication
    template<typename BaseInt, std::size_t Size>
    void mul(BaseInt * x,BaseInt const b);

    template<typename BaseInt, std::size_t Size>
    void mul(BaseInt * x,BaseInt const* y);

    template<typename BaseInt, std::size_t Size>
    void mul(BaseInt * x,BaseInt const* y,BaseInt const* z);

    //multiplication Addition (only for the inner product)
    template<typename BaseInt, std::size_t Size>
    void muladd(BaseInt * x,BaseInt const* y,BaseInt const* z);

    //????_assign functions
    template <class BaseInt, std::size_t Size>
    void plus_assign(vli_cpu<BaseInt,Size> & vli_a, vli_cpu<BaseInt,Size> const& vli_b ){
        add<BaseInt,Size>(&vli_a[0],&vli_b[0]);
    }
     
    template <class BaseInt, std::size_t Size>
    void plus_assign(vli_cpu<BaseInt,Size> & vli_a, BaseInt const b ){  
        add<BaseInt,Size>(&vli_a[0],b);
    }
    
    template <class BaseInt, std::size_t Size>
    void plus_extend_assign(vli_cpu<BaseInt,Size+1> & vli_a, vli_cpu<BaseInt,Size> const& vli_b, vli_cpu<BaseInt,Size> const& vli_c){
        add_extension<BaseInt, Size>(&vli_a[0],&vli_b[0],&vli_c[0]);
    }

    template <class BaseInt, std::size_t Size>
    void minus_assign(vli_cpu<BaseInt,Size> & vli_a, vli_cpu<BaseInt,Size> const& vli_b ){
        sub<BaseInt,Size>(&vli_a[0],&vli_b[0]);
    }
    
    template <class BaseInt, std::size_t Size>
    void minus_assign(vli_cpu<BaseInt,Size> & vli_a, BaseInt const b ){  
        sub<BaseInt,Size>(&vli_a[0],b);
    }

    template <class BaseInt, std::size_t Size>
    void multiplies_assign( vli_cpu<BaseInt, Size>& vli_a , vli_cpu<BaseInt,Size> const & vli_b){ 
        mul<BaseInt,Size>(&vli_a[0],&vli_b[0]);
    }

    template <class BaseInt, std::size_t Size>
    void multiplies_assign(vli_cpu<BaseInt,Size> & vli_a, BaseInt const b){
        mul<BaseInt,Size>(&vli_a[0],b);
    }

    template <class BaseInt, std::size_t Size>
    void multiplies(vli_cpu<BaseInt, 2*Size>& vli_res , vli_cpu<BaseInt,Size> const & vli_a, vli_cpu<BaseInt,Size> const & vli_b){
        mul<BaseInt,Size>(&vli_res[0],&vli_a[0],&vli_b[0]);
    }
    
    template <class BaseInt, std::size_t Size>
    void multiply_add_assign(vli_cpu<BaseInt, 2*Size>& vli_res , vli_cpu<BaseInt,Size> const & vli_a, vli_cpu<BaseInt,Size> const & vli_b){
        muladd<BaseInt,Size>(&vli_res[0],&vli_a[0],&vli_b[0]);
    }

    /* ---------------------------------------------------- Begin Addition specialization ---------------------------------------------------- */

    //specialization addnbits_nbits, until 512 bits
    #define FUNCTION_add_nbits_nbits(z, n, unused) \
        template<> \
        void add<unsigned long int,BOOST_PP_ADD(n,2)>(unsigned long int* x,unsigned long int const* y){ \
        detail::NAME_ADD_NBITS_PLUS_NBITS(n)(x,y); \
        }; \

    BOOST_PP_REPEAT(MAX_ITERATION, FUNCTION_add_nbits_nbits, ~)
    #undef FUNCTION_add_nbits_nbits

    //specialization addnbits_64bits, until 512 bits
    #define FUNCTION_add_nbits_64bits(z, n, unused) \
        template<> \
        void add<unsigned long int,BOOST_PP_ADD(n,2)>(unsigned long int* x,unsigned long int const y){ \
        detail::NAME_ADD_NBITS_PLUS_64BITS(n)(x,&y); \
        }; \

    BOOST_PP_REPEAT(MAX_ITERATION, FUNCTION_add_nbits_64bits, ~)
    #undef FUNCTION_add_nbits_64bits
    // specialization extention addition 
    #define FUNCTION_add_nbits_nminus1bits(z, n, unused) \
        template<> \
        void add_extension<unsigned long int,BOOST_PP_ADD(n,2)>(unsigned long int* x,unsigned long int const* y, unsigned long int const* w){ \
        detail::NAME_ADD_NBITS_PLUS_NMINUS1BITS(n)(x,y,w); \
        }; \

    BOOST_PP_REPEAT(MAX_ITERATION_MINUS_ONE, FUNCTION_add_nbits_nminus1bits, ~)
    #undef FUNCTION_add_nbits_mninus1bits
    /* ---------------------------------------------------- End Addition specialization ---------------------------------------------------- */

    /* ---------------------------------------------------- Begin Substraction specialization ---------------------------------------------------- */
    //specialization subnbits_nbits, until 512 bits
    #define FUNCTION_sub_nbits_nbits(z, n, unused) \
        template<> \
        void sub<unsigned long int,BOOST_PP_ADD(n,2)>(unsigned long int* x,unsigned long int const* y){ \
        detail::NAME_SUB_NBITS_MINUS_NBITS(n)(x,y); \
        }; \

    BOOST_PP_REPEAT(MAX_ITERATION, FUNCTION_sub_nbits_nbits, ~)
    #undef FUNCTION_sub_nbits_nbits
    //specialization subnbits_64bits, until 512 bits
    #define FUNCTION_sub_nbits_64bits(z, n, unused) \
        template<> \
        void sub<unsigned long int,BOOST_PP_ADD(n,2)>(unsigned long int* x,unsigned long int const y){ \
        detail::NAME_SUB_NBITS_MINUS_64BITS(n)(x,&y); \
        }; \

    BOOST_PP_REPEAT(MAX_ITERATION, FUNCTION_sub_nbits_64bits, ~)
    #undef FUNCTION_sub_nbits_64bits

    /* ---------------------------------------------------- end Substraction specialization ---------------------------------------------------- */

    /* ---------------------------------------------------- Begin Multiplication specialization ---------------------------------------------------- */
    //specialization mul    
    //specialization mulnbits_64bits, until 512 bits
    #define FUNCTION_mul_nbits_64bits(z, n, unused) \
        template<> \
        void mul<unsigned long int,BOOST_PP_ADD(n,2)>(unsigned long int* x,unsigned long int const y){ \
        detail::NAME_MUL_NBITS_64BITS(n)(x,&y); \
        }; \

    BOOST_PP_REPEAT(MAX_ITERATION, FUNCTION_mul_nbits_64bits, ~)
    #undef FUNCTION_mul_nbits_64bits
    //specialization mulnbits_nbits, until 512 bits
    #define FUNCTION_mul_nbits_nbits(z, n, unused) \
        template<> \
        void mul<unsigned long int,BOOST_PP_ADD(n,2)>(unsigned long int* x,unsigned long int const* y){ \
        detail::NAME_MUL_NBITS_NBITS(n)(x,y); \
        }; \

    BOOST_PP_REPEAT(MAX_ITERATION, FUNCTION_mul_nbits_nbits, ~)
    #undef FUNCTION_mul_nbits_nbits
    //specialization mul2nbits_nbits_nbits, until 512 bits
    #define FUNCTION_mul_twonbits_nbits_nbits(z, n, unused) \
        template<> \
        void mul<unsigned long int,BOOST_PP_ADD(n,1)>(unsigned long int* x,unsigned long int const* y, unsigned long int const* w){ \
        detail::NAME_MUL_TWONBITS_NBITS_NBITS(n)(x,y,w); \
        }; \

    BOOST_PP_REPEAT(FOUR, FUNCTION_mul_twonbits_nbits_nbits, ~)
    #undef FUNCTION_mul_twonbits_nbits_nbits
    /* ---------------------------------------------------- end Multiplicatio specialization ---------------------------------------------------- */
    /* ---------------------------------------------------- Begin Multiplication Addition specialization ---------------------------------------------------- */
    //specialization muladd2nbits_nbits_nbits, until 512 bits
    #define FUNCTION_muladd_twonbits_nbits_nbits(z, n, unused) \
        template<> \
        void muladd<unsigned long int,BOOST_PP_ADD(n,1)>(unsigned long int* x,unsigned long int const* y, unsigned long int const* w){ \
        detail::NAME_MULADD_TWONBITS_NBITS_NBITS(n)(x,y,w); \
        }; \

    BOOST_PP_REPEAT(FOUR, FUNCTION_muladd_twonbits_nbits_nbits, ~)
    #undef FUNCTION_muladd_twonbits_nbits_nbits

    /* ---------------------------------------------------- End Multiplication Addition specialization ---------------------------------------------------- */
    
} //namespace vli

#endif //VLI_NUMBER_CPU_FUNCTION_HOOKS_HPP
