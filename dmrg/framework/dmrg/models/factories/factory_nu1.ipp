/*****************************************************************************
 *
 * ALPS MPS DMRG Project
 *
 * Copyright (C) 2014 Institute for Theoretical Physics, ETH Zurich
 *               2017 by Alberto Baiardi <alberto.baiardi@phys.chem.ethz.ch>
 *
 * This software is part of the ALPS Applications, published under the ALPS
 * Application License; you can use, redistribute it and/or modify it under
 * the terms of the license, either version 1 or (at your option) any later
 * version.
 *
 * You should have received a copy of the ALPS Application License along with
 * the ALPS Applications; see the file LICENSE.txt. If not, the license is also
 * available from http://alps.comp-phys.org/.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

#include "dmrg/models/prebo/nu1/model.hpp"
#include "dmrg/models/factories/factory.h"

template<class Matrix, int N>
struct coded_model_factory<Matrix, NU1_template<N>> {
    static std::shared_ptr<model_impl<Matrix, NU1_template<N>> > parse
            (Lattice const& lattice, BaseParameters & parms)
    {
        typedef std::shared_ptr<model_impl<Matrix, NU1_template<N>> > impl_ptr;
        if (parms["MODEL"] == std::string("PreBO"))
            return impl_ptr( new PreBO<Matrix, N>(lattice, parms) );
        else {
            throw std::runtime_error("Don't know this model!");
            return impl_ptr();
        }
    }
};
