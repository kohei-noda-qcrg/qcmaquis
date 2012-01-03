/*****************************************************************************
 *
 * MAQUIS DMRG Project
 *
 * Copyright (C) 2011-2011 by Bela Bauer <bauerb@phys.ethz.ch>
 *                            Michele Dolfi <dolfim@phys.ethz.ch>
 *
 *****************************************************************************/

#ifndef APP_DMRG_SIM_H
#define APP_DMRG_SIM_H

#include <cmath>
#include <iterator>
#include <iostream>
#include <sys/stat.h>

using std::cerr;
using std::cout;
using std::endl;

#include "dmrg/sim/sim.h"

#include "dmrg/mp_tensors/ss_optimize.h"

using namespace app;


//template <class Matrix, class SymmGroup>
//struct do_fix_density {
//    
//    static void apply (MPS<Matrix, SymmGroup> & mps, std::vector<block_matrix<Matrix, SymmGroup> > const & dens_ops,
//                       typename SymmGroup::charge initc)
//    { }
//};
//template <class Matrix>
//struct do_fix_density<Matrix, TwoU1> {
//    
//    static void apply (MPS<Matrix, TwoU1> & mps, std::vector<block_matrix<Matrix, TwoU1> > const & dens_ops,
//                       TwoU1::charge initc)
//    {
//        std::vector<std::vector<double> > dens(2, std::vector<double>(mps.length(), 0.));
//        std::fill(dens[0].begin(), dens[0].end(), double(initc[0])/(mps.length()/2));
//        std::fill(dens[1].begin(), dens[1].end(), double(initc[1])/(mps.length()/2));
//        
//        fix_density(mps, dens_ops, dens);
//    }
//};

template <class Matrix, class SymmGroup>
class dmrg_sim : public sim<Matrix, SymmGroup> {
    
    typedef sim<Matrix, SymmGroup> base;
    
    typedef std::vector<MPOTensor<Matrix, SymmGroup> > mpo_t;
    typedef Boundary<Matrix, SymmGroup> boundary_t;
    
public:
    
    dmrg_sim (DmrgParameters & parms_, ModelParameters & model_)
    : base(parms_, model_)
    , optimizer(base::mps,
                base::parms.template get<int>("use_compressed") == 0 ? base::mpo : base::mpoc,
                base::parms, base::ssm)
    { }
    
    
    bool do_sweep (Logger& iteration_log)
    {        
        int exit_site = optimizer.sweep(base::sweep, iteration_log);
        base::ssm.sync();
        
        base::mps = optimizer.get_current_mps();
        
        return exit_site >= 0;
    }
    
private:
    
    ss_optimize<Matrix, SymmGroup, StreamStorageMaster> optimizer;
    
};



#endif
