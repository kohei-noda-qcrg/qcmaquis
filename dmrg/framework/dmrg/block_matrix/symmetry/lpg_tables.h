/*****************************************************************************
 *
 * ALPS MPS DMRG Project
 *
 * Copyright (C) 2013 Institute for Theoretical Physics, ETH Zurich
 *               2013-2013 by Bela Bauer <bauerb@phys.ethz.ch>
 *                            Sebastian Keller <sebkelleb@phys.ethz.ch>
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

#ifndef LPG_TABLES_H
#define LPG_TABLES_H

#include <cmath>
#include <vector>
#include <alps/numeric/matrix.hpp>

template<class S>
std::vector<S> generate_adjoin_table_Ci()
{
    int num_irreps = 4;
    std::vector<S> adjoin_table(num_irreps);
    adjoin_table.push_back(0);
    adjoin_table.push_back(1);
    adjoin_table.push_back(2);
    adjoin_table.push_back(3);

    return adjoin_table;
}

template<class S>
alps::numeric::matrix<S> generate_mult_table_Ci()
{
    int num_irreps = 4;
    alps::numeric::matrix<S> mult_table(num_irreps,num_irreps);
    mult_table(0,0) = 0;
    mult_table(0,1) = 1;
    mult_table(0,2) = 2;
    mult_table(0,3) = 3;
    mult_table(1,0) = 1;
    mult_table(1,1) = 0;
    mult_table(1,2) = 3;
    mult_table(1,3) = 2;
    mult_table(2,0) = 2;
    mult_table(2,1) = 3;
    mult_table(2,2) = 0;
    mult_table(2,3) = 1;
    mult_table(3,0) = 3;
    mult_table(3,1) = 2;
    mult_table(3,2) = 1;
    mult_table(3,3) = 0;

    return mult_table;
}

template<class S>
std::vector<S> generate_adjoin_table_Cs_C2()
{
    int num_irreps = 4;
    std::vector<S> adjoin_table(num_irreps);
    for(int i = 0; i < num_irreps/2; ++i)
        adjoin_table[i] = i;

    for(int i = num_irreps/2; i < num_irreps; ++i)
        adjoin_table[i] = i + pow(-1,i);

    return adjoin_table;
}

template<class S>
alps::numeric::matrix<S> generate_mult_table_Cs_C2()
{
    int num_irreps = 4;
    alps::numeric::matrix<S> mult_table(num_irreps,num_irreps);
    mult_table(0,0) = 0;
    mult_table(0,1) = 1;
    mult_table(0,2) = 2;
    mult_table(0,3) = 3;
    mult_table(1,0) = 1;
    mult_table(1,1) = 0;
    mult_table(1,2) = 3;
    mult_table(1,3) = 2;
    mult_table(2,0) = 2;
    mult_table(2,1) = 3;
    mult_table(2,2) = 1;
    mult_table(2,3) = 0;
    mult_table(3,0) = 3;
    mult_table(3,1) = 2;
    mult_table(3,2) = 0;
    mult_table(3,3) = 1;

    return mult_table;
}

template<class S>
std::vector<S> generate_adjoin_table_C2h()
{
    int num_irreps = 8;
    std::vector<S> adjoin_table(num_irreps);
    for(int i = 0; i < num_irreps/2; ++i)
        adjoin_table[i] = i;

    for(int i = num_irreps/2; i < num_irreps; ++i)
        adjoin_table[i] = i + pow(-1,i);

    return adjoin_table;
}

template<class S>
alps::numeric::matrix<S> generate_mult_table_C2h()
{
    int num_irreps = 8;
    alps::numeric::matrix<S> mult_table(num_irreps,num_irreps);
    for(int j = 0; j < num_irreps; ++j) {
        for(int i = 0; i < num_irreps; ++i) {
            if(i < 4 && j < 4) {
                if(i == j)
                    mult_table(i,j) = 0;
                else if( (i+j == 2) || (i+j == 6) )
                    mult_table(i,j) = 1;
                else if( i+j == 4)
                    mult_table(i,j) = 3;
                else
                    mult_table(i,j) = 2;
            } else if(i >= 4 && j >= 4) {
                if(i == j)
                    mult_table(i,j) = 1;
                else if( i+j == 12)
                    mult_table(i,j) = 2;
                else if( (i+j == 10) || (i+j == 15) )
                    mult_table(i,j) = 0;
                else
                    mult_table(i,j) = 3;
            } else {
                if(i+j == 8)
                    mult_table(i,j) = 7;
                else if( (i+j == 6) || (i+j == 10) )
                    mult_table(i,j) = 5;
                else if( (i+j == 5) || (i+j == 11) )
                    mult_table(i,j) = 4;
                else if( (i == 5) && (j == 1) )
                    mult_table(i,j) = 4;
                else if( (i == 1) && (j == 5) )
                    mult_table(i,j) = 4;
                else if( (i == 2) && (j == 6) )
                    mult_table(i,j) = 4;
                else if( (i == 6) && (j == 2) )
                    mult_table(i,j) = 4;
                else
                    mult_table(i,j) = 6;
            }
        }
    }
    return mult_table;
}

template<class S>
std::vector<S> generate_adjoin_table_Cinf()
{
	//TODO: num_irreps should be a member of the symmetry class
	//      accessible from outside
    int num_irreps = 128;

    std::vector<S> adjoin_table(num_irreps);

    // boson irreps
    adjoin_table[0] = 0;
    adjoin_table[63] = 63;

    for (int i = 1; i < num_irreps/2 - 1; ++i) {
        adjoin_table[i] = i + pow(-1,i+1);
    }

    // fermion irreps
    for (int i = num_irreps/2; i < num_irreps; ++i) {
        adjoin_table[i] = i + pow(-1,i);
    }

    return adjoin_table;
}
  
template<class S>
alps::numeric::matrix<S> generate_mult_table_Cinf()
{
	//TODO: num_irreps should be a member of the symmetry class
	//      accessible from outside
    int num_irreps = 128;
    
	if(num_irreps/2 % 2 == 1){
        throw std::logic_error("Number of boson and fermion irreps must be even\n");}
    int shift = num_irreps/2;
    int irrep = 1;
    int mj = 0;
    std::vector<S> mj2rep(num_irreps+1);
    alps::numeric::matrix<S> mult_table(num_irreps,num_irreps);
    mj2rep[shift+mj] = irrep;
    
    // populate mj2rep vector with boson and fermion irreps
    for(mj = 2; mj <= num_irreps/2-2; mj+=2){
        irrep++;
        mj2rep[shift+mj] = irrep;
        irrep++;
        mj2rep[shift-mj] = irrep;
    }

    mj = num_irreps/2;
    irrep++;
    mj2rep[shift+mj] = irrep;
    mj2rep[shift-mj] = irrep;

    for(mj = 1; mj <= num_irreps/2-1; mj+=2){
        irrep++;
        mj2rep[shift+mj] = irrep;
        irrep++;
        mj2rep[shift-mj] = irrep;
    }

    // build multiplication table
    int mij = 0;
    int jrrep = 0;
    int ijrrep = 0;
    for(int mi = -num_irreps/2; mi <= num_irreps/2; mi++){
        for(mj = -num_irreps/2; mj <= num_irreps/2; mj++){
            mij = mi + mj;
            if(mij <  -num_irreps/2){mij = mij + num_irreps;}
            if(mij >   num_irreps/2){mij = mij - num_irreps;}
            if(mij == -num_irreps/2){mij = num_irreps/2;}
            irrep  = mj2rep[shift+mi];
            jrrep  = mj2rep[shift+mj];
            ijrrep = mj2rep[shift+mij];
            mult_table(irrep-1,jrrep-1) = ijrrep-1;
        }
    }

    return mult_table;
}

#endif

