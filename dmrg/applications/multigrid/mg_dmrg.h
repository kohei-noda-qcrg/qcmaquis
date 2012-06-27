/*****************************************************************************
 *
 * MAQUIS DMRG Project
 *
 * Copyright (C) 2011-2011 by Michele Dolfi <dolfim@phys.ethz.ch>
 *
 *****************************************************************************/

#ifndef APP_MG_DMRG_H
#define APP_MG_DMRG_H

#include "dmrg/utils/DmrgParameters2.h"

#include "dmrg/block_matrix/detail/alps_matrix.hpp"
#ifdef USE_MTM
#include "types/mt_matrix/mt_matrix.h"
typedef alps::numeric::mt_matrix<double> Matrix;
#else

#ifndef USE_COMPLEX
typedef alps::numeric::matrix<double> Matrix;
#else
typedef alps::numeric::matrix<std::complex<double> > Matrix;
#endif

#endif

#include "dmrg/block_matrix/symmetry.h"


// def. of run functions
template <class SymmGroup>
void run_mg_dmrg(DmrgParameters & parms, ModelParameters & model);

#endif
