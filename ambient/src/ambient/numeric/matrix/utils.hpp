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

#ifndef AMBIENT_NUMERIC_UTILS
#define AMBIENT_NUMERIC_UTILS
    
namespace ambient {

    template<class Matrix>
    inline void touch(const numeric::tiles<Matrix>& a){
        int size = a.data.size();
        for(int i = 0; i < size; i++){
            touch(a[i]);
        }
    }

    template<class Matrix>
    inline void migrate(const numeric::tiles<Matrix>& a){
        numeric::tiles<Matrix>& m = const_cast<numeric::tiles<Matrix>&>(a);
        int size = m.data.size();
        for(int i = 0; i < size; i++){
            migrate(m[i]);
        }
    }

    template<class Matrix>
    inline void hint(const numeric::tiles<Matrix>& a){
        int size = a.data.size();
        for(int i = 0; i < size; i++){
            hint(a[i]);
        }
    }

    template<class Matrix>
    inline rank_t get_owner(const numeric::tiles<Matrix>& a){
        return ambient::get_owner(a[0]);
    }

    template<class InputIterator, class Function, class Weight>
    void for_each_redist(InputIterator first, InputIterator last, Function op, Weight weight){
        typedef std::pair<double,size_t> pair;
        size_t range = last-first;
        size_t np = ambient::num_procs();
        double avg = 0.;

        if(range < np) return; // one element per proc
        std::vector<pair> wl(np, std::make_pair(0,0));
        std::vector<pair> cx; cx.reserve(range);
        
        // calculating complexities of the calls
        for(InputIterator it = first; it != last; ++it){
            cx.push_back(std::make_pair(weight(*it), it-first));
            avg += cx.back().first;
        }
        avg /= np;
        std::sort(cx.begin(), cx.end(), 
                  [](const pair& a, const pair& b){ return a.first < b.first; });
        
        // filling the workload with smallest local parts first
        for(size_t p = 0; p < np; ++p){
            wl[p].second = p;
            ambient::actor proc(ambient::scope::begin()+p);
            for(size_t i = 0; i < range; ++i){
                size_t k = cx[i].second;
                if(ambient::get_owner(*(first+k)) != p) continue;
                if(wl[p].first + cx[i].first >= avg) continue;
                op(*(first+k)); 
                wl[p].first += cx[i].first; 
                cx[i].first = 0;
            }
        }
        // rebalancing using difference with average
        for(size_t p = 0; p < np; ++p){
            ambient::actor proc(ambient::scope::begin()+p);
            for(size_t i = 0; i < range; ++i){
                if(cx[i].first == 0) continue;
                if(wl[p].first + cx[i].first >= avg) break;
                op(*(first+cx[i].second)); 
                wl[p].first += cx[i].first; 
                cx[i].first = 0;
            }
        }
        std::sort(wl.begin(), wl.end(), 
                  [](const pair& a, const pair& b){ return a.first < b.first; });
        
        // placing the rest according to sorted workload
        size_t p = 0;
        for(int i = range-1; i >= 0; --i){
            if(cx[i].first == 0) continue;
            ambient::actor proc(ambient::scope::begin()+wl[p++].second);
            op(*(first+cx[i].second)); 
            p %= np;
        }
    }
            
}

#endif
