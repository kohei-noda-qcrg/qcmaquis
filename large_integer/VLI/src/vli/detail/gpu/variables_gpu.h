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

 #ifndef VLI_VARIABLES_GPU_H
 #define VLI_VARIABLES_GPU_H

namespace vli {
    template <char X>
    class var;
   
    class no_variable; 

    namespace detail {

    template <class Variable, unsigned int Order>
    struct extend_stride {
        static unsigned int const value = 2*Order+1;
    };  
        
    template <unsigned int Order>
    struct extend_stride<no_variable,Order> {
        static unsigned int const value = 1;
    };  

    template <class Variable, unsigned int Order>
    struct stride {
        static unsigned int const value = Order+1;
    };  
        
    template <unsigned int Order>
    struct stride<no_variable,Order> {
        static unsigned int const value = 1;
    };  
   
    template <class Variable, unsigned int Order>
    struct stride_pad {
        static unsigned int const value = Order+1;
    };  
        
    template <unsigned int Order>
    struct stride_pad<no_variable,Order> {
        static unsigned int const value = 0;
    };  

    template <class Variable>
    inline unsigned int extend_stridef(unsigned int  Order){
        return 2*Order+1;
    };  

    template <>
    inline unsigned int extend_stridef<no_variable>(unsigned int  Order){
        return 1;
    };  

    template <class Variable>
    inline unsigned int stridef(unsigned int  Order){
        return Order+1;
    };  

    template <>
    inline unsigned int stridef<no_variable>(unsigned int  Order){
        return 1;
    };  
   
    struct SumBlockSize {
       enum { value = 256};
    };

    template<unsigned int Order, class Var0, class Var1, class Var2, class Var3>
    struct MulBlockSize{
        enum {value = ((extend_stride<Var0,Order>::value*extend_stride<Var1,Order>::value*extend_stride<Var2,Order>::value*extend_stride<Var3,Order>::value)/2U >= 256U) ? 
                       256U :
                       (((extend_stride<Var0,Order>::value*extend_stride<Var1,Order>::value*extend_stride<Var2,Order>::value*extend_stride<Var3,Order>::value))/2U+32U-1U)/32U*32U };
    };

    template<unsigned int Order, class Var0, class Var1, class Var2, class Var3>
    struct MaxIterationCount{
        enum {value = (extend_stride<Var0,Order>::value*extend_stride<Var1,Order>::value*extend_stride<Var2,Order>::value*extend_stride<Var3,Order>::value+32U-1U)/32U};
    };

    template<std::size_t Size>
    struct size_pad{
        enum {value = (((Size>>1)<<1)+1)};
    };

    }
}

#endif
