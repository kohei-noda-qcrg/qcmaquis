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

#ifndef AMBIENT_INTERFACE_SCOPE_HPP
#define AMBIENT_INTERFACE_SCOPE_HPP

namespace ambient { 

        inline int scope::balance(int k, int max_k){
            if(max_k > ambient::num_workers()){
                int k_ = k/((int)(max_k / ambient::num_workers()));
                if(k_ < ambient::num_workers()) return k_;
            }
            return k;
        }
        inline int scope::permute(int k, const std::vector<int>& s){
            if(k >= s.size()){ printf("Error: permutation overflow!\n"); return k; }
            return s[k];
        }
        inline scope::~scope(){
            if(!dry) ctxt.pop();
        }
        inline scope::scope(scope_t t) : type(t){
            if(t == scope_t::common){
                if(ctxt.scoped()){
                    if(ctxt.context->type == scope_t::common){ this->dry = true; return; }
                    printf("Error: common scope inside other scope type\n");
                }else{
                    this->dry = false;
                    this->rank = ctxt.get_controller().get_shared_rank();
                    this->state = ambient::locality::common;
                    ctxt.push(this);
                }
            }else{
                printf("Error: unknown scope type!\n");
            }
        }
        inline scope::scope(int r) : type(scope_t::single) {
            if(ambient::ctxt.scoped()) dry = true;
            else{ dry = false; ctxt.push(this); }
            this->round = ambient::num_workers();
            this->set(r);
        }
        inline void scope::set(int r){
            this->rank = r % this->round;
            this->state = (this->rank == ambient::rank()) ? ambient::locality::local : ambient::locality::remote;
        }

        inline workflow::workflow(){
            context = this;
            int db = ambient::isset("AMBIENT_DB_NUM_PROCS") ? ambient::getint("AMBIENT_DB_NUM_PROCS") : 0;
            c.init(db);

            this->round = ambient::num_workers();
            this->state = ambient::rank() ? ambient::locality::remote : ambient::locality::local;
            this->rank  = 0;
            this->scores.resize(round, 0);

            if(ambient::isset("AMBIENT_VERBOSE")){
                ambient::cout << "ambient: initialized ("                   << AMBIENT_THREADING_TAGLINE     << ")\n";
                if(ambient::isset("AMBIENT_MKL_NUM_THREADS")) ambient::cout << "ambient: selective threading (mkl)\n";
                ambient::cout << "ambient: size of instr bulk chunks: "     << AMBIENT_INSTR_BULK_CHUNK       << "\n";
                ambient::cout << "ambient: size of data bulk chunks: "      << AMBIENT_DATA_BULK_CHUNK        << "\n";
                if(ambient::isset("AMBIENT_BULK_LIMIT")) ambient::cout << "ambient: max chunks of data bulk: " << ambient::getint("AMBIENT_BULK_LIMIT") << "\n";
                if(ambient::isset("AMBIENT_BULK_REUSE")) ambient::cout << "ambient: enabled bulk garbage collection\n";
                if(ambient::isset("AMBIENT_BULK_DEALLOCATE")) ambient::cout << "ambient: enabled bulk deallocation\n";
                ambient::cout << "ambient: maximum sid value: "             << AMBIENT_MAX_SID                << "\n";
                ambient::cout << "ambient: number of db procs: "            << ambient::num_db_procs()        << "\n";
                ambient::cout << "ambient: number of work procs: "          << ambient::num_workers()         << "\n";
                ambient::cout << "ambient: number of threads per proc: "    << ambient::num_threads()         << "\n";
                ambient::cout << "\n";
            }
            if(ambient::isset("AMBIENT_MKL_NUM_THREADS")) mkl_parallel();
        }

        inline typename workflow::controller_type& workflow::get_controller(size_t n){
            return c;
        }
        inline void workflow::sync(){
            c.flush();
            c.clear();  
            memory::data_bulk::drop();
        }
        inline bool workflow::scoped() const {
            return (context != this);
        }
        inline void workflow::push(const scope* s){
            this->context = s; // no nesting
        }
        inline void workflow::pop(){
            this->context = this;
        }
        inline bool workflow::remote() const {
            return (this->context->state == ambient::locality::remote);
        }
        inline bool workflow::local() const {
            return (this->context->state == ambient::locality::local);
        }
        inline bool workflow::common() const {
            return (this->context->state == ambient::locality::common);
        }
        inline int workflow::which() const {
            return this->context->rank;
        }
        inline void workflow::intend_read(models::ssm::revision* r){
            if(r == NULL || model_type::common(r)) return;
            this->scores[model_type::owner(r)] += r->spec.extent;
        }
        inline void workflow::intend_write(models::ssm::revision* r){
            if(r == NULL || model_type::common(r)) return;
            this->stakeholders.push_back(model_type::owner(r));
        }
        inline bool workflow::tunable() const { 
            if(c.serial) return false;
            return (context == this);
        }
        inline void workflow::schedule(){
            int max = 0;
            if(stakeholders.empty()){
                for(int i = 0; i < round; i++)
                if(scores[i] >= max){
                    max = scores[i];
                    this->rank = i;
                }
            }else{
                for(int i = 0; i < stakeholders.size(); i++){
                    int k = stakeholders[i];
                    if(scores[k] >= max){
                        max = scores[k];
                        this->rank = k;
                    }
                }
                stakeholders.clear();
            }
            std::fill(scores.begin(), scores.end(), 0);
            this->state = (this->rank == ambient::rank()) ? 
                          ambient::locality::local : ambient::locality::remote;
        }
}

#endif
