/*****************************************************************************
*
* ALPS MPS DMRG Project
*
* Copyright (C) 2014 Institute for Theoretical Physics, ETH Zurich
*               2011-2011 by Bela Bauer <bauerb@phys.ethz.ch>
*               2011-2013    Michele Dolfi <dolfim@phys.ethz.ch>
*               2014-2014    Sebastian Keller <sebkelle@phys.ethz.ch>
*               2019         Leon Freitag <lefreita@ethz.ch>
*               2019         Stefan Knecht <stknecht@ethz.ch>
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
#include "starting_guess.h"

#include "dmrg/models/measurements/chementropy.h"
#include "dmrg/utils/BaseParameters.h"
#include "dmrg/sim/matrix_types.h"

#include "dmrg/models/chem/cideas/cideas.hpp"


// namespace chem
// {
//     namespace detail
//     {
//         template <class V>
//         inline
//         BaseParameters set_su2u1_parameters(int L, int Nel, int spin, const chem::integral_map<V> & integrals, const std::vector<int> & site_types)
//         {
//             BaseParameters parms;

//             parms.set("L", L);
//             parms.set("nelec", Nel);
//             parms.set("spin", spin);

//             // take care of site types
//             std::string site_types_str;
//             assert(site_types.size() == L);
//             for (int i = 0; i < L; i++)
//                 site_types_str += std::to_string(site_types[i]) + ((i < L - 1) ? "," : "") ;
//             parms.set("site_types", site_types_str);

//             // integrals
//             parms.set("integrals_binary", chem::serialize(integrals));

//             return parms;
//         }
//     }
// }

namespace maquis
{
    template <class V>
    class StartingGuess<V>::Impl
    {
        public:
            typedef alps::numeric::matrix<V> Matrix;

#if defined(HAVE_SU2U1PG)
            typedef SU2U1PG SymmGroup;
#elif defined(HAVE_SU2U1)
            typedef SU2U1 SymmGroup;
#else
    #error SU2 symmetry (SU2U1 or SU2U1PG) must be enabled to compile DMRG interface
#endif
            Impl(const DmrgParameters& parms, const std::string& pname, int nstates, bool do_fiedler, bool do_cideas,
                 const std::vector<std::vector<int> > & hf_occupations)
                : parms_(parms), nstates_(nstates), pname_(pname), pname_guess_(pname + "_guess"), do_fiedler_(do_fiedler), do_cideas_(do_cideas)
            {

                parms_.erase_measurements();

                if (do_cideas_)
                {
                    // Make sure HF occupations are provided, or if we have only 1 state hf_occ is set before running CI-DEAS
                    if (!hf_occupations.empty())
                        assert(hf_occupations.size() == nstates);
                    else
                        if (!(nstates == 1 && parms_.is_set("hf_occ"))) // if we have only 1 state and provided the occupation in hf_occ, do nothing
                            throw std::runtime_error("CI-DEAS option requires the HF occupation to be set manually!");
                }

                if (do_fiedler_)
                {
                    if (parms_.is_set("orbital_order"))
                    {
                        // reset to default orbital order if some order is present
                        // if this isn't done, there're strange side-effects
                        int L = parms_["L"];
                        std::vector<int> v(L);
                        std::iota(v.begin(), v.end(), 1);
                        parms_.set("orbital_order", vector_tostring(v));
                    }
                    // we need mutual information for the Fiedler ordering
                    parms_.set("MEASURE[ChemEntropy]", 1);
                }

                measurements.reserve(nstates);

                // set sweeps and m, same values as in the old python interface
                parms_.set("nsweeps", 4);
                if (parms_.is_set("init_bond_dimension"))
                {
                    int init_bond_dimension = parms_["init_bond_dimension"];
                    parms_.set("max_bond_dimension", init_bond_dimension);
                }
                else
                {
                    if (parms_.is_set("L"))
                        parms_.set("max_bond_dimension", parms_["L"] > 24 ? 256 : 128);
                    else
                        throw std::runtime_error("L not defined for a starting guess calculation!");
                }

                for (int i = 0; i < nstates; i++)
                {
                    // set correct checkpoints and result file names
                    std::string chkpfile = checkpoint_name(pname_guess_, i);
                    parms_.set("chkpfile", chkpfile);
                    // std::string rfile = pname_guess_ + ".results_state." + std::to_string(i) + ".h5";
                    // parms_.set("rfile", rfile);

                    // set HF occupation
                    if (!hf_occupations.empty())
                        parms_.set("hf_occ", vector_tostring(hf_occupations[i]));

                    if (i > 0)
                    {
                        parms_.set("n_ortho_states", i-1);
                        std::string all_ortho_states;
                        for (int j = 0; j < i; j++)
                            all_ortho_states += checkpoint_name(pname_guess_, j) + ((j < i-1) ? " " : "");
                        parms_.set("ortho_states", all_ortho_states);
                    }

                    interface.reset(new DMRGInterface<V>(parms_));
                    interface->optimize();
                    measurements.emplace_back(std::move(interface->measurements()));
                }

                // Get state-average single-orbital entropies and mutual information
                // Collect mutual information from all the states

                std::vector<Matrix> mutI;

                // Calculate S1 always and mutual information only if Fiedler ordering is requested
                s1_.reserve(nstates_);
                if (do_fiedler)
                    mutI.reserve(nstates_);

                for (int i = 0; i < nstates_; i++)
                {

                    // get the entropy data
                    EntanglementData<Matrix> em(get_measurements()[i]);

                    // store data
                    s1_.emplace_back(std::move(em.s1()));
                    if (do_fiedler)
                        mutI.emplace_back(std::move(em.I()));
                }


                // Calculate average mutual information
                if (do_fiedler)
                {
                    Matrix SAmutI(mutI[0].num_rows(), mutI[0].num_cols(),0.0);
                    for (auto& n : mutI)
                        SAmutI += n;

                    // Divide mutual information by the number of states: irrelevant for Fiedler ordering
                    // but let's still do it for the consistency
                    SAmutI /= nstates;
                    SA_mutI_ = SAmutI;
                }
                // Calculate average single-orbital entropy
                Matrix SAs1(s1_[0].num_rows(), s1_[0].num_cols(),0.0);
                for (auto& n : s1_)
                    SAs1 += n;

                // Divide s1 information by the number of states
                SAs1 /= nstates;
                SA_s1_ = SAs1;
		}

            ~Impl()
            {
               // Delete checkpoint files
                for (int i = 0; i < nstates_; i++)
                {
                    std::string chkpfile = checkpoint_name(pname_guess_, i);
                    if (boost::filesystem::exists(chkpfile))
                        boost::filesystem::remove_all(chkpfile);
                }
            }

            // return single-orbital entropy for state i or the state-averaged one (i=-1)
            const Matrix& SA_s1(int i) const
            {
                if (i > 0)
                    return s1_[i];
                else
                    return SA_s1_;
            }
            //
            // return single-orbital entropy for state i or the state-averaged one (i=-1)
            // num_rows == 1!
            std::vector<V> get_s1(int k) const
            {
                int L =0;
		    if(k > 0)
			L = s1_[k].num_cols();
		    else
			L = SA_s1_.num_cols();

                std::vector<V> s1(L);
                if (k > 0){
                    for (int i = 0; i < s1_[k].num_rows(); i++)
                       for (int j = 0; j < s1_[k].num_cols(); j++)
			         s1[j] = s1_[k](i,j);
		    }
                else
		    {
                    for (int i = 0; i < SA_s1_.num_rows(); i++)
                       for (int j = 0; j < SA_s1_.num_cols(); j++)
			         s1[j] = SA_s1_(i,j);
		    }
                return s1;
            }

            // return mutual information as vector - state-averaged one
            std::vector<V> get_mutI() const
            {
                std::vector<V> mutI(SA_mutI_.num_cols()*SA_mutI_.num_rows());
   		    int nrow = SA_mutI_.num_rows();
                for (int i = 0; i < SA_mutI_.num_rows(); i++)
                    for (int j = 0; j < SA_mutI_.num_cols(); j++)
		            mutI[nrow*(i)+j] = SA_mutI_(i,j);
                return mutI;
            }

            // Get state-average mutual information
            const Matrix& SA_mutI() const
            {
                if (do_fiedler_)
                    return SA_mutI_;
                else
                    throw std::runtime_error("Please enable Fiedler ordering to calculate mutual information");
            }

            // calculate Fiedler order
            std::string getFiedlerOrder()
            {
                // TODO: implement also Block fiedler ordering per symmetry

                // get Laplacian of the average mutual information
                Matrix L = get_laplacian(SA_mutI());

                if (L.num_rows() < 2)
                    throw std::runtime_error("Fiedler vector orbital ordering doesn't work for only one orbital!");

                // get eigenvectors and eigenvalues of the Laplacian
                Matrix evecs(L.num_rows(), L.num_cols());
                std::vector<V> evals(L.num_rows());
                alps::numeric::syev(L,evecs,evals);

                // get the Fiedler vector, i.e. the eigenvector corresponding to the second lowest eigenvalue of the Laplacian
                // The eigenvalues in evecs are assumed to be sorted starting from the highest eigenvalue
                // i.e. the second lowest eigenvalue has an index L-2
                auto fv_col = evecs.col(L.num_rows()-2);
                std::vector<V> fiedler_vector(fv_col.first, fv_col.second);

                // prepare ordering. first create a vector with indices 0..L-1 in ascending order
                std::vector<int> order(fiedler_vector.size());
                std::iota(order.begin(), order.end(), 0);

                // Sort the order vector according to the Fiedler vector
                std::sort(order.begin(), order.end(), [&fiedler_vector](size_t i1, size_t i2) { return fiedler_vector[i1] < fiedler_vector[i2]; });

                // add 1 to each element because in the parameters our counting starts with 1
                for (auto&& n: order) n++;
                // std::transform(order.begin(), order.end(), order.begin(), [](int i){ return i+1; });

                // convert the ordering into a string
                return vector_tostring(order);
            }

            // perform CI-DEAS and save the resulting MPS as "pname.checkpoint_state.X.h5"
            void cideas()
            {
                if (!do_cideas_)
                    throw std::runtime_error("do_cideas option must be enabled to perform CI-DEAS");

                for (int i = 0; i < nstates_; i++)
                {
                    MPS<Matrix, SymmGroup> mps = maquis::cideas<Matrix, SymmGroup>(parms_, s1_[i]);

                    std::string chkp = checkpoint_name(pname_, i);
                    save(chkp, mps);
                    storage::archive ar(chkp+"/props.h5", "w");
                    ar["/parameters"] << parms_;
                    //TODO obtain version string from cmake
                    //ar["/version"] << DMRG_VERSION_STRING;
                    ar["/status/sweep"] << -1;
                    ar["/status/site"] << -1;
                }
            }

        private:
            DmrgParameters parms_;
            std::string pname_, pname_guess_;
            int nstates_;

            std::unique_ptr<DMRGInterface<V> > interface;
            std::vector<results_map_type<V> > measurements;

            bool do_fiedler_, do_cideas_;

            // Store single-orbital entropy and mutual information
            // Mutual information is state-average while single-orbital entropy is state-specific
            // state-average single-orbital entropy is available as well
            Matrix SA_mutI_;
            Matrix SA_s1_;
            std::vector<Matrix> s1_;

            // Calculate the Laplacian
            Matrix get_laplacian(const Matrix & mutI) const
            {
                // Ported to C++ from fiedler.py

                Matrix laplacian(mutI.num_rows(), mutI.num_cols(), 0.0);

                for (int i = 0; i < mutI.num_rows(); i++)
                    for (int j = 0; j < mutI.num_cols(); j++)
                        laplacian(i,i) += mutI(i,j);

                laplacian -= mutI;
                return laplacian;
            }

            template <class K>
            inline std::string vector_tostring(const std::vector<K> & v) const
            {
                std::string s;

                for (int i = 0; i < v.size(); i++)
                    s += std::to_string(v[i]) + ((i < v.size()-1) ? "," : "") ;
                return s;
            }

            inline
            std::string checkpoint_name(const std::string & pname, int state)
            {
                return pname + ".checkpoint_state." + std::to_string(state) + ".h5";
            }

            // Obtain measurements
            const std::vector<results_map_type<V> > & get_measurements() const { return measurements; };


    };

    template <class V>
    StartingGuess<V>::StartingGuess(const DmrgParameters& parms, int nstates, const std::string & pname, bool do_fiedler, bool do_cideas,
                 const std::vector<std::vector<int> > & hf_occupations)
        : impl_(new Impl(parms, pname, nstates, do_fiedler, do_cideas, hf_occupations)) {}

    template <class V>
    StartingGuess<V>::~StartingGuess() = default;

    template <class V>
    std::string StartingGuess<V>::getFiedlerOrder() { return impl_-> getFiedlerOrder(); }

    template<class V> 
    std::vector<V> StartingGuess<V>::get_s1(int k) { return impl_->get_s1(k); }

    template<class V> 
    std::vector<V> StartingGuess<V>::get_mutI() { return impl_->get_mutI(); }

    template<class V> void StartingGuess<V>::cideas() { impl_->cideas(); }

    template class StartingGuess<double>;
}



