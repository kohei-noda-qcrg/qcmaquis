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

#ifndef AMBIENT_CHANNELS_MPI_PACKETS_AUX
#define AMBIENT_CHANNELS_MPI_PACKETS_AUX

namespace ambient { namespace channels { namespace mpi {

    template<typename T>
    T& get_t(){
        return packet_t::get<T>();
    }

    template<typename T>
    MPI_Datatype get_mpi_t(){
        return get_t<T>().mpi_t;
    }

    template<typename T>
    size_t sizeof_t(int field = -1){
        if(field == -1) return get_t<T>().get_size();
        else return get_t<T>().sizes[field];
    }

    template<typename T>
    void commit_t(){
        if(get_mpi_t<T>() != MPI_DATATYPE_NULL) MPI_Type_free(&get_t<T>().mpi_t);
        get_t<T>().commit();
    }

    template<typename T>
    void change_t(int field, int size){
        get_t<T>().change_field_size(field, size);
        commit_t<T>();
    }

    template<typename T>
    void* alloc_t(){
        return alloc_t(get_t<T>());
    }

    template<typename T>
    packet* pack(void* memory, ...){
        packet* instance;
        va_list fields;
        va_start(fields, memory); 
        instance = new packet(get_t<T>(), memory, fields);
        va_end(fields);
        return instance;
    }

    template<typename T>
    packet* unpack(void* memory){
        return unpack(get_t<T>(), memory);
    }

} } }

#endif
