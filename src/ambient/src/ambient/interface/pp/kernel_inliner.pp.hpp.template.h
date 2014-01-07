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

#include <boost/preprocessor.hpp>
#define ARGS_MAX_LEN 12

#define typedef_arguments(z, n, unused)                                                                              \
    typedef T ## n t ## n;

#define cleanup_object(z, n, unused)                                                                                 \
    info<T ## n>::typed::template deallocate<n>(o);

#define ready_revision(z, n, unused)                                                                                 \
    info<T ## n>::typed::template ready<n>(o) &&

#define extract_arguments(z, n, unused)                                                                              \
    info<T ## n>::typed::template modify<n>(arg ## n, o);

#define extract_local_arguments(z, n, unused)                                                                        \
    info<T ## n>::typed::template modify_local<n>(arg ## n, o);

#define extract_remote_arguments(z, n, unused)                                                                       \
    info<T ## n>::typed::template modify_remote<n>(arg ## n);

#define traverse_arguments(z, n, unused)                                                                             \
    info<T ## n>::typed::template pin<n>(o) ||

#define score_arguments(z, n, unused)                                                                                \
    info<T ## n>::typed::template score<n>(arg ## n);

#define type_arg_list(z, n, pn)                                                                                      \
    BOOST_PP_COMMA_IF(n)                                                                                             \
    T ## n& arg ## n                                                                                                          

#define type_list(z, n, pn)                                                                                          \
    BOOST_PP_COMMA_IF(n)                                                                                             \
    T ## n&                                                                                                          

#define arg_list(z, n, pn)                                                                                           \
    BOOST_PP_COMMA_IF(n)                                                                                             \
    info<T ## n>::typed::template revised<n>(o)
                                                                                                                     
#ifndef BOOST_PP_IS_ITERATING
#ifndef AMBIENT_INTERFACE_KERNEL_INLINER_PP
#define AMBIENT_INTERFACE_KERNEL_INLINER_PP


#define BOOST_PP_ITERATION_LIMITS (1, ARGS_MAX_LEN)
#define BOOST_PP_FILENAME_1 "ambient/interface/pp/kernel_inliner.pp.hpp.template.h"
#include BOOST_PP_ITERATE()
#endif
#else
#define n BOOST_PP_ITERATION()
#define TYPES_NUMBER n

template<BOOST_PP_ENUM_PARAMS(TYPES_NUMBER, typename T) , void(*fp)( BOOST_PP_REPEAT(TYPES_NUMBER, type_list, BOOST_PP_ADD(n,1)) )>
struct kernel_inliner<void(*)( BOOST_PP_REPEAT(TYPES_NUMBER, type_list, BOOST_PP_ADD(n,1)) ), fp> {
    BOOST_PP_REPEAT(TYPES_NUMBER, typedef_arguments, ~)
    static const int arity = n; 
    static inline void latch(functor* o, BOOST_PP_REPEAT(TYPES_NUMBER, type_arg_list, n) ){
        if(controller.tunable()){
            BOOST_PP_REPEAT(TYPES_NUMBER, score_arguments, ~)
            controller.schedule();
        }
        if(controller.remote()){
            BOOST_PP_REPEAT(TYPES_NUMBER, extract_remote_arguments, ~)
            return;
        }else if(controller.local()){
            BOOST_PP_REPEAT(TYPES_NUMBER, extract_local_arguments, ~) 
        }else{
            BOOST_PP_REPEAT(TYPES_NUMBER, extract_arguments, ~) 
        }
        BOOST_PP_REPEAT(TYPES_NUMBER, traverse_arguments, ~)
        controller.queue(o);
    }
    static inline void invoke(functor* o){
        (*fp)( BOOST_PP_REPEAT(TYPES_NUMBER, arg_list, BOOST_PP_ADD(n,1)) );
    }
    static inline void cleanup(functor* o){
        BOOST_PP_REPEAT(TYPES_NUMBER, cleanup_object, ~) 
    }
    static inline bool ready(functor* o){
        return (BOOST_PP_REPEAT(TYPES_NUMBER, ready_revision, ~) true);
    }
};

#endif
