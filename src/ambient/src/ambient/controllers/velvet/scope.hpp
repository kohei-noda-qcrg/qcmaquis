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

#ifndef AMBIENT_INTERFACE_SCOPE
#define AMBIENT_INTERFACE_SCOPE

namespace ambient { 

    using ambient::controllers::velvet::controller;

    template<scope_t T>
    class scope {};

    template<>
    class scope<base> : public controller::scope {
    public:
        scope(){
            this->round = ambient::channel.wk_dim();
            this->state = ambient::rank() ? ambient::remote : ambient::local;
            this->sector = 0;
            this->factor = AMBIENT_SCOPE_SWITCH_FACTOR;
            this->op_alloc = 0;
            this->op_transfer = 0;
        }
        virtual bool tunable() const { 
            return false; // can be enabled but the algorithm should be finalized
        }
        virtual void consider_allocation(size_t size) const {
            this->op_alloc += size;
        }
        virtual void consider_transfer(size_t size, ambient::locality l) const {
            if(l == ambient::common) return;
            this->op_transfer += size;
        }
        virtual void toss(){
            if(this->op_transfer < this->op_alloc){
                if(this->factor < this->op_alloc){
                    this->factor = AMBIENT_SCOPE_SWITCH_FACTOR;
                    ++this->sector %= this->round;
                    this->state = (this->sector == ambient::rank()) ? 
                                  ambient::local : ambient::remote;
                }else{
                    this->factor -= this->op_alloc;
                }
            }
            this->op_alloc = 0;
            this->op_transfer = 0;
        }
        size_t factor;
        mutable size_t op_alloc;
        mutable size_t op_transfer;
        int round;
    };

    template<>
    class scope<single> : public controller::scope {
    public:
        scope(int value = 0) : index(value), iterator(value) {
            if(ambient::controller.context != ambient::controller.context_base) dry = true;
            else{ dry = false; ambient::controller.set_context(this); }
            this->round = ambient::channel.wk_dim();
            this->shift();
        }
        void shift(){
            this->sector = (++this->iterator %= this->round);
            this->state = (this->sector == ambient::rank()) ? ambient::local : ambient::remote;
            if(this->sector == 0 && this->round > 1) this->shift();
        }
        scope& operator++ (){
            this->shift();
            this->index++;
            return *this;
        }
        operator size_t (){
            return index;
        }
        bool operator < (size_t lim){
            return index < lim;
        }
       ~scope(){
            if(!dry) ambient::controller.pop_context();
        }
        virtual bool tunable() const {
            return false; 
        }
        size_t index;
        bool dry;
        int iterator;
        int round;
    };

    template<>
    class scope<funnel> : public controller::scope {
    public:
        scope(){
            this->sector = -1;
        }
        scope(int s){
            this->latch(s);
        }
        scope& operator = (const scope<single>& s){
            this->sector = s.sector;
            this->state = s.state;
            return *this;
        }
        void latch(int s){
            this->sector = s;
            this->state = (s == ambient::rank()) ? ambient::local : ambient::remote;
        }
        void push() const {
            parent = ambient::controller.context;
            if(this->sector == -1) printf("Warning: uninitialized context!\n");
            else ambient::controller.set_context(this);
        }
        void pop() const {
            ambient::controller.set_context(parent);
        }
        virtual bool tunable() const {
            return false; 
        }
        mutable const controller::scope* parent;
    };

    template<>
    class scope<dedicated> : public controller::scope {
    public:
        scope(){
            ambient::controller.set_context(this);
            this->sector = ambient::rank.dedicated();
            this->state = (this->sector == ambient::rank()) ? ambient::local : ambient::remote;
        }
       ~scope(){
            ambient::controller.pop_context();
        }
        virtual bool tunable() const {
            return false; 
        }
    };

    template<>
    class scope<shared> : public controller::scope {
    public:
        scope(){
            ambient::controller.set_context(this);
            this->state = ambient::common;
        }
       ~scope(){
            ambient::controller.pop_context();
        }
        virtual bool tunable() const { 
            return false; 
        }
    };
}

#endif
