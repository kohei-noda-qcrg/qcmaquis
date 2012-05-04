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

#ifndef __VLI_GPU_POINTER_ARRAY_VECTOR__
#define __VLI_GPU_POINTER_ARRAY_VECTOR__

#include <iostream>
#include <boost/static_assert.hpp>
#include "vli/utils/gpu_manager.h"

namespace vli
{   
    template<class T>
    class gpu_pointer
    {
    public:
        
        gpu_pointer():data_(NULL){};
                
        T* p(){
            return data_;
        }
        
        T const* p() const{
            return data_;
        }
        
    protected: // for the swap, to change ?
        T* data_;
    };

    template<class BaseInt, std::size_t Size>
    struct gpu_array : public gpu_pointer<BaseInt>
    {
        gpu_array(){ // here, Size is template
		    gpu::cu_check_error(cudaMalloc((void**)&(this->data_), Size*sizeof(BaseInt)), __LINE__);			
	    	gpu::cu_check_error(cudaMemset((void*)(this->data_), 0, Size*sizeof(BaseInt)), __LINE__);			
        }
        
        gpu_array(gpu_array const& a)
        {
		    gpu::cu_check_error(cudaMalloc((void**)&(this->data_), Size*sizeof(BaseInt)), __LINE__);
            gpu::cu_check_error(cudaMemcpy((void*)this->data_,(void*)a.data_,Size*sizeof(BaseInt), cudaMemcpyDeviceToDevice), __LINE__);
        } 
        
        ~gpu_array(){
             gpu::cu_check_error(cudaFree(this->data_), __LINE__);   
        }

        gpu_array& operator = (gpu_array a)
        {
            
            swap(*this,a);

            return *this;
        }

        friend void swap(gpu_array& a1, gpu_array& a2)
        {
            std::swap(a1.data_,a2.data_);
        }
    };

    template<class BaseInt>
    class gpu_vector : public gpu_pointer<BaseInt>
    {
    public:
        gpu_vector(std::size_t size = 1) // Andreas discuss about this value
        :vector_size_(size){ // here, Size is an argument
            gpu::cu_check_error(cudaMalloc((void**)&(this->data_), size*sizeof(BaseInt)), __LINE__);
            gpu::cu_check_error(cudaMemset((void*)this->data_,0, size*sizeof(BaseInt)), __LINE__);   
        }
        
        gpu_vector(gpu_vector const& a)
        :vector_size_(a.vector_size_)
        {
            gpu::cu_check_error(cudaMalloc((void**)&(this->data_), vector_size_*sizeof(BaseInt)), __LINE__);
            gpu::cu_check_error(cudaMemcpy((void*)this->data_,(void*)a.data_,a.vector_size_*sizeof(BaseInt), cudaMemcpyDeviceToDevice), __LINE__);
        }
        
        ~gpu_vector(){
            gpu::cu_check_error(cudaFree(this->data_), __LINE__);
        }
        
        void vec_resize(std::size_t newsize){
            BaseInt* temp;
            gpu::cu_check_error(cudaMalloc((void**)(&temp), newsize*sizeof(BaseInt)), __LINE__);
            if(newsize > vector_size_)
            {
                gpu::cu_check_error(cudaMemcpy((void*)(temp), this->data_, vector_size_*sizeof(BaseInt),cudaMemcpyDeviceToDevice), __LINE__);
                gpu::cu_check_error(cudaMemset((void*)(temp+vector_size_),0, (newsize-vector_size_)*sizeof(BaseInt)), __LINE__);
            }
            else
            {
                gpu::cu_check_error(cudaMemcpy((void*)(temp),this->data_, newsize*sizeof(BaseInt),cudaMemcpyDeviceToDevice), __LINE__);
            }
            vector_size_ = newsize;
            std::swap(temp,this->data_);
            gpu::cu_check_error(cudaFree(temp), __LINE__);
        }
        
        gpu_vector& operator = (gpu_vector a)
        {  
            swap(*this,a);
            return *this;
        }
        
        friend void swap(gpu_vector& a1, gpu_vector& a2)
        {
            using std::swap;
            swap(a1.vector_size_,a2.vector_size_);
            swap(a1.data_,a2.data_);
        }
        
        std::size_t size() const {
            return vector_size_;
        }
 
    private:
        std::size_t vector_size_; // number of elements
    };
    
}

#endif
