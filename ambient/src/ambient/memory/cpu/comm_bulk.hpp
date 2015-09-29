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

#ifndef AMBIENT_MEMORY_CPU_COMM_BULK_HPP
#define AMBIENT_MEMORY_CPU_COMM_BULK_HPP

#include "utils/rss.hpp"
#define DEFAULT_LIMIT 20

namespace ambient { namespace memory { namespace cpu {

    inline comm_bulk& comm_bulk::instance(){
        static comm_bulk singleton; return singleton;
    }

    inline comm_bulk::comm_bulk(){
        this->soft_limit = (ambient::isset("AMBIENT_COMM_BULK_LIMIT") ? ambient::getint("AMBIENT_COMM_BULK_LIMIT") : DEFAULT_LIMIT) * 
                           ((double)getRSSLimit() / AMBIENT_COMM_BULK_CHUNK / 100);
    }

    template<size_t S> void* comm_bulk::malloc()         { return instance().memory.malloc(S); }
                inline void* comm_bulk::malloc(size_t s) { return instance().memory.malloc(s); }

    inline void comm_bulk::drop(){
        instance().memory.reset();
        if(instance().soft_limit < factory<AMBIENT_COMM_BULK_CHUNK>::size())
            factory<AMBIENT_COMM_BULK_CHUNK>::deallocate();
        factory<AMBIENT_COMM_BULK_CHUNK>::reset();
    }

} } }

#undef DEFAULT_LIMIT
#endif
