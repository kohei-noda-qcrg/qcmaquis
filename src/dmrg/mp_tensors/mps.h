#ifndef MPS_H
#define MPS_H

#include "mp_tensors/mpstensor.h"
#include "mp_tensors/mpotensor.h"

template<class Matrix, class SymmGroup>
class MPS : public std::vector<MPSTensor<Matrix, SymmGroup> >
{
public:
    typedef std::size_t size_t;
    
    MPS(size_t L, size_t Mmax, Index<SymmGroup> phys,
        typename SymmGroup::charge right_end = SymmGroup::SingletCharge);
    
    size_t length() const { return this->size(); }
    Index<SymmGroup> const & site_dim(size_t i) const { return (*this)[i].site_dim(); }
    Index<SymmGroup> const & row_dim(size_t i) const { return (*this)[i].row_dim(); }
    Index<SymmGroup> const & col_dim(size_t i) const { return (*this)[i].col_dim(); }
    
    void canonize(size_t center);
    block_matrix<Matrix, SymmGroup> canonize_left_step(size_t site);
    block_matrix<Matrix, SymmGroup> canonize_right_step(size_t site);
    
    void normalize_left();
    void normalize_right();
    
    std::string description() const;
    
    void grow_l2r_sweep(MPOTensor<Matrix, SymmGroup> const & mpo,
                        Boundary<Matrix, SymmGroup> const & left,
                        Boundary<Matrix, SymmGroup> const & right,
                        std::size_t l, double alpha,
                        double cutoff, std::size_t Mmax,
                        std::pair<std::size_t, double> & truncation);
    void grow_r2l_sweep(MPOTensor<Matrix, SymmGroup> const & mpo,
                        Boundary<Matrix, SymmGroup> const & left,
                        Boundary<Matrix, SymmGroup> const & right,
                        std::size_t l, double alpha,
                        double cutoff, std::size_t Mmax,
                        std::pair<std::size_t, double> & truncation);
    
    Boundary<Matrix, SymmGroup> left_boundary() const;
    Boundary<Matrix, SymmGroup> right_boundary() const;
    
#ifdef HAVE_ALPS_HDF5
    void serialize(alps::hdf5::iarchive & ar);
    void serialize(alps::hdf5::oarchive & ar) const;
#endif
    
private:
    typename Matrix::value_type canonize_left();
    typename Matrix::value_type canonize_right();
    
};

#include "mp_tensors/mps.hpp"

#endif
