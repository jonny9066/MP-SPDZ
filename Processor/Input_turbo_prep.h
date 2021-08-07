
#ifndef PROCESSOR_INPUT_H_
#define PROCESSOR_INPUT_H_

#include <vector>
#include <tuple>
using namespace std;

#include "Tools/Buffer.h"
#include "Tools/time-func.h"
#include "Tools/PointerVector.h"

// #include "Processor.h"
// #include "Processor/DummyProtocol.h"//@TZ
#include "Math/gfp.h"//@TZ
#include "Protocols/Share.h"//@TZ

#ifdef TZDEBUG
#define DEBUG_IN(str) do { cout<<"INPUT: " << str << endl; } while( false )
#else
#define DEBUG_IN(str) do { } while ( false )
#endif


class ArithmeticProcessor;

template<class T>
class InputBase
{
    typedef typename T::clear clear;

protected:
    Player* P;

    Buffer<typename T::clear, typename T::clear> buffer;
    Timer timer;

public:
    // @TZ for testing
    typedef Share<gfp_<0, 2>> uShare;   
    typedef gfp_<0, 2> uClear;  

    // vector of octetStram(s) for every player
    // used to broadcast values
    vector<octetStream> os;
    int values_input;

    template<class U>
    static void input(SubProcessor<T>& Proc, const vector<int>& args, int size);

    static int get_player(SubProcessor<T>& Proc, int arg, bool player_from_arg);
    static void input_mixed(SubProcessor<T>& Proc, const vector<int>& args,
            int size, bool player_from_reg);
    template<class U>
    static void prepare(SubProcessor<T>& Proc, int player, const int* params, int size);
    template<class U>
    static void finalize(SubProcessor<T>& Proc, int player, const int* params, int size);

    InputBase(ArithmeticProcessor* proc = 0);
    InputBase(SubProcessor<T>* proc);
    virtual ~InputBase();

    virtual void reset(int player) = 0;
    void reset_all(Player& P);

    virtual void add_mine(const typename T::open_type& input, int n_bits = -1) = 0;
    virtual void add_other(int player) = 0;
    void add_from_all(const clear& input);

    virtual void send_mine() = 0;
    virtual void exchange();

    virtual T finalize_mine() = 0;
    //@TZ does the same as finalize_mine
    virtual typename T::open_type finalize_mine_rand(){
        throw runtime_error("finalize_mine_rand not implemented");
    }
    virtual void finalize_other(int player, T& target, octetStream& o, int n_bits = -1) = 0;
    virtual T finalize(int player, int n_bits = -1);
    //@TZ returns the random value and its share
    virtual tuple<T, typename T::open_type, bool> finalize_tzprep(int player, int n_bits = -1);

    void raw_input(SubProcessor<T>& proc, const vector<int>& args, int size);
};

// T is the 'Share' type
template<class T>
class Input : public InputBase<T>
{
    // @TZ for testing
    typedef Share<gfp_<0, 2>> uShare;   
    typedef gfp_<0, 2> uClear;  
    
    typedef typename T::open_type open_type;
    typedef typename T::clear clear;
    typedef typename T::MAC_Check MAC_Check;

    SubProcessor<T>* proc;
    MAC_Check& MC;
    Preprocessing<T>& prep;
    Player& P;
    vector< PointerVector<T> > shares;
    PointerVector<open_type> rand; //@TZ stores my random vals
    open_type rr, t, xi;

public:
    Input(SubProcessor<T>& proc);
    Input(SubProcessor<T>& proc, MAC_Check& mc);
    Input(SubProcessor<T>* proc, Player& P);
    Input(MAC_Check& MC, Preprocessing<T>& prep, Player& P);

    void reset(int player);

    void add_mine(const open_type& input, int n_bits = -1);
    void add_other(int player);

    void send_mine();

    T finalize_mine();
    open_type finalize_mine_rand();
    void finalize_other(int player, T& target, octetStream& o, int n_bits = -1);
};

#endif