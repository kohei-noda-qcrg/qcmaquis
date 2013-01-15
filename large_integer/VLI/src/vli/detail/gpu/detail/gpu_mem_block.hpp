/*
*Very Large Integer Library, License - Version 1.0 - May 3rd, 2012
*
*Timothee Ewart - University of Geneva, 
*Andreas Hehn - Swiss Federal Institute of technology Zurich.
*Maxim Milakov - NVIDIA
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
// to remove when size ok
#ifndef GPU_MEM_BLOCK_HPP
#define GPU_MEM_BLOCK_HPP

#include <boost/mpl/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include "vli/detail/gpu/detail/gpu_mem_block.h" // memory

/* \cond I do not need this part in the doc*/

namespace vli{
namespace detail {
/*
    std::size_t & gpu_memblock::BlockSize() {
        return block_size_;
    }; 
*/
    template <std::size_t NumBits, class MaxOrder, int NumVars>
    struct resize_helper{
    };
    // A free function, syntax light compared nested into the class
    // max order each specialization 
    template <std::size_t NumBits, int Order, int NumVars>
    struct resize_helper<NumBits, max_order_each<Order>, NumVars>{
        static void resize(gpu_memblock const& pgm,  std::size_t vector_size){

            std::size_t req_size_V = vector_size * num_words<NumBits>::value * stride<0,NumVars,Order>::value * stride<1,NumVars,Order>::value * stride<2,NumVars,Order>::value * stride<3,NumVars,Order>::value;
            std::size_t req_size_PoutData   = 2*num_words<NumBits>::value * result_stride<0, NumVars, Order>::value * result_stride<1, NumVars, Order>::value * result_stride<2, NumVars, Order>::value * result_stride<3, NumVars, Order>::value; 
            std::size_t req_size_VinterData = vector_size * req_size_PoutData; 

            if( req_size_V > pgm.block_size_V_ ) {
                pgm.block_size_V_  = req_size_V;
                //cudaFree does nothing is the adress passes is NULL
                gpu::cu_check_error(cudaFree((void*)pgm.V1Data_),__FILE__,__LINE__);
                gpu::cu_check_error(cudaFree((void*)pgm.V2Data_),__FILE__,__LINE__);
                gpu::cu_check_error(cudaMalloc((void**)&(pgm.V1Data_), req_size_V*sizeof(boost::uint32_t)),__FILE__,__LINE__); //input 1
                gpu::cu_check_error(cudaMalloc((void**)&(pgm.V2Data_), req_size_V*sizeof(boost::uint32_t)),__FILE__,__LINE__); //input 2
            }
          
            if( req_size_VinterData > pgm.block_size_VinterData_) {
                pgm.block_size_VinterData_ = req_size_VinterData;
                gpu::cu_check_error(cudaFree((void*)pgm.VinterData_),__FILE__,__LINE__);
                gpu::cu_check_error(cudaMalloc((void**)&(pgm.VinterData_), vector_size *  2*num_words<NumBits>::value 
                                                         * result_stride<0, NumVars, Order>::value * result_stride<1, NumVars, Order>::value * result_stride<2, NumVars, Order>::value * result_stride<3, NumVars, Order>::value * sizeof(boost::uint32_t)),__FILE__,__LINE__); 
            }
          
            if( req_size_PoutData  > pgm.block_size_PoutData_) {
                pgm.block_size_PoutData_ = req_size_PoutData;
                gpu::cu_check_error(cudaFree((void*)pgm.PoutData_),__FILE__,__LINE__);
                gpu::cu_check_error(cudaMalloc((void**)&(pgm.PoutData_),                  2*num_words<NumBits>::value 
                                                         * result_stride<0, NumVars, Order>::value * result_stride<1, NumVars, Order>::value * result_stride<2, NumVars, Order>::value * result_stride<3, NumVars, Order>::value * sizeof(boost::uint32_t)),__FILE__,__LINE__);
            }
        } // end function
    }; // end struct

    // max order combined specialization 
    template <std::size_t NumBits, int Order, int NumVars>
    struct resize_helper<NumBits, max_order_combined<Order>, NumVars>{
        static void resize(gpu_memblock const& pgm,  std::size_t vector_size){

        std::size_t req_size_V = vector_size * num_words<NumBits>::value * vli::detail::max_order_combined_helpers::size<NumVars+1, Order>::value;
        std::size_t req_size_PoutData   = 2*num_words<NumBits>::value * vli::detail::max_order_combined_helpers::size<NumVars+1, 2*Order>::value;
        std::size_t req_size_VinterData = vector_size * req_size_PoutData; 

        if( req_size_V > pgm.block_size_V_ ) {
            pgm.block_size_V_  = req_size_V;
            //cudaFree does nothing is the adress passes is NULL
            gpu::cu_check_error(cudaFree((void*)pgm.V1Data_),__FILE__,__LINE__);
            gpu::cu_check_error(cudaFree((void*)pgm.V2Data_),__FILE__,__LINE__);
            gpu::cu_check_error(cudaMalloc((void**)&(pgm.V1Data_), req_size_V*sizeof(boost::uint32_t)),__FILE__,__LINE__); //input 1
            gpu::cu_check_error(cudaMalloc((void**)&(pgm.V2Data_), req_size_V*sizeof(boost::uint32_t)),__FILE__,__LINE__); //input 2
        }

        if( req_size_VinterData  > pgm.block_size_VinterData_) {
            pgm.block_size_VinterData_ = req_size_VinterData;
            gpu::cu_check_error(cudaFree((void*)pgm.VinterData_),__FILE__,__LINE__);
            gpu::cu_check_error(cudaMalloc((void**)&(pgm.VinterData_), vector_size *  2*num_words<NumBits>::value * vli::detail::max_order_combined_helpers::size<NumVars+1, 2*Order>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__); 
        }

        if( req_size_PoutData > pgm.block_size_PoutData_) {
            gpu::cu_check_error(cudaFree((void*)pgm.PoutData_),__FILE__,__LINE__);
            gpu::cu_check_error(cudaMalloc((void**)&(pgm.PoutData_),                  2*num_words<NumBits>::value * vli::detail::max_order_combined_helpers::size<NumVars+1, 2*Order>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
        }
        } // end fonction
    }; //end struct
    
    template <std::size_t NumBits, class MaxOrder, int NumVars, typename  range = void > // last template arg for SFINAE 
    struct memory_transfer_helper;
    
    // max order each specialization 
    template <std::size_t NumBits, int Order, int NumVars>
    struct memory_transfer_helper<NumBits, max_order_each<Order>, NumVars, typename boost::enable_if_c< (full_value<NumBits, max_order_each<Order>, NumVars>::value*sizeof(unsigned int) < shared_min::value)>::type >{ //full shared mem version
         static void transfer_up(gpu_memblock const& pgm, boost::uint32_t const* pData1, boost::uint32_t const* pData2,  std::size_t VectorSize){
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V1Data_,(void*)pData1,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V2Data_,(void*)pData2,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
//for kepler data are cached into the texture memory
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_1,(void*)pgm.V1Data_,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_2,(void*)pgm.V2Data_,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
#endif
         }

         static void unbind_texture(){
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_1),__FILE__,__LINE__);
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_2),__FILE__,__LINE__);
#endif
         }
    
    };

    template <std::size_t NumBits, int Order, int NumVars>
    struct memory_transfer_helper<NumBits, max_order_each<Order>, NumVars, typename boost::enable_if_c< (full_value<NumBits, max_order_each<Order>, NumVars>::value*sizeof(unsigned int) >= shared_min::value) &&  (full_value<NumBits, max_order_each<Order> , NumVars>::value*sizeof(unsigned int) < 2*shared_min::value) >::type >{ // hybrid version texture and shared mem
         static void transfer_up(gpu_memblock const& pgm, boost::uint32_t const* pData1, boost::uint32_t const* pData2,  std::size_t VectorSize){
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V1Data_,(void*)pData1,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V2Data_,(void*)pData2,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
            //only the second poly is cashed into the texture memory
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_2,(void*)pgm.V2Data_,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_1,(void*)pgm.V1Data_,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
#endif
         }

         static void unbind_texture(){
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_2),__FILE__,__LINE__);
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_1),__FILE__,__LINE__);
#endif
         }
    };

    template <std::size_t NumBits, int Order, int NumVars>
    struct memory_transfer_helper<NumBits, max_order_each<Order>, NumVars, typename boost::enable_if_c< (full_value<NumBits, max_order_each<Order>, NumVars>::value*sizeof(unsigned int) >= 2*shared_min::value) >::type >{ // full texture memory
         static void transfer_up(gpu_memblock const& pgm, boost::uint32_t const* pData1, boost::uint32_t const* pData2,  std::size_t VectorSize){
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V1Data_,(void*)pData1,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V2Data_,(void*)pData2,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
            //two polys are cached, too large
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_1,(void*)pgm.V1Data_,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_2,(void*)pgm.V2Data_,VectorSize*stride<0,NumVars,Order>::value*stride<1,NumVars,Order>::value*stride<2,NumVars,Order>::value*stride<3,NumVars,Order>::value
                                *num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
         }

         static void unbind_texture(){
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_1),__FILE__,__LINE__);
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_2),__FILE__,__LINE__);
         }
    };

    // max order combined  specialization 
    template <std::size_t NumBits, int Order, int NumVars>
    struct memory_transfer_helper<NumBits, max_order_combined<Order>, NumVars, typename boost::enable_if_c< (full_value<NumBits, max_order_combined<Order>, NumVars>::value*sizeof(unsigned int) < shared_min::value) >::type >{ //full shared mem version
         static void transfer_up(gpu_memblock const& pgm, boost::uint32_t const* pData1, boost::uint32_t const* pData2,  std::size_t VectorSize){
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V1Data_,(void*)pData1,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V2Data_,(void*)pData2,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_1,(void*)pgm.V1Data_,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_2,(void*)pgm.V2Data_,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
#endif
         }

         static void unbind_texture(){
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_1),__FILE__,__LINE__);
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_2),__FILE__,__LINE__);
#endif
         }
    };

    template <std::size_t NumBits, int Order, int NumVars>
    struct memory_transfer_helper<NumBits, max_order_combined<Order>, NumVars, typename boost::enable_if_c< (full_value<NumBits, max_order_combined<Order>, NumVars>::value*sizeof(unsigned int) >= shared_min::value) &&  (full_value<NumBits, max_order_combined<Order> , NumVars>::value*sizeof(unsigned int) < 2*shared_min::value) >::type >{ // hybrid version texture and shared mem
         static void transfer_up(gpu_memblock const& pgm, boost::uint32_t const* pData1, boost::uint32_t const* pData2,  std::size_t VectorSize){
            //only the second poly is cashed into the texture memory
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V1Data_,(void*)pData1,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V2Data_,(void*)pData2,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);

            gpu::cu_check_error(cudaBindTexture(0,tex_reference_2,(void*)pgm.V2Data_,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_1,(void*)pgm.V1Data_,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
#endif
         }

         static void unbind_texture(){
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_2),__FILE__,__LINE__);
#ifdef VLI_KEPLER
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_1),__FILE__,__LINE__);
#endif
         }
    };

    template <std::size_t NumBits, int Order, int NumVars>
    struct memory_transfer_helper<NumBits, max_order_combined<Order>, NumVars, typename boost::enable_if_c< (full_value<NumBits, max_order_combined<Order>, NumVars>::value*sizeof(unsigned int) >= 2*shared_min::value) >::type >{ // hybrid version texture and shared mem
         static void transfer_up(gpu_memblock const& pgm, boost::uint32_t const* pData1, boost::uint32_t const* pData2,  std::size_t VectorSize){
            //only the second poly is cashed into the texture memory
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V1Data_,(void*)pData1,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);
  	    gpu::cu_check_error(cudaMemcpyAsync((void*)pgm.V2Data_,(void*)pData2,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t),cudaMemcpyHostToDevice),__FILE__,__LINE__);

            gpu::cu_check_error(cudaBindTexture(0,tex_reference_1,(void*)pgm.V1Data_,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
            gpu::cu_check_error(cudaBindTexture(0,tex_reference_2,(void*)pgm.V2Data_,VectorSize*max_order_combined_helpers::size<NumVars+1, Order>::value*num_words<NumBits>::value*sizeof(boost::uint32_t)),__FILE__,__LINE__);
         }
       
         static void unbind_texture(){
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_1),__FILE__,__LINE__);
            gpu::cu_check_error(cudaUnbindTexture(tex_reference_2),__FILE__,__LINE__);
         }
    };

    } // end namespace detail
}// end namespace vli

/* \endcond I do not need this part in the doc*/


#endif
