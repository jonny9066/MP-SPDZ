
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

template<class T>
typename T::clear Beaver<T>::prepare_mul(const T& x, const T& y, int n)
{
    (void) n;
    triples.push_back({{}});
    auto& triple = triples.back();
    triple = prep->get_triple(n);
    //@TZ triple[0] - x instead of x - triple[0]
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

template<class T>
T Beaver<T>::finalize_mul(int n)
{
    (void) n;
    typename T::open_type masked[2];
    T& tmp = (*triple)[2];
    for (int k = 0; k < 2; k++)
    {   
        masked[k] = *it++;
        tmp += (masked[k] * (*triple)[1 - k]);
    }
    tmp += T::constant(masked[0] * masked[1], P.my_num(), MC->get_alphai());
    triple++;
    return tmp;
}

template<class T>
pair<array<typename T::open_type,2>,T> Beaver<T>::finalize_mul_prep()
{
    // typename T::open_type open_perm[2]; // open permutation elements
    array<typename T::open_type,2> open_perm; // open permutation elements
    T& c = (*triple)[2];
    for (int k = 0; k < 2; k++)
        open_perm[k] = *it++;
    T zz = prep->get_random();
    zz+= c;
    pair<array<typename T::open_type,2>,T> res = {open_perm, zz};
    triple++;
    return res;
}