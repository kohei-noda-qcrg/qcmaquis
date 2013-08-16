/*****************************************************************************
 *
 * MAQUIS DMRG Project
 *
 * Copyright (C) 2011-2011 by Bela Bauer <bauerb@phys.ethz.ch>
 *               2011-2012 by Michele Dolfi <dolfim@phys.ethz.ch>
 *
 *****************************************************************************/

#ifndef STORAGE_ARCHIVE_H
#define STORAGE_ARCHIVE_H

#ifdef USE_AMBIENT
#include "ambient/ambient.hpp"
#endif

#include <alps/hdf5.hpp>
#include <alps/utility/encode.hpp>

namespace storage {

    inline std::string once(std::string fp){
        #ifdef USE_AMBIENT
        if(!ambient::master()) return fp+"."+std::to_string(ambient::rank());
        #endif
        return fp;
    }

    inline void uniq(std::string fp){
        #ifdef USE_AMBIENT
        if(!ambient::master()) std::remove(once(fp).c_str());
        #endif
    }

    class archive {
    public:
        archive(std::string fp) : write(false), fp(fp) {
            impl = new alps::hdf5::archive(fp);
        }
        archive(std::string fp, const char* rights) : write(true), fp(fp) {
            impl = new alps::hdf5::archive(once(fp), rights); 
        }
       ~archive(){
           delete impl;
           if(write) uniq(fp); 
        }
        bool is_group(const char* path){
            return impl->is_group(path);
        }
        bool is_scalar(const char* path){
            return impl->is_scalar(path);
        }
        bool is_data(const char* path){
            return impl->is_data(path);
        }
        template<typename T>
        void operator << (const T& obj){
            (*impl) << obj;
        }
        template<typename T>
        void operator >> (T& obj){
            (*impl) >> obj;
        }
        alps::hdf5::detail::archive_proxy<alps::hdf5::archive> operator[](std::string path){
            return (*impl)[path];
        }
    private:
        std::string fp;
        alps::hdf5::archive* impl;
        bool write;
    };
    
    inline std::string encode(std::string const & s){
        return alps::hdf5_name_encode(s);
    }
}

#endif
