/*
 * Input.cpp
 *
 */


#ifndef PROCESSOR_INPUT_HPP_
#define PROCESSOR_INPUT_HPP_

#include <typeinfo>
#include <algorithm> // for reverse

#include "Input.h"
#include "Processor.h"

#include "IntInput.h"
#include "FixInput.h"
#include "FloatInput.h"

#include "IntInput.hpp"


template<class T>
InputBase<T>::InputBase(ArithmeticProcessor* proc) :
        P(0), values_input(0)
{
    if (proc)
        buffer.setup(&proc->private_input, -1, proc->private_input_filename);
}

template<class T>
InputBase<T>::InputBase(SubProcessor<T>* proc) :
        InputBase(proc ? proc->Proc : 0)
{
}

template<class T>
Input<T>::Input(SubProcessor<T>& proc) :
        Input(proc, proc.MC)
{
}

template<class T>
Input<T>::Input(SubProcessor<T>& proc, MAC_Check& mc) :
        InputBase<T>(proc.Proc), proc(&proc), MC(mc), prep(proc.DataF), P(proc.P),
        shares(proc.P.num_players()),rndshr(P.num_players())
{
}

template<class T>
Input<T>::Input(SubProcessor<T>* proc, Player& P) :
        InputBase<T>(proc->Proc), proc(proc), MC(proc->MC), prep(proc->DataF), P(
                proc->P), shares(P.num_players()), rndshr(P.num_players())
{
    assert (proc != 0);
}

template<class T>
Input<T>::Input(MAC_Check& MC, Preprocessing<T>& prep, Player& P) :
        proc(0), MC(MC), prep(prep), P(P), shares(P.num_players()), rndshr(P.num_players())
{
}

template<class T>
InputBase<T>::~InputBase()
{
#ifdef VERBOSE
    if (timer.elapsed() > 0)
        cerr << T::type_string() << " inputs: " << timer.elapsed() << endl;
#endif
}

template<class T>
void Input<T>::reset(int player)
{
    InputBase<T>::reset(player);
    shares[player].clear();
}

template<class T>
void InputBase<T>::reset(int player)
{
    os.resize(max(os.size(), player + 1UL));
    os[player].reset_write_head();
}

template<class T>
void InputBase<T>::reset_all(Player& P)
{
    this->P = &P;
    os.resize(P.num_players());
    for (int i = 0; i < P.num_players(); i++)
        reset(i);
}

template<class T>
void Input<T>::add_mine(const open_type& input, int n_bits)
{
    (void) n_bits;
    int player = P.my_num();

    //get corresponding random share and value from prep
    rr = rand.next(); 
    T& rrshare = rndshr[player].next();

    // t here is external value for the wire
    t = input + rr;
    extvals.push_back(t);
    // broadcast t
    t.pack(this->os[player]);

    // create share of input
    shares[player].push_back(T::constant(t, player, MC.get_alphai()));
    T& share = shares[player].back();
    share = share-rrshare;
    
    this->values_input++;
}

template<class T>
void Input<T>::send_mine()
{
    P.send_all(this->os[P.my_num()]);
}

template<class T>
void InputBase<T>::exchange()
{
    for (int i = 0; i < P->num_players(); i++)
        if (i == P->my_num())
            send_mine();
        else
            P->receive_player(i, os[i]);
}

template<class T>
T Input<T>::finalize_mine()
{
    return shares[P.my_num()].next();
}

template<class T>
typename T::open_type Input<T>::finalize_mine_ext()
{
    return extvals.next();
}

// get references to share and extval and pass them on this way
template<class T>
void Input<T>::finalize_other_tzonline(int player, T& target, typename T::open_type& ev,
        octetStream& o, int n_bits)
{
    (void) n_bits;
    target = rndshr[player].next();
    t.unpack(o);
    ev = t; // save external value
    target =  T::constant(t, P.my_num(), MC.get_alphai()) - target; // create share
}

template<class T>
pair<T, typename T::open_type> InputBase<T>::finalize_tzonline(int player, int n_bits)
{
    if (player == P->my_num())
        return {finalize_mine(), finalize_mine_ext()};
    else
    {
        T res;
        typename T::open_type extval;
        finalize_other_tzonline(player, res, extval, os[player], n_bits);
        return {res, extval};
    }
}

template<class T>
template<class U>
void InputBase<T>::prepare(SubProcessor<T>& Proc, int player, const int* params,
        int size)
{
    auto& input = Proc.input;
    assert(Proc.Proc != 0);
    if (player == Proc.P.my_num())
    {
        for (int j = 0; j < size; j++)
        {
            U tuple = Proc.Proc->template get_input<U>(Proc.Proc->use_stdin(),
                    params);
            for (auto x : tuple.items)
                input.add_mine(x);
        }
    }
    //@TZ online deprecated
    // else
    // {
    //     for (int j = 0; j < U::N_DEST * size; j++)
    //         input.add_other(player);
    // }
}

// load the values in an order they can be used in add_mine, add_other
// which means reverse of what they were in prep
template<class T>
template<class U>
void InputBase<T>::init_input_turbo_online(SubProcessor<T>& Proc, int player, const int* dest,
        int size)
{
    auto& input = Proc.input;
    auto& rand = input.get_rand();
    auto& rndshr = input.get_rndshr();
    for (int k = 0; k < size; k++){
        for (int j = 0; j < U::N_DEST; j++){
            // @TZ load preprocessing data into
            if (player == Proc.P.my_num()) // if my share
                rand.push_back(Proc.get_Rand_ref(dest[j] + k));
            rndshr[player].push_back(Proc.get_Perm_ref(dest[j] + k));
        }
    }
    
}

//
template<class T>
template<class U>
void InputBase<T>::finalize(SubProcessor<T>& Proc, int player, const int* dest,
        int size)
{
    auto& input = Proc.input;
    for (int k = 0; k < size; k++){
        for (int j = 0; j < U::N_DEST; j++){
            pair<T, typename T::open_type> psv = input.finalize_tzonline(player);
            Proc.get_S_ref(dest[j] + k) = psv.first;
            Proc.get_E_ref(dest[j] + k) = psv.second;
            }
        }
}


template<class T>
int InputBase<T>::get_player(SubProcessor<T>& Proc, int arg, bool player_from_reg)
{
   if (player_from_reg)
   {
       assert(Proc.Proc);
       auto res = Proc.Proc->read_Ci(arg);
       if (res >= Proc.P.num_players())
           throw runtime_error("player id too large: " + to_string(res));
       return res;
   }
   else
       return arg;
}

template<class T>
void InputBase<T>::input_mixed(SubProcessor<T>& Proc, const vector<int>& args,
    int size, bool player_from_reg)
{
    auto& input = Proc.input;
    input.reset_all(Proc.P);
    int last_type = -1;

    //copied finalize section to reuse that code,
    // we will initialize vectors with data from prep
    for (size_t i = 0; i < args.size();)
    {
        int n_arg_tuple;
        int type = args[i];
        int player;
        switch (type)
        {
#define X(U) \
        case U::TYPE: \
            n_arg_tuple = U::N_DEST + U::N_PARAM + 2; \
            player = get_player(Proc, args[i + n_arg_tuple - 1], player_from_reg); \
            init_input_turbo_online<U>(Proc, player, &args[i + 1], size); \
            break;
        X(IntInput<typename T::clear>) X(FixInput) X(FloatInput)
#undef X
        default:
            throw unknown_input_type(type);
        }
        i += n_arg_tuple;
    }

    for (size_t i = 0; i < args.size();)
    {
        int n_arg_tuple;
        int type = args[i];
        int player;
        switch (type)
        {
#undef X
#define X(U) \
        case U::TYPE: \
            n_arg_tuple = U::N_DEST + U::N_PARAM + 2; \
            player = get_player(Proc, args[i + n_arg_tuple - 1], player_from_reg); \
            if (type != last_type and Proc.Proc and Proc.Proc->use_stdin()) \
                cout << "Please input " << U::NAME << "s:" << endl; \
            prepare<U>(Proc, player, &args[i + U::N_DEST + 1], size); \
            break;
        X(IntInput<typename T::clear>) X(FixInput) X(FloatInput)
#undef X
        default:
            throw unknown_input_type(type);
        }
        i += n_arg_tuple;
        last_type = type;
    }

    input.exchange();

    for (size_t i = 0; i < args.size();)
    {
        int n_arg_tuple;
        int type = args[i];
        int player;
        switch (type)
        {
#define X(U) \
        case U::TYPE: \
            n_arg_tuple = U::N_DEST + U::N_PARAM + 2; \
            player = get_player(Proc, args[i + n_arg_tuple - 1], player_from_reg); \
            finalize<U>(Proc, player, &args[i + 1], size); \
            break;
        X(IntInput<typename T::clear>) X(FixInput) X(FloatInput)
#undef X
        default:
            throw unknown_input_type(type);
        }
        i += n_arg_tuple;
    }
}

//@ depreated stuff TZ online


template<class T>
void InputBase<T>::raw_input(SubProcessor<T>& proc, const vector<int>& args,
        int size)
{
    throw runtime_error("raw_input deprecated in TZ online");
    auto& P = proc.P;
    reset_all(P);

    for (auto it = args.begin(); it != args.end();)
    {
        int player = *it++;
        it++;
        if (player == P.my_num())
        {
            for (int i = 0; i < size; i++)
            {
                clear t;
                try
                {
                    this->buffer.input(t);
                }
                catch (not_enough_to_buffer& e)
                {
                    throw runtime_error("Insufficient input data to buffer");
                }
                add_mine(t);
            }
        }
        else
        {
            for (int i = 0; i < size; i++)
                add_other(player);
        }
    }

    timer.start();
    exchange();
    timer.stop();

    for (auto it = args.begin(); it != args.end();)
    {
        int player = *it++;
        int base = *it++;
        for (int i = 0; i < size; i++)
            proc.get_S_ref(base + i) = finalize(player);
    }
}


template<class T>
template<class U>
void InputBase<T>::input(SubProcessor<T>& Proc,
        const vector<int>& args, int size)
{
    throw runtime_error("input deprecated in TZ online");

    auto& input = Proc.input;
    input.reset_all(Proc.P);
    int n_arg_tuple = U::N_DEST + U::N_PARAM + 1;
    assert(args.size() % n_arg_tuple == 0);

    int n_from_me = 0;

    if (Proc.Proc and Proc.Proc->use_stdin())
    {
        for (size_t i = n_arg_tuple - 1; i < args.size(); i += n_arg_tuple)
            n_from_me += (args[i] == Proc.P.my_num()) * size;
        if (n_from_me > 0)
            cout << "Please input " << n_from_me << " " << U::NAME << "(s):" << endl;
    }

    for (size_t i = U::N_DEST; i < args.size(); i += n_arg_tuple)
    {
        int n = args[i + U::N_PARAM];
        InputBase<T>::prepare<U>(Proc, n, &args[i], size);
    }

    if (n_from_me > 0)
        cout << "Thank you" << endl;

    input.exchange();

    for (size_t i = 0; i < args.size(); i += n_arg_tuple)
    {
        int player = args[i + n_arg_tuple - 1];
        finalize<U>(Proc, player, &args[i], size);
    }
}

//@TZ replaced with finalize_tzonline
template<class T>
T InputBase<T>::finalize(int player, int n_bits)
{
    throw runtime_error("finalize deprecated in TZ online");
    if (player == P->my_num())
        return finalize_mine();
    else
    {
        T res;
        finalize_other(player, res, os[player], n_bits);
        return res;
    }
}


template<class T>
void Input<T>::add_other(int player)
{
    throw runtime_error("add_other deprecated in TZ online");
    // @TZ same as in SPDZ, not sure if helps here
    open_type t;
    shares.at(player).push_back({});
    // gets share of other player's input
    prep.get_input(shares[player].back(), t, player);
}

template<class T>
void InputBase<T>::add_from_all(const clear& input)
{
    throw runtime_error("add_from_all deprecated in TZ online");
    for (int i = 0; i < P->num_players(); i++)
        if (i == P->my_num())
            add_mine(input);
        else
            add_other(i);
}
template<class T>
void Input<T>::finalize_other(int player, T& target, 
        octetStream& o, int n_bits)
{
    throw runtime_error("finalize_other deprecated in TZ online");
    (void) n_bits;
    target = shares[player].next();
    t.unpack(o);
    T extval = T::constant(t, P.my_num(), MC.get_alphai());
    target =  extval - target;
}



#endif 

