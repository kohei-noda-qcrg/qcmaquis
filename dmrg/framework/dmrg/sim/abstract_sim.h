/*****************************************************************************
*
* ALPS MPS DMRG Project
*
* Copyright (C) 2014 Institute for Theoretical Physics, ETH Zurich
*               2011-2011 by Bela Bauer <bauerb@phys.ethz.ch>
*               2011-2013    Michele Dolfi <dolfim@phys.ethz.ch>
*               2014-2014    Sebastian Keller <sebkelle@phys.ethz.ch>
*               2018-2019    Leon Freitag <lefreita@ethz.ch>
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
#ifndef ABSTRACT_SIM_H
#define ABSTRACT_SIM_H

#include <map>
#include <vector>
#include "integral_interface.h"
#include "dmrg/utils/results_collector.h"

class abstract_sim {
public:
    virtual ~abstract_sim() {}
    virtual void run(std::string runType) = 0;
};

/**
 * @brief Virtual class defining the scheleton of a simulation object.
 * 
 * This virtual class defines the skeleton of a simulation object 
 * that runs a DMRG calculation.
 * 
 * @tparam Matrix class defining the types of the matrix entering the definition
 * of the block_matrix.
 */

template <class Matrix>
class abstract_interface_sim {
public:
    // warning, these types are defiled in model_impl already
    typedef std::pair<std::vector<std::vector<int> >, std::vector<typename Matrix::value_type> > meas_with_results_type;
    typedef std::map<std::string, meas_with_results_type> results_map_type;

    virtual ~abstract_interface_sim() {}
    virtual void run(std::string runType) = 0;
    virtual void run_measure() = 0;
    virtual typename Matrix::value_type get_energy() = 0;
    virtual results_collector& get_iteration_results() = 0;
    virtual int get_last_sweep() = 0;
    virtual results_map_type measure_out() =0;
    virtual void update_integrals(const chem::integral_map<typename Matrix::value_type> &)=0;
    virtual typename Matrix::value_type get_overlap(const std::string &) = 0;
//  virtual std::string ... get_fiedler_order
};

#endif
