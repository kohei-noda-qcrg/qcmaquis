/*
 * Ambient Project
 *
 * Copyright (C) 2014 Institute for Theoretical Physics, ETH Zurich
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

#ifndef AMBIENT_MEMORY_DATA_BULK_HPP
#define AMBIENT_MEMORY_DATA_BULK_HPP

namespace ambient { namespace memory {

    inline data_bulk& data_bulk::instance(){
        static data_bulk singleton; return singleton;
    }

    inline data_bulk::data_bulk(){
        this->limit = ambient::isset("AMBIENT_BULK_LIMIT") ? ambient::getint("AMBIENT_BULK_LIMIT") : AMBIENT_MAX_SID;
        this->reuse_enabled = ambient::isset("AMBIENT_BULK_REUSE") ? true : false; 
        this->reset_enabled = ambient::isset("AMBIENT_BULK_DEALLOCATE") ? true : false; 
    }

    template<size_t S> void* data_bulk::malloc()         { return instance().memory.malloc(S);     }
                inline void* data_bulk::malloc(size_t s) { return instance().memory.malloc(s);     }
    template<size_t S> void* data_bulk::calloc()         { void* m = malloc<S>(); memset(m, 0, S); return m; }
                inline void* data_bulk::calloc(size_t s) { void* m = malloc(s);   memset(m, 0, s); return m; }

    inline void data_bulk::reuse(void* ptr){
        if(instance().reuse_enabled) factory<AMBIENT_DATA_BULK_CHUNK>::reuse(ptr); 
    }

    inline void data_bulk::drop(){
        instance().memory.reset();
        if(instance().reset_enabled) factory<AMBIENT_DATA_BULK_CHUNK>::deallocate();
        factory<AMBIENT_DATA_BULK_CHUNK>::reset();
    }

    inline region_t data_bulk::signature(){
        return region_t::bulk;
    }

    inline bool data_bulk::open(){
        return instance().limit > factory<AMBIENT_DATA_BULK_CHUNK>::size();
    }

} }

#endif

