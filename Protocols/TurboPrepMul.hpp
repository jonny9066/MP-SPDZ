
#ifndef PROTOCOLS_BEAVER_HPP_
#define PROTOCOLS_BEAVER_HPP_

#include "Beaver.h"

#include "Replicated.hpp"

#include <array>
#include <tuple>

template<class T>
Player& Beaver<T>::branch()
{
    return P;
}

template<class T>
void Beaver<T>::init_mul(SubProcessor<T>* proc)
{
    assert(proc != 0);
    init_mul(proc->DataF, proc->MC);
}

template<class T>
void Beaver<T>::init_mul(Preprocessing<T>& prep, typename T::MAC_Check& MC)
{
    this->prep = &prep;
    this->MC = &MC;
    shares.clear();
    opened.clear();
    triples.clear();
}

// x and y are the permutation elements for wires x, y
template<class T>
typename T::clear Beaver<T>::prepare_mul(const T& x, const T& y, int n)
{
    (void) n;
    triples.push_back({{}});
    auto& triple = triples.back();
    triple = prep->get_triple(n);
    //@TZ in SPDZ is was x - triple[0]
    shares.push_back(triple[0] - x);
    shares.push_back(triple[1] - y);
    return 0;
}

template<class T>
void Beaver<T>::exchange()
{
    // throw runtime_error("Shouldn't reach here in Turbo prep")
    MC->POpen(opened, shares, P);
    it = opened.begin();
    triple = triples.begin();
}

template<class T>
void Beaver<T>::start_exchange()
{
    MC->POpen_Begin(opened, shares, P);
}

template<class T>
void Beaver<T>::stop_exchange()
{
    MC->POpen_End(opened, shares, P);
    it = opened.begin();
    triple = triples.begin();
}

//@TZ deprecated, see finalize_mul_prep
template<class T>
T Beaver<T>::finalize_mul(int n)
{
    throw runtime_error("finalize_mul deprecated in turboprep");
}

template<class T>
tuple<array<typename T::open_type,2>,T, array<T,3>> Beaver<T>::finalize_mul_prep()
{
    // save the offset values
    array<typename T::open_type,2> oval; // open permutation elements
    for (int k = 0; k < 2; k++)
        oval[k] = *it++;
    // compute z wire permutation element
    T zperm = prep->get_random();
    T& cc = (*triple)[2];
    zperm+= cc;
    // return offset values, z permutation element and associated triple
    tuple<array<typename T::open_type,2>,T, array<T,3>> res = make_tuple(oval, zperm, *triple);
    triple++;
    return res;
}
#endif