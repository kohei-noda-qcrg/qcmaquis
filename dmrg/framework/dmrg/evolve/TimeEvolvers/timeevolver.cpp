/*****************************************************************************
 *
 * ALPS Project: Algorithms and Libraries for Physics Simulations
 *
 * ALPS Libraries
 *
 * Copyright (C) 2021 by Alberto Baiardi <abaiardi@ethz.ch>
 *
 * This software is part of the ALPS libraries, published under the ALPS
 * Library License; you can use, redistribute it and/or modify it under
 * the terms of the license, either version 1 or (at your option) any later
 * version.
 *
 * You should have received a copy of the ALPS Library License along with
 * the ALPS Libraries; see the file LICENSE.txt. If not, the license is also
 * available from http://alps.comp-phys.org/.
 *
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

#include <memory>

// -- Empty constructor --
/*
template<class Matrix, class SymmGroup, class ParameterType>
TimeEvolver<Matrix, SymmGroup, ParameterType>::TimeEvolver() : accuracy_(1.0E-6), is_imag_(false), max_iterations_(100),
                                                               time_step_(1.)
{
    time_step_ /= (2.418884326505E-2*219474.63E0) ;
    time_step_ /= 2. ;
};
*/

// -- Constructor from parameter object --
template<class Matrix, class SymmGroup, class ParameterType>
TimeEvolver<Matrix, SymmGroup, ParameterType>::TimeEvolver(ParameterType& parms) : accuracy_(parms["propagator_accuracy"]),
                                                                                   is_imag_(false), has_td_part_(false),
                                                                                   max_iterations_(parms["propagator_maxiter"]),
                                                                                   time_step_(parms["time_step"]),
                                                                                   time_current_(0.)
{
  if (parms["imaginary_time"] == "yes")
    is_imag_ = true;
  time_step_ /= 2.;
  // Set the time step. Check also all relevant units conversion.
  if (parms["hamiltonian_units"] == "Hartree") {
    if (parms["time_units"] == "fs")
      time_step_ *= 41.341374575751;
    else if (parms["time_units"] == "as")
      time_step_ *= 0.041341374575751;
    else
      throw std::runtime_error("Units for the time variable not yet supported");
  }
  else {
    throw std::runtime_error("Units for the Hamiltonian not yet supported");
  }
  // Checks if it has a TD part
  if (parms.is_set("TD_perturbation")) {
    has_td_part_ = true;
    std::string intAlgo = parms["TD_integration_algorithm"];
    if (intAlgo == "RungeKutta") {
      time_evolution_algorithm_ = std::make_unique< RKEvolver<Matrix, SymmGroup> >(time_step_, has_td_part_, is_imag_);
    }
    else if (intAlgo == "EMR2") {
      time_evolution_algorithm_ = std::make_unique< LanczosEMR >(time_step_, has_td_part_, is_imag_, accuracy_, max_iterations_);
      // This must be decommented once it works.
      //useExtrapolated_ = true;
    }
    else if (intAlgo == "CF4") {
      time_evolution_algorithm_ = std::make_unique< LanczosFourthOrder>(time_step_, has_td_part_, is_imag_, accuracy_, max_iterations_);
    }
    else {
      throw std::runtime_error("TD integration algorithm not recognized");
    }
  } else {
    time_evolution_algorithm_ = std::make_unique< LanczosTI >(time_step_, has_td_part_, is_imag_, accuracy_, max_iterations_);
  }
};

template<class Matrix, class SymmGroup, class ParameterType>
typename TimeEvolver<Matrix, SymmGroup, ParameterType>::time_type
         TimeEvolver<Matrix, SymmGroup, ParameterType>::get_time() const
{
  return this->time_step_;
}

template<class Matrix, class SymmGroup, class ParameterType>
typename TimeEvolver<Matrix, SymmGroup, ParameterType>::time_type
         TimeEvolver<Matrix, SymmGroup, ParameterType>::get_time_current()
{
  return this->time_current_;
}

template<class Matrix, class SymmGroup, class ParameterType>
void TimeEvolver<Matrix, SymmGroup, ParameterType>::add_to_current_time(time_type time_step)
{
  time_current_ += time_step;
}

// +-------+
//  METHODS
// +-------+

template<class Matrix, class SymmGroup, class ParameterType>
template<class SiteProblem, class MatrixType>
void TimeEvolver<Matrix, SymmGroup, ParameterType>::evolve(SiteProblem const& site_problem, MatrixType& matrix,
                                                           bool is_forward) const
{
  time_evolution_algorithm_->evolve(site_problem, matrix, is_forward, time_current_, time_step_);
}