/*****************************************************************************
*
* ALPS MPS DMRG Project
*
* Copyright (C) 2014 Institute for Theoretical Physics, ETH Zurich
*               2011-2011 by Bela Bauer <bauerb@phys.ethz.ch>
*               2011-2013    Michele Dolfi <dolfim@phys.ethz.ch>
*               2014-2014    Sebastian Keller <sebkelle@phys.ethz.ch>
*               2018-2019    Leon Freitag <lefreita@ethz.ch>
*               2020-        Robin Feldmann <robinfe@phys.chem.ethz.ch>
*               2021-        Alberto Baiardi <abaiardi@ethz.ch>
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

#include "maquis_dmrg.h"
#include <complex>

#include "dmrg/sim/symmetry_factory.h"
#include "dmrg/sim/matrix_types.h"
#include "dmrg/sim/interface_sim.h"

namespace maquis
{
    #if defined(HAVE_SU2U1PG)
    typedef SU2U1PG SU2U1grp;
    typedef TwoU1PG TwoU1grp;
    #elif defined(HAVE_SU2U1)
    typedef SU2U1 SU2U1grp;
    typedef TwoU1 TwoU1grp;
    #endif

    template<class V>
    struct simulation_traits {
        typedef std::shared_ptr<abstract_interface_sim<tmatrix<V> > > shared_ptr;
        template <class SymmGroup> struct F {
            typedef interface_sim<tmatrix<V>, SymmGroup> type;
        };
    };

    template <class V, Hamiltonian HamiltonianType>
    struct DMRGInterface<V, HamiltonianType>::Impl
    {
        typedef typename simulation_traits<V>::shared_ptr sim_ptr;
        sim_ptr sim;

        Impl(sim_ptr sim_) : sim(sim_) {};
        ~Impl() = default;
    };

    template <class V, Hamiltonian HamiltonianType>
    DMRGInterface<V, HamiltonianType>::DMRGInterface(DmrgParameters & parms_)
        : parms(parms_), impl_(new Impl(::dmrg::symmetry_factory<simulation_traits<V> >(parms_, parms_))) {};

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::optimize()
    {
        try {
            //std::cout << "start optimization inside interface --> " <<std::endl;
            //std::cout << " parms are -> " << std::endl;
            //std::cout << parms << std::endl;
            //sim->run();
            impl_->sim->run("optimize");
        }
        catch (std::exception & e) {
            maquis::cerr << "Exception thrown!" << std::endl;
            maquis::cerr << e.what() << std::endl;
            exit(1);
        }
    }

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::evolve()
    {
        try {
            impl_->sim->run("evolve");
        }
        catch (std::exception & e) {
            maquis::cerr << "Exception thrown!" << std::endl;
            maquis::cerr << e.what() << std::endl;
            exit(1);
        }
    }

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::run_measure()
    {
        try
        {
            impl_->sim->run_measure();
        }
        catch (std::exception & e)
        {
            maquis::cerr << "Exception thrown!" << std::endl;
            maquis::cerr << e.what() << std::endl;
            exit(1);
        }
    }

    template <class V, Hamiltonian HamiltonianType>
    V DMRGInterface<V, HamiltonianType>::energy()
    {
        return impl_->sim->get_energy();
    }

    template <class V, Hamiltonian HamiltonianType>
    results_collector& DMRGInterface<V, HamiltonianType>::get_iteration_results()
    {
        return impl_->sim->get_iteration_results();
    }

    template <class V, Hamiltonian HamiltonianType>
    int DMRGInterface<V, HamiltonianType>::get_last_sweep()
    {
        return impl_->sim->get_last_sweep();
    }

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::measure()
    {
        measurements_ = impl_->sim->measure_out();
    }

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::update_integrals(const integral_map<V> & integrals)
    {
        impl_->sim->update_integrals(integrals);
    }

    template <class V, Hamiltonian HamiltonianType>
    const typename DMRGInterface<V, HamiltonianType>::results_map_type& DMRGInterface<V, HamiltonianType>::measurements()
    {
        if (measurements_.empty())
            measure();
        // This is probably not going to work if we call optimize() several times
        // TODO: handle also these cases!
        return measurements_;
    };

    template <class V, Hamiltonian HamiltonianType>
    const typename DMRGInterface<V, HamiltonianType>::meas_with_results_type& DMRGInterface<V, HamiltonianType>::mutinf()
    {
        return measurements().at("mutinf");
    }

    // TODO: This does not work for 2U1/2U1PG symmetry because "oneptdm" measurement is not recognised by the model!
    // Fix the model to recognise it!
    template <class V, Hamiltonian HamiltonianType>
    const typename DMRGInterface<V, HamiltonianType>::meas_with_results_type& DMRGInterface<V, HamiltonianType>::onerdm()
    {
        return measurements().at("oneptdm");
    }

    template <class V, Hamiltonian HamiltonianType>
    const typename DMRGInterface<V, HamiltonianType>::meas_with_results_type& DMRGInterface<V, HamiltonianType>::onespdm()
    {
        return measurements().at("oneptspdm");
    }

    template <class V, Hamiltonian HamiltonianType>
    const typename DMRGInterface<V, HamiltonianType>::meas_with_results_type& DMRGInterface<V, HamiltonianType>::twordm()
    {
        return measurements().at("twoptdm");
    }

    template <class V, Hamiltonian HamiltonianType>
    const typename DMRGInterface<V, HamiltonianType>::meas_with_results_type& DMRGInterface<V, HamiltonianType>::threerdm()
    {
        parms.set("MEASURE[3rdm]", 1); // required for 3-RDM measurement
        return measurements().at("threeptdm");
    }

    template <class V, Hamiltonian HamiltonianType>
    const typename DMRGInterface<V, HamiltonianType>::meas_with_results_type& DMRGInterface<V, HamiltonianType>::fourrdm()
    {
        parms.set("MEASURE[4rdm]", 1); // required for 4-RDM measurement
        return measurements().at("fourptdm");
    }

    #define measure_and_save_rdm(N) \
        BaseParameters meas_parms = parms.measurements(); \
        parms.erase_measurements(); \
        parms.set("MEASURE[" #N "rdm]", 1); \
        impl_->sim->run_measure(); \
        parms.erase_measurements(); \
        parms << meas_parms

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::measure_and_save_3rdm()
    {
        measure_and_save_rdm(3);
    }

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::measure_and_save_4rdm()
    {
        // Clear all unnecessary measurements before running 4-RDM measurement
        // FIXME: clearing parms here has NO EFFECT on the measurements! This has to be changed in another way!
        // For now the measurements are modified in maquis_cinterface.cpp, but it won't work if DMRGInterface is called directly!
        // Back up measurements
        measure_and_save_rdm(4);
    }

    #undef measure_and_save_rdm

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::measure_and_save_trans3rdm(const std::string & bra_name)
    {
        BaseParameters meas_parms = parms.measurements();
        parms.erase_measurements();
        parms.set("MEASURE[trans3rdm]", bra_name);
        impl_->sim->run_measure();
        parms.erase_measurements();
        parms << meas_parms;
    }

    template <class V, Hamiltonian HamiltonianType>
    DMRGInterface<V, HamiltonianType>::~DMRGInterface() = default;

    template <class V, Hamiltonian HamiltonianType>
    V DMRGInterface<V, HamiltonianType>::overlap(const std::string& aux_mps_name)
    {
        return impl_->sim->get_overlap(aux_mps_name);
    }

    template <class V, Hamiltonian HamiltonianType>
    void DMRGInterface<V, HamiltonianType>::dump_parameters(const std::string & file)
    {
        std::ofstream fs(file);
        fs << parms;
    }

    template class DMRGInterface<double>;
    template class DMRGInterface<std::complex<double> >;
    template class DMRGInterface<double, Hamiltonian::PreBO>;
}
