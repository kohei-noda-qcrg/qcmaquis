#ifndef AMBIENT_INTERFACE_H
#define AMBIENT_INTERFACE_H
#include "ambient/ambient.h"

namespace blas{ 
//  forward declarations if handy
    template <typename T> 
    class p_dense_matrix; 
}

namespace ambient{ 

    template <typename FL, typename FC, class T0, class T1, class T2>
    void push(FL l_kernel, FC c_kernel, T0& arg0, T1& arg1, T2& arg2);

    template <typename ST, typename FL, typename FC, class T0, class T1>
    ST push(FL l_kernel, FC c_kernel, T0& arg0, T1& arg1);

    template <typename L, typename R>
    void pin(L& proxy_object, const R& real_object);

    template<typename T>
    void assign(T& ref, int i, int j = 0, int k = 0);

    template<typename T>
    dim3 get_grid_dim(T& ref);

    template<typename T>
    dim3 get_group_dim(T& ref);

    template<typename T>
    dim3 get_item_dim(T& ref);

    template<typename T>
    dim3 get_group_id(T& ref);

    #include "ambient/interface/core.hpp"
    #include "ambient/interface/kernels.hpp"
    #include "ambient/interface/breakdown.hpp"
    #include "ambient/interface/interface.hpp"
}
#endif
