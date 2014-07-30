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

namespace ambient { namespace models { namespace ssm {

    inline history::history(dim2 dim, size_t ts) : current(NULL), dim(dim), extent(ambient::memory::aligned_64(dim.square()*ts)) { }

    inline void history::init_state(){
        revision* r = new revision(extent, NULL, ambient::locality::common, ambient::rank());
        this->current = r;
    }

    template<ambient::locality L>
    inline void history::add_state(void* g){
        revision* r = new revision(extent, g, L, ambient::rank());
        this->current = r;
    }

    template<ambient::locality L>
    inline void history::add_state(rank_t g){
        revision* r = new revision(extent, NULL, L, g);
        this->current = r;
    }

    inline revision* history::back() const {
        return this->current;
    }

    inline bool history::weak() const {
        return (this->back() == NULL);
    }

} } }
