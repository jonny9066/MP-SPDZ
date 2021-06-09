

#ifdef TZDEBUG
#define DEBUG_PRPMUL(str) do { cout<<"PREP MUL: " << str << endl; } while( false )
#else
#define DEBUG_PRPMUL(str) do { } while ( false )
#endif

template<class T> class SubProcessor;
template<class T> class MAC_Check_Base;
class Player;

template<class T>
class Beaver : public ProtocolBase<T>
{
    vector<T> shares;
    vector<typename T::open_type> opened;
    vector<array<T, 3>> triples;
    typename vector<typename T::open_type>::iterator it;
    typename vector<array<T, 3>>::iterator triple;
    Preprocessing<T>* prep;
    typename T::MAC_Check* MC;

public:
    static const bool uses_triples = true;

    Player& P;

    Beaver(Player& P) : prep(0), MC(0), P(P) {}

    Player& branch();

    void init_mul(SubProcessor<T>* proc);
    void init_mul(Preprocessing<T>& prep, typename T::MAC_Check& MC);
    typename T::clear prepare_mul(const T& x, const T& y, int n = -1);
    void exchange();
    T finalize_mul(int n = -1);
    pair<array<typename T::open_type,2>,T> finalize_mul_prep();

    void start_exchange();
    void stop_exchange();

    int get_n_relevant_players() { return 1 + T::threshold(P.num_players()); }
};
