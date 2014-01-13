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

#ifndef AMBIENT
#define AMBIENT
// {{{ system includes
#include <mpi.h>
#include <complex>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <limits>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <memory.h>
#include <stdarg.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <algorithm>
#include <execinfo.h>
// }}}

#define AMBIENT_MEMORY_SQUEEZE
#define AMBIENT_IB                    2048
#define AMBIENT_INSTR_BULK_CHUNK      16777216 // 16 MB
#define AMBIENT_DATA_BULK_CHUNK       67108864 // 64 MB
#define AMBIENT_MAX_SID               2097152  // Cray MPI

#include "ambient/utils/dim2.h"
#include "ambient/utils/enums.h"
#include "ambient/utils/tree.hpp"
#include "ambient/utils/fence.hpp"
#include "ambient/utils/singleton.hpp"
#include "ambient/utils/enable_threading.hpp"

#include "ambient/memory/pool.hpp"
#include "ambient/memory/new.h"
#include "ambient/memory/allocator.h"

#include "ambient/models/ssm/revision.h"
#include "ambient/models/ssm/history.h"
#include "ambient/models/ssm/transformable.h"
#include "ambient/models/ssm/model.h"

#include "ambient/channels/mpi/group.h"
#include "ambient/channels/mpi/multirank.h"
#include "ambient/channels/mpi/channel.h"
#include "ambient/channels/mpi/request.h"
#include "ambient/channels/mpi/collective.h"

#include "ambient/controllers/ssm/functor.h"
#include "ambient/controllers/ssm/get.h"
#include "ambient/controllers/ssm/set.h"
#include "ambient/controllers/ssm/collector.h"
#include "ambient/controllers/ssm/controller.h"

#include "ambient/utils/auxiliary.hpp"
#include "ambient/utils/io.hpp"

#include "ambient/memory/new.hpp"
#include "ambient/memory/allocator.hpp"
#include "ambient/memory/data_bulk.hpp"
#include "ambient/memory/instr_bulk.hpp"

#include "ambient/models/ssm/revision.hpp"
#include "ambient/models/ssm/history.hpp"
#include "ambient/models/ssm/transformable.hpp"
#include "ambient/models/ssm/model.hpp"

#include "ambient/channels/mpi/group.hpp"
#include "ambient/channels/mpi/multirank.hpp"
#include "ambient/channels/mpi/channel.hpp"
#include "ambient/channels/mpi/request.hpp"
#include "ambient/channels/mpi/collective.hpp"

#include "ambient/controllers/ssm/get.hpp"
#include "ambient/controllers/ssm/set.hpp"
#include "ambient/controllers/ssm/scope.hpp"
#include "ambient/controllers/ssm/collector.hpp"
#include "ambient/controllers/ssm/controller.hpp"

#include "ambient/interface/typed.hpp"
#include "ambient/interface/kernel.hpp"
#include "ambient/interface/access.hpp"

#ifdef AMBIENT_BUILD_LIBRARY

namespace ambient {

    void* fence::nptr = NULL;
    int scope<single>::grain = 1;
    std::vector<int> scope<single>::permutation;
    mkl_parallel::fptr_t mkl_parallel::fptr = NULL;

    class universe {
    public:
        typedef controllers::ssm::controller controller_type;
        typedef channels::mpi::channel channel_type;

       ~universe(){
            channel_type::unmount();
        }
        universe(){
            int db = ambient::isset("AMBIENT_DB_NUM_PROCS") ? ambient::getint("AMBIENT_DB_NUM_PROCS") : 0;
            channel_type::mount();
            c.get_channel().init();
            c.init(db);
            if(ambient::isset("AMBIENT_VERBOSE")){
                ambient::cout << "ambient: initialized ("                   << AMBIENT_THREADING_TAGLINE     << ")\n";
                if(ambient::isset("AMBIENT_MKL_NUM_THREADS")) ambient::cout << "ambient: selective threading (mkl)\n";
                ambient::cout << "ambient: size of instr bulk chunks: "     << AMBIENT_INSTR_BULK_CHUNK       << "\n";
                ambient::cout << "ambient: size of data bulk chunks: "      << AMBIENT_DATA_BULK_CHUNK        << "\n";
                if(ambient::isset("AMBIENT_BULK_LIMIT")) ambient::cout << "ambient: max chunks of data bulk: " << ambient::getint("AMBIENT_BULK_LIMIT") << "\n";
                ambient::cout << "ambient: maximum sid value: "             << AMBIENT_MAX_SID                << "\n";
                if(db) ambient::cout << "ambient: number of db procs: "     << ambient::num_db_procs()        << "\n";
                ambient::cout << "ambient: number of work procs: "          << ambient::num_workers()         << "\n";
                ambient::cout << "ambient: number of threads per proc: "    << ambient::num_threads()         << "\n";
                ambient::cout << "\n";
            }
            if(ambient::isset("AMBIENT_MKL_NUM_THREADS")) mkl_parallel();
        }
        controller_type& operator()(size_t n){
            return c;
        }
        void sync(){
            c.flush();
            c.clear();  
            memory::data_bulk::drop();
            memory::instr_bulk::drop();
        }
    private:
        controller_type c;
    } u;

    controllers::ssm::controller& get_controller(){ return u(AMBIENT_THREAD_ID); }
    void sync(){ u.sync(); }
    utils::mpostream cout;
    utils::mpostream cerr;

}

#endif
#endif
