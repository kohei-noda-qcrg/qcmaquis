/*****************************************************************************
 *
 * MAQUIS DMRG Project
 *
 * Copyright (C) 2011-2013 by Michele Dolfi <dolfim@phys.ethz.ch>
 *                            Bela Bauer <bauerb@comp-phys.org>
 *
 *****************************************************************************/

#ifdef USE_AMBIENT
#include <mpi.h>
#endif
#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/shared_ptr.hpp>

#include <alps/hdf5.hpp>

#include "matrix_selector.hpp" /// define matrix
#include "symm_selector.hpp"   /// define grp

#include "dmrg/models/lattice.h"
#include "dmrg/models/model.h"
#include "dmrg/models/generate_mpo.hpp"

#include "dmrg/mp_tensors/mps.h"
#include "dmrg/utils/DmrgParameters2.h"

#include "utils/timings.h"


int main(int argc, char ** argv)
{
    try {
        if (argc != 3)
            throw std::runtime_error("Usage: <parms> <model_parms>");
        
        /// Loading parameters
        std::ifstream param_file(argv[1]);
        if (!param_file)
            throw std::runtime_error("Could not open parameter file.");
        DmrgParameters parms(param_file);
        
        /// Loading model
        std::ifstream model_file(argv[2]);
        if (!model_file)
            throw std::runtime_error("Could not open model file.");
        ModelParameters model_parms(model_file);
        
        /// Timers
#ifdef AMBIENT
        ambient::synctime
#else
        Timer
#endif
        tim_model      ("Parsing model"),  tim_load       ("Load MPS"),
        tim_l_boundary ("Left boundary"),  tim_r_boundary ("Right boundary"),
        tim_optim_jcd  ("Optim. JCD"   ),  tim_truncation ("Truncation"),
        tim_ts_obj     ("Two site obj.");
        
        /// Parsing model
        tim_model.begin();
        Lattice lattice(parms, model_parms);
        Model<matrix, grp> model(lattice, parms, model_parms);
        
        MPO<matrix, grp> mpo = make_mpo(lattice, model, model_parms);
        tim_model.end();
        maquis::cout << "Parsing model done!\n";
        
        
        /// Initialize & load MPS
        tim_load.begin();
        int L = lattice.size();
        MPS<matrix, grp> mps;
        load(parms["chkpfile"].str(), mps);
        int _site;
        {
            alps::hdf5::archive ar(parms["chkpfile"].str()+"/props.h5");
            ar["/status/site"] >> _site;
        }
        int site, lr;
        if (_site < L) {
            site = _site;
            lr = 1;
        } else {
            site = 2*L-_site-1;
            lr = -1;
        }
        tim_load.end();
        maquis::cout << "Load MPS done!\n";
        maquis::cout << "Optimization at site " << site << " in " << lr << " direction." << std::endl;
        
        mps.canonize(site);

        /// Compute left boundary
        tim_l_boundary.begin();
        Boundary<matrix, grp> left = mps.left_boundary();
        for (size_t i=0; i<site; ++i)
            left = contraction::overlap_mpo_left_step(mps[i], mps[i], left, mpo[i]);
        {
            std::string fname = "left" + boost::lexical_cast<std::string>(site) + ".h5";
            storage::archive ar(parms["chkpfile"].str()+"/"+fname, "w");
            ar["/props/site"] << site;
            ar["/props/type"] << "left";
            ar["/tensor"]     << left;
        }
        tim_l_boundary.end();
        maquis::cout << "Left boundary done!\n";
        
        /// Compute right boundary
        tim_r_boundary.begin();
        Boundary<matrix, grp> right = mps.right_boundary();
        for (int i=L-1; i>site+1; --i)
            right = contraction::overlap_mpo_right_step(mps[i], mps[i], right, mpo[i]);
        {
            std::string fname = "right" + boost::lexical_cast<std::string>(site+2) + ".h5";
            storage::archive ar(parms["chkpfile"].str()+"/"+fname, "w");
            ar["/props/site"] << site+2;
            ar["/props/type"] << "right";
            ar["/tensor"]     << right;
        }
        tim_r_boundary.end();
        maquis::cout << "Right boundary done!\n";
        
    } catch (std::exception & e) {
        maquis::cerr << "Exception caught:" << std::endl << e.what() << std::endl;
        exit(1);
    }
}

