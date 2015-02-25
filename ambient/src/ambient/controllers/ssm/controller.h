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

#ifndef AMBIENT_CONTROLLERS_SSM_CONTROLLER
#define AMBIENT_CONTROLLERS_SSM_CONTROLLER

#define AMBIENT_STACK_RESERVE 65536

namespace ambient { namespace controllers { namespace ssm {

    using ambient::models::ssm::history;
    using ambient::models::ssm::revision;
    using ambient::models::ssm::transformable;

    class controller {
    public:
        typedef models::ssm::model model_type;
        typedef channels::AMBIENT_CHANNEL_NAME::channel channel_type;
        typedef ambient::memory::private_region<AMBIENT_INSTR_BULK_CHUNK, 
                                               ambient::memory::private_factory<AMBIENT_INSTR_BULK_CHUNK> 
                                               > memory_type;
        controller();
       ~controller();
        void reserve();
        bool empty();
        void flush();
        void clear();
        bool queue (functor* f);
        bool update(revision& r);
        void sync  (revision* r);
        void lsync (revision* r);
        void rsync (revision* r);
        void lsync (transformable* v);
        void rsync (transformable* v);
        template<typename T> void collect(T* o);
        void squeeze(revision* r) const;

        void touch(const history* o);
        void use_revision(history* o);
        template<ambient::locality L, typename G>
        void add_revision(history* o, G g);

        void fence() const;
        int get_tag_ub() const;
        rank_t get_rank() const;
        rank_t get_shared_rank() const;
        int get_num_procs() const;
        channel_type & get_channel();

        void check_mem() const;
        bool verbose() const;
        bool is_serial() const;

        memory_type memory;
    public:
        model_type model;
        channel_type channel;
        std::vector< functor* > stack_m;
        std::vector< functor* > stack_s;
        std::vector< functor* >* chains;
        std::vector< functor* >* mirror;
        ambient::memory::collector garbage;
    };
    
} } }

#endif
