/*
*Very Large Integer Library, License - Version 1.0 - May 3rd, 2012
*
*Timothee Ewart - University of Geneva, 
*Andreas Hehn - Swiss Federal Institute of technology Zurich 
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

#ifndef TIMINGS_H
#define TIMINGS_H
#include "vli/vli_config.h"
#include <cassert>

#ifdef VLI_USE_GPU
#include "cuda_runtime_api.h"
#endif
//prototype for removing warning of my compiler
unsigned long getcpuclocks();

#if defined(__i386__)
 
unsigned long getcpuclocks()
{
 unsigned long tsc;
 asm(".byte 0x0f, 0x31" : "=A" (x));
 return( tsc );
}
 
#elif (defined(__amd64__) || defined(__x86_64__))
 
unsigned long getcpuclocks()
{
 unsigned long lo, hi;
 asm( "rdtsc" : "=a" (lo), "=d" (hi) );
 return( lo | (hi << 32) );
}

#endif


class Timer
{
public:
    Timer(std::string name_)
    : val(0), name(name_), freq(CPU_FREQ),nCounter(0)
    { }
    
    ~Timer() { std::cout << name << " " << val << std::endl; }
    
    Timer & operator+=(double t)
    {
        val += t;
        return *this;
    }
    
    void begin()
    {
        t0 = getcpuclocks();
    }
    
    void end()
    {
		nCounter += 1;
        unsigned long long t1 = getcpuclocks();
        if (t1 < t0)
            assert(true);
        else
            val += (getcpuclocks()-t0)/freq; // to milliseconds
    }

    double get_time() const {
	    return  val;
    }

protected:
    double val, t0;
    std::string name;
    unsigned long long freq, nCounter;
};

#ifdef VLI_USE_GPU
class TimerCuda : public Timer
{
public:
	TimerCuda(std::string name_) : Timer(name_)
	{}
	
	~TimerCuda(){}
	
	void begin()
    {
        cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventRecord(start, 0);
    }
    
    void end()
    {
        nCounter += 1;
		cudaEventRecord(stop, 0);
		cudaEventSynchronize(stop);
		float elapsedTime;
		cudaEventElapsedTime(&elapsedTime, start, stop); // that's our time!
		cudaEventDestroy(start);
		cudaEventDestroy(stop);
		val = static_cast<double>(elapsedTime)/1000; // because time is in milisecond
    }
	
private:
	cudaEvent_t start, stop;
	
};
#endif
#ifdef _OPENMP
class TimerOMP : public Timer
{
public:
	TimerOMP(std::string name_) : Timer(name_), timer_start(0.0), timer_end(0.0){}

	~TimerOMP(){}
	
	void begin()
	{
		timer_start = omp_get_wtime(); 
	}
	
	void end()
	{
		nCounter += 1;
		timer_end = omp_get_wtime();
		val += timer_end - timer_start;
	}
	
private:
	double timer_start, timer_end;
	
};
#endif
#endif
