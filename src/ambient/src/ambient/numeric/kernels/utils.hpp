/*
 * Ambient, License - Version 1.0 - May 3rd, 2012
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

#ifndef AMBIENT_NUMERIC_MATRIX_KERNELS_UTILS
#define AMBIENT_NUMERIC_MATRIX_KERNELS_UTILS

#include <cstring>

namespace ambient {

    template <typename T>
    inline void memcpy(T* dd, T *sd, size_t w, T alfa){
        std::memcpy(dd, sd, w);
    }

    template <typename T>
    inline void memscal(T* dd, T *sd, size_t w, T alfa){
        int z = w/sizeof(T);
        do{ *dd++ = alfa*(*sd++); }while(--z > 0); // note: dd != sd
    }

    template <typename T>
    inline void memscala(T* dd, T *sd, size_t w, T alfa){
        int z = w/sizeof(T);
        do{ *dd++ += alfa*(*sd++); }while(--z > 0); // note: dd != sd
    }

    template <typename T>
    inline typename std::complex<T>::value_type dot(std::complex<T>* a, std::complex<T>* b, int size){
        typename std::complex<T>::value_type summ(0);
        for(size_t k=0; k < size; k++)
           summ += std::real(a[k]*std::conj(b[k]));
        return summ;
    }

    inline double dot(double* a, double* b, int size){
        static const int ONE = 1;
        return ddot_(&size, a, &ONE, b, &ONE);
    }

    template<typename T, void(*PTF)(T* dd, T* sd, size_t w, T alfa)>
    inline void memptf(T* dst, int ldb, dim2 dst_p, 
                       T* src, int lda, dim2 src_p, 
                       dim2 size, T alfa = 0.0)
    {
        #ifdef AMBIENT_CHECK_BOUNDARIES
        if(ambient::dim(dst).x - dst_p.x < size.x || ambient::dim(dst).y - dst_p.y < size.y ||
           ambient::dim(src).x - src_p.x < size.x || ambient::dim(src).y - src_p.y < size.y){
            ambient::cout << "Error: invalid memory movement: \n";
            ambient::cout << "Matrix dst " << ambient::dim(dst).x << "x" << ambient::dim(dst).y << "\n";
            ambient::cout << "Dest p " << dst_p.x << "x" << dst_p.y << "\n";
            ambient::cout << "Matrix src " << ambient::dim(src).x << "x" << ambient::dim(src).y << "\n";
            ambient::cout << "Src p " << src_p.x << "x" << src_p.y << "\n";
            ambient::cout << "Block size " << size.x << "x" << size.y << "\n";
            AMBIENT_TRACE
        }
        #endif
        int n = size.x;
        int m = size.y*sizeof(T);
        T* sd = src + src_p.y + src_p.x*lda;
        T* dd = dst + dst_p.y + dst_p.x*ldb;
        do{ PTF(dd, sd, m, alfa); sd += lda; dd += ldb; }while(--n > 0);
    }
}

#endif
